	
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

	ship = init_proj();
	ship->kind = SHIP;
	ship->offsetX = 26;
	ship->offsetY = 22;
	ship->sizeX = 45;
	ship->sizeY = 7;
	//Setup the ship's laser pool.
	sl_pool = init_smartItemPool(PROJ_POOL_SIZE, (void *(*)())&init_proj);
	for (i = 0; i < sl_pool->poolsize; i++) {
		PROJP tmp = sl_pool->pool[i];
		tmp->kind = BOLT;
		tmp->sizeX = 50;
		tmp->sizeY = 20;
	}

	//Setup some enemy shots for test purposes.
	ast_pool = init_smartItemPool(ENEMY_POOL_SIZE, (void *(*)())&init_proj);
	for (i = 0; i < ast_pool->poolsize; i++) {
		PROJP tmp = ast_pool->pool[i];
		tmp->kind = ASTEROID;
		tmp->offsetX = 6;
		tmp->offsetY = 7;
		tmp->sizeX = 12;
		tmp->sizeY = 12;
	}

	blast_pool = init_smartItemPool(PROJ_POOL_SIZE, (void *(*)())&init_proj);
	for (i = 0; i < blast_pool->poolsize; i++) {
		PROJP tmp = blast_pool->pool[i];
		tmp->kind = BLAST;
		tmp->health = 36;
		tmp->animFrame = 0;
	}


	//The +1 is for the ship projectile, the *4 is for the 4 corners of each projectile.
	node_pool = init_smartItemPool((ENEMY_POOL_SIZE + PROJ_POOL_SIZE + 1) * 4, (void *(*)())&init_node);

	//Return 0 if everything went okay.
	return 0;
}