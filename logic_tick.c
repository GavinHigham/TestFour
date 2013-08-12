void logic_tick()
{
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

	update_proj_position(ship);

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
}