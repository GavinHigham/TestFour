//C stuff.
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h> 
//Allegro stuff.
#include <allegro5/allegro.h>
#include "allegro5/allegro_image.h"
//My stuff.
#include "definitions.h"
#include "projectile.c"
#include "collision.c"
#include "init.c"
#include "tex_load.c"
#include "logic_tick.c"

int main()
{
	//Initializing a bunch of modules.
	if (init_stuff()) return -1;

	//Loading in the textures.
	if (load_textures()) return -1;

	//Setup the ship projectile.
	ship->pos->x = SCREEN_W / 4;
	ship->pos->y = SCREEN_H / 2;

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
		
		if (ev.type == ALLEGRO_EVENT_TIMER) {
			logic_tick();
			redraw = true;
		}
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) break;

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

			for (i = 0; i < ast_pool->liveIndex; i++) {
				//printf("Attempting to draw asteroid %4i\n", i);
				al_draw_bitmap(asteroid[ast_pool->pool[i]->animFrame], ast_pool->pool[i]->pos->x, ast_pool->pool[i]->pos->y, 0);
				//printf("Successfully drew asteroid  %4i\n", i);
			}

			for (i = 0; i < sl_pool->liveIndex; i++) {
				al_draw_bitmap(bolt[boltFrame=(++boltFrame%12)], sl_pool->pool[i]->pos->x, sl_pool->pool[i]->pos->y, 0);
			}

			//Draw the ship.
			al_draw_bitmap(shipFrames[(int)shipFrame+shipFramesetSwap], ship->pos->x, ship->pos->y, 0);
			
			/*
			for (i = 0; i < blast_pool->liveIndex; i++) {
				al_draw_bitmap(blastFrames[36 - blast_pool->pool[i]->health], blast_pool->pool[i]->pos->x, blast_pool->pool[i]->pos->y, 0);
			}
			*/

			al_flip_display();
			fps++;
		}
	}

	//Free all the things!
	for (i = 0; i < 60; i++)  al_destroy_bitmap(asteroid[i]);
	for (i = 0; i < 12; i++)  al_destroy_bitmap(bolt[i]);
	for (i = 0; i < 120; i++) al_destroy_bitmap(shipFrames[i]);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	//free_projectile(ship);
	free_smartprojpool(sl_pool);
	free_smartprojpool(ast_pool);
	//free_smartprojpool(blast_pool);
	free_smartnodepool(node_pool);

	return 0;
}
