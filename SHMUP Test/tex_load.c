int load_textures()
{

	//Load in the backdrop.
	backdrop = al_load_bitmap("MoonBrushTest.jpg");
	if (!backdrop) printf("Backdrop failed to load.\n");

	//Loading in the bolt frames.
	for (i = 0; i < 12; i++) {
		boltFrames[i] = NULL;
		char path[] = "Bolt/bolt0000.png\0";
		path[11] = i/10 + '0';
		path[12] = i%10 + '0';
		boltFrames[i] = al_load_bitmap(path);
	}

	//Loading in the asteroid frames.
	for (i = 0; i < 60; i++) {
		asteroidFrames[i] = NULL;
		//printf("Loading %2i: ", i);
		char path[] = "Aster4/aster0000.png\0";
		path[14] = i/10 + '0';
		path[15] = i%10 + '0';
		asteroidFrames[i] = al_load_bitmap(path);
		//if (asteroidFrames[i]) printf("Successfully loaded frame %2i from %s\n", i, path);
		if (!(al_get_bitmap_flags(asteroidFrames[i]) & ALLEGRO_VIDEO_BITMAP))
			printf("Asteroid %2i is not hardware-accelerated!\n", i);
	}

	//Loading ship frames.
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
	
	//Loading in the explosion frames.
	for (i = 0; i < 35; i++) {
		blastFrames[i] = NULL;
		//printf("Loading %2i: ", i);
		char path[] = "smoke/smoke0000.png\0";
		path[13] = i/10 + '0';
		path[14] = i%10 + '0';
		blastFrames[i] = al_load_bitmap(path);
		//if (blastFrames[i]) printf("Successfully loaded frame %2i from %s\n", i, path);
		if (!(al_get_bitmap_flags(blastFrames[i]) & ALLEGRO_VIDEO_BITMAP))
			printf("Blast %2i is not hardware-accelerated!\n", i);
	}

	return 0;
}