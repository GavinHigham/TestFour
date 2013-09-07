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

	ship->velX = 0;
	ship->velY = 0;
	//Checking ship movement keys.
	if(key[KEY_UP]) {
		ship->velY -= stdv;
		if (shipFrame < 58) shipFrame+= 3;
		if (backdropy < 0) backdropy++;
	}
	if(key[KEY_DOWN]) {
		ship->velY += stdv;
		if (shipFrame > 1) shipFrame-= 3;
		if (backdropy + BACKDROP_H > SCREEN_H) backdropy--;
	}
	if(key[KEY_LEFT]) {
		ship->velX -= stdv;
		if (backdropx < 0) backdropx++;
	}
	if(key[KEY_RIGHT]) {
		ship->velX += stdv;
		if (backdropx + BACKDROP_W > SCREEN_W) backdropx--;
	}

	if (ship->velX && ship->velY) {
		ship->velX *= diagscale;
		ship->velY *= diagscale;
	}

	if (shipFrame > 29) shipFrame-=2;
	if (shipFrame < 29) shipFrame+=2;

	update_proj_position(ship);

	//Update the ship's laser pool projectile positions.
	update_pool(sl_pool, &proj_update);

	//Creation and swapping of laser projectiles.
	if (key[KEY_SPACE] && !ship_cooldown && sl_pool->liveIndex < sl_pool->poolsize) {
		PROJP new = new_pool_item(sl_pool);
		new->posX = ship->posX + SHOT_OFFSET_X;
		new->posY = ship->posY + SHOT_OFFSET_Y;
		//give_vel(sl_pool->pool[sl_pool->liveIndex], ship->vel);
		new->velX = 15;
		new->velY = 0;
		if (ship_cooldown == 0) ship_cooldown = SHOT_COOLDOWN;
	}
	
	//Update the temporary debris positions.
	update_pool(ast_pool, &proj_update);
	
	for (i = 0; i < 10; i++) {
		//Creation and swapping of temporary debris.
		if (!enemy_cooldown && ast_pool->liveIndex < ast_pool->poolsize) {
			PROJP new = (PROJP)new_pool_item(ast_pool);
			new->posX = SCREEN_W + MARGIN;
			new->posY = (rand() % SCREEN_H);
			new->velX = (rand() % 3) - 4;
			new->velY = (rand() % 5) - 2;
			if (enemy_cooldown == 0) enemy_cooldown = ENEMY_COOLDOWN;
			//printf("%i\n", ast_pool->liveIndex);
		}
	}

	update_pool(blast_pool, &proj_update);
	for (i = 0; i < blast_pool->liveIndex; i++) {
		if (((PROJP)blast_pool->pool[i])->health > 0) ((PROJP)blast_pool->pool[i])->health--;
	}
	
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