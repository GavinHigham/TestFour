#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h> 

typedef struct vector {
   int x;
   int y;
} VECTOR, *VP;

typedef struct projectile {
   VP pos;
   VP vel;
   int animFrame;
} PROJ, *PROJP;

typedef struct smartpool {
	PROJP *pool;
	int poolsize;
	int liveIndex;
} SMARTPOOL, *SPP;

VECTOR struct_zero_vector = {0, 0};
VP zero_vec = &struct_zero_vector;

struct timeval tv;

void init_random() {
	//This is just for getting random numbers.                                                              
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec);
}

PROJP init_projectile()
{
	PROJP pp = malloc(sizeof(PROJ));
	//pp->live = 0;
	pp->pos = malloc(sizeof(VECTOR));
	pp->pos->x = 0;
	pp->pos->y = 0;
	pp->vel = malloc(sizeof(VECTOR));
	pp->vel->x = 0;
	pp->vel->y = 0;
	pp->animFrame = rand() % 60;
	return pp;
}

void free_projectile(PROJP pp)
{
	free(pp->pos);
	free(pp->vel);
	free(pp);
}

void init_projectile_pool(PROJP pool[], int count) {
	int i;
	for (i = 0; i < count; i++) {
		pool[i] = init_projectile();
	}
}

void free_projectile_pool(PROJP pool[], int count)
{
	int i;
	for (i = 0; i < count; i++) free_projectile(pool[i]);
}

//This one swaps the actual structs at two pointers.
void swap_proj(PROJP a, PROJP b)
{
   PROJ tmp = *a;
   *a = *b;
   *b = tmp;
}

void give_pos(PROJP pp, VP pos)
{
	pp->pos->x = pos->x;
	pp->pos->y = pos->y;
}

void give_vel(PROJP pp, VP vel)
{
	pp->vel->x = vel->x;
	pp->vel->y = vel->y;
}

//This one swaps two pointers in an array.
void swap_array_proj(PROJP pool[], int a, int b)
{
	PROJP temp = pool[a];
	pool[a] = pool[b];
	pool[b] = temp;
}

void update_proj_position(PROJP pp)
{
	pp->pos->x += pp->vel->x;
	pp->pos->y += pp->vel->y;
}

//returns 1 if a projectile is offscreen, 0 otherwise.
//Margin is how many pixels beyond the screen the proj should be to return 1.
int proj_offscreen(PROJP pp, int screen_w, int screen_h, int margin)
{
	if ((pp->pos->x > (screen_w + margin)) || (pp->pos->x < (0 - margin)) ||
		(pp->pos->y > (screen_h + margin)) || (pp->pos->y < (0 - margin)) ) {
		return 1;
	}
	else return 0;
}

SPP init_smartprojpool(int count)
{
	SPP sp = malloc(sizeof(SMARTPOOL));
	sp->pool = malloc(sizeof(PROJP)*count);
	init_projectile_pool(sp->pool, count);
	sp->liveIndex = 0;
	sp->poolsize = count;
	return sp;
}

void free_smartprojpool(SPP sp) {
	free_projectile_pool(sp->pool, sp->poolsize);
	free(sp);
}

//Updates the positions of an entire pool of projectiles.
//Cond is a function giving a condition for "killing" a particular proj.
//For instance, it could take a PROJP and tell if it is off the screen area.
void update_pool_positions(SPP sp, int (*cond)(PROJP)) {
	int i;
		for (i = 0; i < sp->liveIndex; i++) {
				if (cond(sp->pool[i])) {
					swap_array_proj(sp->pool, i, sp->liveIndex - 1);
					sp->liveIndex--;
				}
			update_proj_position(sp->pool[i]);
			sp->pool[i]->animFrame++;
			sp->pool[i]->animFrame %= 60;
		}
}