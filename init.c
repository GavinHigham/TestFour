	
//Initializing a bunch of modules.
void init_random()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	//Seeding with microseconds.
	//You can re-seed rapidly without getting the same results.
	srand(tv.tv_usec);
}

int init_stuff()
{
	init_random();

	if(!al_init())
	{
		fprintf(stderr, "Failed to initialize allegro!\n");
		return -1;
	}
 
	if(!al_install_keyboard())
	{
		fprintf(stderr, "Failed to initialize the keyboard!\n");
		return -1;
	}
 
	timer = al_create_timer(1.0 / FPS);
	if(!timer)
	{
		fprintf(stderr, "Failed to create timer!\n");
		return -1;
	}

	if(!al_init_image_addon())
	{
		fprintf(stderr, "Failed to initialize al_init_image_addon!\n");
		return -1;
	}
 
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display)
	{
		fprintf(stderr, "Failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	ship = init_projectile();
	ship->kind = SHIP;
	ship->size->x = 100;
	ship->size->y = 50;
	//Setup the ship's laser pool.
	sl_pool = init_smartprojpool(PROJ_POOL_SIZE);
	for (i = 0; i < sl_pool->poolsize; i++) {
		sl_pool->pool[i]->kind = BOLT;
		sl_pool->pool[i]->size->x = 50;
		sl_pool->pool[i]->size->y = 20;
	}

	//Setup some enemy shots for test purposes.
	ast_pool = init_smartprojpool(ENEMY_POOL_SIZE);
	for (i = 0; i < ast_pool->poolsize; i++) {
		ast_pool->pool[i]->kind = ASTEROID;
		ast_pool->pool[i]->size->x = 25;
		ast_pool->pool[i]->size->y = 25;
	}

	/*
	blast_pool = init_smartprojpool(PROJ_POOL_SIZE);
	for (i = 0; i < blast_pool->poolsize; i++) {
		blast_pool->pool[i]->kind = BLAST;
		blast_pool->pool[i]->health = 36;
		blast_pool->pool[i]->animFrame = 0;
	}
	*/

	//The +1 is for the ship projectile, the *4 is for the 4 corners of each projectile.
	node_pool = init_smartnodepool((ENEMY_POOL_SIZE + PROJ_POOL_SIZE + 1) * 4);

	//Return 0 if everything went okay.
	return 0;
}