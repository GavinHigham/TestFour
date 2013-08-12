	
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
	//Setup the ship's laser pool.
	sl_pool = init_smartprojpool(PROJ_POOL_SIZE);

	//Setup some enemy shots for test purposes.
	en_pool = init_smartprojpool(ENEMY_POOL_SIZE);

	//Return 0 if everything went okay.
	return 0;
}