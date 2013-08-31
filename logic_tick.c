void logic_tick()
{
	if (ship_cooldown > 0) ship_cooldown--;
	if (enemy_cooldown > 0) enemy_cooldown--;

	shipFramesetSwap^=60;

	if (logicTick >= 60) {
		printf("FPS: %2i, Pool Size: %4i, Ship health: %3i\n", fps, ast_pool->liveIndex, ship->health);
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
	update_pool_positions(sl_pool, &offscreen);

	//Creation and swapping of laser projectiles.
	if (key[KEY_SPACE] && !ship_cooldown && sl_pool->liveIndex < sl_pool->poolsize) {
		PROJP new = new_proj(sl_pool);
		new->pos->x = ship->pos->x + SHOT_OFFSET_X;
		new->pos->y = ship->pos->y + SHOT_OFFSET_Y;
		//give_vel(sl_pool->pool[sl_pool->liveIndex], ship->vel);
		give_vel(new, zero_vec);
		new->vel->x += 15;
		if (ship_cooldown == 0) ship_cooldown = SHOT_COOLDOWN;
	}
	
	//Update the temporary debris positions.
	update_pool_positions(ast_pool, &offscreen);
	
	for (i = 0; i < 10; i++) {
		//Creation and swapping of temporary debris.
		if (!enemy_cooldown && ast_pool->liveIndex < ast_pool->poolsize) {
			VECTOR random_vert_pos = {SCREEN_W + MARGIN, (rand() % SCREEN_H)};
			VECTOR random_vel = {(rand() % 3) - 4, (rand() % 5) - 2};
			PROJP new = new_proj(ast_pool);

			give_pos(new, &random_vert_pos);
			give_vel(new, &random_vel);
			if (enemy_cooldown == 0) enemy_cooldown = ENEMY_COOLDOWN;
			//printf("%i\n", ast_pool->liveIndex);
		}
	}

	/*
	//Setup my function pointer.
	printf("It probably breaks after this.\n");
	printf("%x\n", &no_health);
	update_pool_positions(blast_pool, &no_health);
	//printf("Hmm, it didn't break.\n");
	
	for (i = 0; i < blast_pool->liveIndex; i++) {
		blast_pool->pool[i]->health--;
	}
	*/
	
	clear_collboxes();
	//printf("It broke after clearing.\n");
	//printf("Checking in the ship:\n");
	check_in_proj(ship);
	//printf("\n");
	//printf("It broke after ship check-in\n");
	check_in_smartpool(sl_pool);
	check_in_smartpool(ast_pool);
	//printf("Smartpool check-in was successful.\n");
	do_collision();
	//printf("Collision was successful.\n");

}