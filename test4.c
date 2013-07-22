#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include "allegro5/allegro_image.h"
//#include "allegro5/allegro_native_dialog.h"
#include "projectile.c"

//#define ROOT
#define SCREEN_W 800
#define SCREEN_H 600
#define MARGIN 300
#define FPS 60
#define PROJ_POOL_SIZE 1000
#define ENEMY_POOL_SIZE 100
#define SHOT_COOLDOWN 8
#define ENEMY_COOLDOWN 2
#define SHOT_SPREAD 0
#define SHOT_OFFSET_X 69
#define SHOT_OFFSET_Y 20
#define BACKDROP_H 800
#define BACKDROP_W 1280
#define STARNUMBER 200
 
enum MYKEYS {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_SPACE};

int offscreen(PROJP pp)
{
	return proj_offscreen(pp, SCREEN_W, SCREEN_H, MARGIN);
}

int main()
{
	int i;
	short fps = 0;
	short logicTick = 0;
	float stdv = 3.1;
	//Value to scale x and y vel by if both keys are pressed.
	float diagscale = 0.71;
	int backdropx = 0;
	int backdropy = (SCREEN_H - BACKDROP_H) / 2;

	ALLEGRO_DISPLAY *display   = NULL;
	ALLEGRO_BITMAP *backdrop = NULL;
	ALLEGRO_BITMAP *star = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;

	bool key[5] = { false, false, false, false };
	bool redraw = true;
	bool doexit = false;

	//Initializing a bunch of modules.

	init_random();

	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
 
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to initialize the keyboard!\n");
		return -1;
	}
 
	timer = al_create_timer(1.0 / FPS);
	if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}

	if(!al_init_image_addon()) {
		//al_show_native_message_box(display, "Error", "Error", "Failed to initialize al_init_image_addon!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}
 
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	//Loading in the textures.
	star = al_load_bitmap("pixel.png");
	backdrop = al_load_bitmap("MoonBrushTest.jpg");
	if (!backdrop) printf("Backdrop failed to load.\n");

	short boltFrame = 0;
	ALLEGRO_BITMAP *bolt[12];
	for (i = 0; i < 12; i++) {
		bolt[i] = NULL;
		char path[] = "Bolt/bolt0000.png\0";
		path[11] = i/10 + '0';
		path[12] = i%10 + '0';
		bolt[i] = al_load_bitmap(path);
	}

	//Hacky test of animation.
	ALLEGRO_BITMAP *asteroid[60];
	for (i = 0; i < 60; i++) {
		asteroid[i] = NULL;
		//printf("Loading %2i: ", i);
		char path[] = "Aster4/aster0000.png\0";
		path[14] = i/10 + '0';
		path[15] = i%10 + '0';
		asteroid[i] = al_load_bitmap(path);
		//if (asteroid[i]) printf("Successfully loaded frame %2i from %s\n", i, path);
		if (!(al_get_bitmap_flags(asteroid[i]) & ALLEGRO_VIDEO_BITMAP))
				printf("Asteroid %2i is not hardware-accelerated!\n", i);
	}

	//Ship animation frames.
	short shipFrame = 29;
	short shipFramesetSwap = 60;
	ALLEGRO_BITMAP *shipFrames[120];
	for (i = 0; i < 60; i++) {
		shipFrames[i] = NULL;
		char path[] = "Ship1/ship0000.png\0";
		path[12] = i/10 + '0';
		path[13] = i%10 + '0';
		shipFrames[i] = al_load_bitmap(path);
	}
	//These are an alternate set with a different seed value for flares.
	//That way the ship rockets never appear static.
	for (i = 0; i < 60; i++) {
		shipFrames[i+60] = NULL;
		char path[] = "Ship2/ship0000.png\0";
		path[12] = i/10 + '0';
		path[13] = i%10 + '0';
		shipFrames[i+60] = al_load_bitmap(path);
	}

	//Setup the ship projectile.
	PROJP ship = init_projectile();
	ship->pos->x = SCREEN_W / 4;
	ship->pos->y = SCREEN_H / 2;
	int ship_cooldown = SHOT_COOLDOWN;

	//Setup the ship's laser pool.
	SPP sl_pool = init_smartprojpool(PROJ_POOL_SIZE);

	//Setup some enemy shots for test purposes.
	SPP en_pool = init_smartprojpool(ENEMY_POOL_SIZE);
	int enemy_cooldown = ENEMY_COOLDOWN;

	//Setup a pool for small starlike debris.
	SPP smallDebris = init_smartprojpool(STARNUMBER);

	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}
 
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_start_timer(timer);
 
	while(!doexit) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		//The ship's standard velocity.
		
		if(ev.type == ALLEGRO_EVENT_TIMER) {

			if (ship_cooldown > 0) ship_cooldown--;
			if (enemy_cooldown > 0) enemy_cooldown--;

			shipFramesetSwap^=60;

			if (logicTick >= 60) {
				printf("FPS: %2i, Pool Size: %4i\n", fps, en_pool->liveIndex);
				fps = logicTick = 0;
			}
			logicTick++;

			give_vel(ship, zero_vec);
			//Checking ship movement keys.
			if(key[KEY_UP]) {
				ship->vel->y -= stdv;
				if (shipFrame < 58) shipFrame+= 3;
				if (backdropy < 0) backdropy++;
			}
			if(key[KEY_DOWN]) {
				ship->vel->y += stdv;
				if (shipFrame > 1) shipFrame-= 3;
				if (backdropy + BACKDROP_H > SCREEN_H) backdropy--;
			}
			if(key[KEY_LEFT]) {
				ship->vel->x -= stdv;
				if (backdropx < 0) backdropx++;
			}
			if(key[KEY_RIGHT]) {
				ship->vel->x += stdv;
				if (backdropx + BACKDROP_W > SCREEN_W) backdropx--;
			}

			if (ship->vel->x && ship->vel->y) {
				ship->vel->x *= diagscale;
				ship->vel->y *= diagscale;
			}

			if (shipFrame > 29) shipFrame-=2;
			if (shipFrame < 29) shipFrame+=2;

			//Update the ship's position.
			ship->pos->x += ship->vel->x;
			ship->pos->y += ship->vel->y;

			//Update the ship's laser pool projectile positions.
			int (*offscreenp)(PROJP pp);
			offscreenp = &offscreen;
			update_pool_positions(sl_pool, offscreenp);

			//Creation and swapping of laser projectiles.
			if (key[KEY_SPACE] && !ship_cooldown && sl_pool->liveIndex < sl_pool->poolsize) {
				PROJP new = sl_pool->pool[sl_pool->liveIndex];
				give_pos(new, ship->pos);
				//give_vel(sl_pool->pool[sl_pool->liveIndex], ship->vel);
				give_vel(new, zero_vec);
				new->vel->x += 15;
				sl_pool->liveIndex++;
				if (ship_cooldown == 0) ship_cooldown = SHOT_COOLDOWN;
			}
			
			//Update the temporary debris positions.
			update_pool_positions(en_pool, offscreenp);
			
			for (i = 0; i < 10; i++) {
				//Creation and swapping of temporary debris.
				if (!enemy_cooldown && en_pool->liveIndex < en_pool->poolsize) {
					VECTOR random_vert_pos = {SCREEN_W + MARGIN, (rand() % SCREEN_H)};
					VECTOR random_vel = {(rand() % 3) - 4, (rand() % 5) - 2};
					PROJP new = en_pool->pool[en_pool->liveIndex];

					give_pos(new, &random_vert_pos);
					give_vel(new, &random_vel);
					en_pool->liveIndex++;
					if (enemy_cooldown == 0) enemy_cooldown = ENEMY_COOLDOWN;
					//printf("%i\n", en_pool->liveIndex);
				}
			}

			//Spawning of small "star" particles.
			update_pool_positions(smallDebris, offscreenp);
			for (i = 0; i < 10; i++) {
				//Creation and swapping of laser projectiles.
				if (smallDebris->liveIndex < smallDebris->poolsize) {
					VECTOR random_vert_pos = {SCREEN_W + MARGIN, rand() % SCREEN_H};
					VECTOR random_vel = {(rand() % 8) - 13, 0};
					PROJP new = smallDebris->pool[smallDebris->liveIndex];

					give_pos(new, &random_vert_pos);
					give_vel(new, &random_vel);
					smallDebris->liveIndex++;
				}
			}

			redraw = true;
		}
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
		break;
	}
	else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
		switch(ev.keyboard.keycode) {
			case ALLEGRO_KEY_W:
				key[KEY_UP] = true;
				break;
	 
			case ALLEGRO_KEY_S:
				key[KEY_DOWN] = true;
				break;
	 
			case ALLEGRO_KEY_A: 
				key[KEY_LEFT] = true;
				break;
	 
			case ALLEGRO_KEY_D:
				key[KEY_RIGHT] = true;
				break;

			case ALLEGRO_KEY_SPACE:
				key[KEY_SPACE] = true;
				break;
		}
	}
	else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
		switch(ev.keyboard.keycode) {
			case ALLEGRO_KEY_W:
				key[KEY_UP] = false;
				break;
		 
			case ALLEGRO_KEY_S:
				key[KEY_DOWN] = false;
				break;
		 
			case ALLEGRO_KEY_A: 
				key[KEY_LEFT] = false;
				break;
		 
			case ALLEGRO_KEY_D:
				key[KEY_RIGHT] = false;
				break;

			case ALLEGRO_KEY_SPACE:
				key[KEY_SPACE] = false;
				break;
		 
			case ALLEGRO_KEY_ESCAPE:
				doexit = true;
				break;
			}
	}
 
	if(redraw && al_is_event_queue_empty(event_queue)) {
		redraw = false;
		//al_clear_to_color(al_map_rgb(255,255,255));
		al_draw_bitmap(backdrop, backdropx, backdropy, 0);

		//Draw half the little star/debris bits.
		for (i = 0; i < smallDebris->liveIndex/2; i++) {
			al_draw_bitmap(star, smallDebris->pool[i]->pos->x+backdropx, smallDebris->pool[i]->pos->y+backdropy, 0);
		}

		for (i = 0; i < en_pool->liveIndex; i++) {
			//printf("Attempting to draw asteroid %4i\n", i);
			al_draw_bitmap(asteroid[en_pool->pool[i]->animFrame], en_pool->pool[i]->pos->x + backdropx, en_pool->pool[i]->pos->y + backdropy, 0);
			//printf("Successfully drew asteroid  %4i\n", i);
		}

		for (i = 0; i < sl_pool->liveIndex; i++) {
			al_draw_bitmap(bolt[boltFrame=(++boltFrame%12)], sl_pool->pool[i]->pos->x+SHOT_OFFSET_X, sl_pool->pool[i]->pos->y+SHOT_OFFSET_Y, 0);
		}

		al_draw_bitmap(shipFrames[(int)shipFrame+shipFramesetSwap], ship->pos->x, ship->pos->y, 0);

		//Draw the other half of the little star/debris bits.
		for (i = smallDebris->liveIndex/2; i < smallDebris->liveIndex; i++) {
			al_draw_bitmap(star, smallDebris->pool[i]->pos->x+backdropx, smallDebris->pool[i]->pos->y+backdropy, 0);
		}
		
		al_flip_display();
		fps++;
		}
	}
	for (i = 0; i < 60; i++) al_destroy_bitmap(asteroid[i]);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	free_smartprojpool(sl_pool);
	free_smartprojpool(en_pool);

	return 0;
}
