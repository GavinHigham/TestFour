#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_native_dialog.h"
#include "projectile.c"

//#define ROOT
#define SCREEN_W 800
#define SCREEN_H 600
#define MARGIN 100
#define FPS 60
#define PROJ_POOL_SIZE 100
#define ENEMY_POOL_SIZE 2000
#define SHOT_COOLDOWN 3
#define ENEMY_COOLDOWN 0
#define SHOT_SPREAD 0
#define SHOT_OFFSET_X 73
#define SHOT_OFFSET_Y 29
 
enum MYKEYS {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_SPACE};

int offscreen(PROJP pp)
{
	return proj_offscreen(pp, SCREEN_W, SCREEN_H, MARGIN);
}

int main(int argc, char **argv)
{
	int red = 0;
	int green = 0;
	int blue = 0;
	if (argc == 4) {
		red = atoi(argv[1]);
		green = atoi(argv[2]);
		blue = atoi(argv[3]);
	}
	int i;

	ALLEGRO_DISPLAY *display   = NULL;
	ALLEGRO_BITMAP *ship_image = NULL;
	ALLEGRO_BITMAP *shot_image = NULL;
	ALLEGRO_BITMAP *enemy_shot_image = NULL;
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
		al_show_native_message_box(display, "Error", "Error", "Failed to initialize al_init_image_addon!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}
 
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	//Loading in the textures.
	ship_image = al_load_bitmap("testship.png");
	shot_image = al_load_bitmap("ship_shot.png");
	enemy_shot_image = al_load_bitmap("enemy_shot.png");

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
 
 	//Checking for loading errors.
	if(!ship_image) {
		al_show_native_message_box(display, "Error", "Error", "Failed to load image!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(display);
		return 0;
	}

	if(!shot_image) {
		al_show_native_message_box(display, "Error", "Error", "Failed to load image!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(display);
		return 0;
	}

	if(!enemy_shot_image) {
		al_show_native_message_box(display, "Error", "Error", "Failed to load image!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(display);
		return 0;
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

	al_draw_bitmap(ship_image, ship->pos->x,ship->pos->y, 0);
	al_flip_display();

	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_bitmap(shot_image);
		al_destroy_bitmap(ship_image);
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
		int stdv = 3;
		if(ev.type == ALLEGRO_EVENT_TIMER) {

			if (ship_cooldown > 0) ship_cooldown--;
			if (enemy_cooldown > 0) enemy_cooldown--;

			give_vel(ship, zero_vec);
			//Checking ship movement keys.
			if(key[KEY_UP])    ship->vel->y = -stdv;
			if(key[KEY_DOWN])  ship->vel->y =  stdv;
			if(key[KEY_LEFT])  ship->vel->x = -stdv;
			if(key[KEY_RIGHT]) ship->vel->x =  stdv;

			//Update the ship's position.
			ship->pos->x += ship->vel->x;
			ship->pos->y += ship->vel->y;

			//Update the ship's laser pool projectile positions.
			int (*offscreenp)(PROJP pp);
			offscreenp = &offscreen;
			update_pool_positions(sl_pool, offscreenp);

			//Creation and swapping of laser projectiles.
			if(key[KEY_SPACE] && !ship_cooldown && sl_pool->liveIndex < sl_pool->poolsize) {
				give_pos(sl_pool->pool[sl_pool->liveIndex], ship->pos);
				give_vel(sl_pool->pool[sl_pool->liveIndex], ship->vel);
				sl_pool->pool[sl_pool->liveIndex]->vel->x += 15;
				sl_pool->liveIndex++;
				if (ship_cooldown == 0) ship_cooldown = SHOT_COOLDOWN;
			}

			//Update the temporary debris positions.
			update_pool_positions(en_pool, offscreenp);
			
			for (i = 0; i < 10; i++) {
				//Creation and swapping of temporary debris.
				if (!enemy_cooldown && en_pool->liveIndex < en_pool->poolsize) {
					VECTOR random_vert_pos = {SCREEN_W, (rand() % SCREEN_H)};
					VECTOR random_vel = {(rand() % 3), (rand() % 5) - 2};

					give_pos(en_pool->pool[en_pool->liveIndex], &random_vert_pos);
					give_vel(en_pool->pool[en_pool->liveIndex], &random_vel);
					en_pool->pool[en_pool->liveIndex]->vel->x -= 4;
					en_pool->liveIndex++;
					if (enemy_cooldown == 0) enemy_cooldown = ENEMY_COOLDOWN;
					//printf("%i\n", en_pool->liveIndex);
				}
			}


			redraw = true;
		}
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
		break;
	}
	else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
		switch(ev.keyboard.keycode) {
			case ALLEGRO_KEY_UP:
				key[KEY_UP] = true;
				break;
	 
			case ALLEGRO_KEY_DOWN:
				key[KEY_DOWN] = true;
				break;
	 
			case ALLEGRO_KEY_LEFT: 
				key[KEY_LEFT] = true;
				break;
	 
			case ALLEGRO_KEY_RIGHT:
				key[KEY_RIGHT] = true;
				break;

			case ALLEGRO_KEY_SPACE:
				key[KEY_SPACE] = true;
				break;
		}
	}
	else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
		switch(ev.keyboard.keycode) {
			case ALLEGRO_KEY_UP:
				key[KEY_UP] = false;
				break;
		 
			case ALLEGRO_KEY_DOWN:
				key[KEY_DOWN] = false;
				break;
		 
			case ALLEGRO_KEY_LEFT: 
				key[KEY_LEFT] = false;
				break;
		 
			case ALLEGRO_KEY_RIGHT:
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
		al_clear_to_color(al_map_rgb(red, green, blue));
		for (i = 0; i < en_pool->liveIndex; i++) {
			//printf("Attempting to draw asteroid %4i\n", i);
			al_draw_bitmap(asteroid[en_pool->pool[i]->animFrame], en_pool->pool[i]->pos->x, en_pool->pool[i]->pos->y, 0);
			//printf("Successfully drew asteroid  %4i\n", i);
		}
		for (i = 0; i < sl_pool->liveIndex; i++)
			al_draw_bitmap(shot_image, sl_pool->pool[i]->pos->x+SHOT_OFFSET_X, sl_pool->pool[i]->pos->y+SHOT_OFFSET_Y, 0);
		al_draw_bitmap(ship_image, ship->pos->x, ship->pos->y, 0);
		
		al_flip_display();
		}
	}
	for (i = 0; i < 60; i++) al_destroy_bitmap(asteroid[i]);
	al_destroy_bitmap(ship_image);
	al_destroy_bitmap(enemy_shot_image);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	free_smartprojpool(sl_pool);
	free_smartprojpool(en_pool);

	return 0;
}
