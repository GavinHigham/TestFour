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
	pp->size = malloc(sizeof(VECTOR));
	pp->size->x = 100;
	pp->size->y = 100;
	pp->animFrame = rand() % 60;
	pp->next = NULL;
	pp->health = 100;
	pp->dead = 0;
	//Temporary sizes for testing.
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

//Kills a live projectile and returns it to the pool.
void kill_proj(SPP sp, int index) {
	swap_array_proj(sp->pool, index, sp->liveIndex - 1);
	sp->liveIndex--;
}

//Same as above, but takes a specially-prepared PROJP.
void special_kill_proj(SPP sp, PROJP pp) {
	swap_array_proj(sp->pool, pp->index, sp->liveIndex - 1);
	sp->liveIndex--;
}

//Revives a projectile from the smartpool and gives it to you.
PROJP new_proj(SPP sp)
{
	PROJP new = sp->pool[sp->liveIndex];
	new->dead = 0;
	sp->liveIndex++;
	return new;
}

void update_proj_position(PROJP pp)
{
	printf(".");
	pp->pos->x += pp->vel->x;
	pp->pos->y += pp->vel->y;
}
void update_pool_positions(SPP sp, int (*cond)(PROJP)) {
	int i;
	PROJP tmp;
	printf("Updating SPP:");
	for (i = 0; i < sp->liveIndex; i++) {
		if (sp->pool[i]->dead || cond(sp->pool[i])) kill_proj(sp, i);
		tmp = sp->pool[i];
		update_proj_position(tmp);

		tmp->animFrame++;
		tmp->animFrame %= 60;
	}
	printf("\n");
}

//returns 1 if a projectile is offscreen, 0 otherwise.
//Margin is how many pixels beyond the screen the proj should be to return 1.
int proj_offscreen(PROJP pp, int screen_w, int screen_h, int margin)
{
	return ((pp->pos->x > (screen_w + margin)) || (pp->pos->x < (0 - margin)) ||
		(pp->pos->y > (screen_h + margin)) || (pp->pos->y < (0 - margin)) );
}
//This one takes only one argument so it can be passed as a function pointer with filled-in values.
int offscreen(PROJP pp)
{
	return proj_offscreen(pp, SCREEN_W, SCREEN_H, MARGIN);
}
//Returns 1 if a PROJ has no health.
int no_health(PROJP pp)
{
	return (int)(pp->health <= 0);
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