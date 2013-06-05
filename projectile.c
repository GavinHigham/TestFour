#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h> 

//A 2D floating-point vector.
typedef struct vector {
   float x;
   float y;
} VECTOR, *VP;

//A 2D integer vector.
typedef struct ivector {
   int x;
   int y;
} IVECT, *IVP;

typedef struct projectile {
   VP pos;
   VP vel;
   IVP size;
   void *next; //Used for collision.
   int animFrame;
} PROJ, *PROJP;

typedef struct smartpool {
	PROJP *pool;
	int poolsize;
	int liveIndex;
	int colgroup;
} SMARTPOOL, *SPP;

VECTOR struct_zero_vector = {0, 0};
VP zero_vec = &struct_zero_vector;
int numcolgroups = 0;

struct timeval tv;

//This is just for getting random numbers.     
void init_random() {                                                         
	gettimeofday(&tv, NULL);
	//Seeding with microseconds.
	//You can re-seed rapidly without getting the same results.
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
	pp->next = NULL;
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

void kill_proj(SPP sp, int index) {
	swap_array_proj(sp->pool, index, sp->liveIndex - 1);
	sp->liveIndex--;
}

void update_proj_position(PROJP pp)
{
	pp->pos->x += pp->vel->x;
	pp->pos->y += pp->vel->y;
}
void update_pool_positions(SPP sp, int (*cond)(PROJP)) {
	int i;
	PROJP tmp;
	for (i = 0; i < sp->liveIndex; i++) {
		if (cond(sp->pool[i])) kill_proj(sp, i);
		tmp = sp->pool[i];
		update_proj_position(tmp);

		tmp->animFrame++;
		tmp->animFrame %= 60;
	}
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
	sp->colgroup = numcolgroups;
	numcolgroups++;
	return sp;
}

void free_smartprojpool(SPP sp) {
	free_projectile_pool(sp->pool, sp->poolsize);
	free(sp);
}