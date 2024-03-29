//========================
//////////////////////////
// All this code below is VERY similar to smartpool code for projectiles.
// I should generalize it later and share code for smartpools.
//////////////////////////
//========================

NODEP init_node()
{
	NODEP np = malloc(sizeof(NODE));
	np->data = NULL;
	np->next = NULL;
	return np;
}

//========================================

void ship_aster_coll(PROJP ship, PROJP asteroid)
{
	//printf("The ship takes a hit!\n");
	//printf("%llx & %llx\n", (llui)ship, (llui)asteroid);
	asteroid->dead = 1;
	ship->health--;
	PROJP blastEffect = new_pool_item(blast_pool);
	blastEffect->dead = 0;
	blastEffect->health = 34;
	blastEffect->posX = asteroid->posX - 13.5;
	blastEffect->posY = asteroid->posY - 13.5;
	blastEffect->velX = asteroid->velX * 0.6;
	blastEffect->velY = asteroid->velY * 0.6;
}
void bolt_aster_coll(PROJP bolt, PROJP asteroid)
{
	//printf("An asteroid is hit by a bolt!\n");
	//printf("%llx & %llx\n", (llui)bolt, (llui)asteroid);
	bolt->dead = 1;
	asteroid->dead = 1;
	PROJP blastEffect = new_pool_item(blast_pool);
	blastEffect->dead = 0;
	blastEffect->health = 34;
	blastEffect->posX = asteroid->posX - 13.5;
	blastEffect->posY = asteroid->posY - 13.5;
	blastEffect->velX = asteroid->velX * 0.6;
	blastEffect->velY = asteroid->velY * 0.6;
}
void bolt_enemy_coll(PROJP bolt, PROJP enemy)
{
	//printf("An enemy is hit by a bolt!\n");
	//printf("%llx & %llx\n", (llui)bolt, (llui)enemy);
}

void handleCollision(PROJP proj1, PROJP proj2)
{
	//printf("Handling collision.\n");
	//These are temporary.
	if (proj1->kind == SHIP && proj2->kind == ASTEROID) //There may be more thorough checks in the future.
		ship_aster_coll(proj1, proj2);
	if (proj1->kind == BOLT) {
		if (proj2->kind == ENEMY) bolt_enemy_coll(proj1, proj2);
		if (proj2->kind == ASTEROID) bolt_aster_coll(proj1, proj2);
	}
}

/*
void setup_collision_architecture()
{
	//This opaque expression figures out the number of bytes I need to store my BSP info.
	int temp = 1 << COLLISIONDIVISION;
	BUCKETP buckets = malloc(temp);
	char *collisionPile = malloc((((temp << 1) - 2) >> 2) + 1);

	//Finish this later, naïve implementation for now.
}
*/

//Naïve solution! I will make 8 by 6 100px^2 buckets, and iterate through them.

//Clears the collision array's buckets.
void clear_collboxes()
{
	for (i = 0; i < 8; i++) {
		int j;
		for (j = 0; j < 6; j++) {
			COLLBOXP temp = &collision_array[i][j];
			temp->ship = NULL;
			temp->bolts = NULL;
			temp->asteroids = NULL;
			temp->enemies = NULL;
		}
	}
	node_pool->liveIndex = 0;
}

//Returns 0 if the point is not in given bounds, 0 to width, 0 to height.
int point_in_bounds(int width, int height, int pointX, int pointY)
{
	return (pointX >= 0 && pointX < width && pointY >= 0 && pointY < height);
}

//Checks if a point for a projectile is valid, checks it in to the collision array if it is.
void point_checkin(PROJP pp, int pointX, int pointY)
{
	if (point_in_bounds(SCREEN_W, SCREEN_H, pointX, pointY)) {
		COLLBOXP cbp = &collision_array[pointX / 100][pointY / 100];
		switch(pp->kind) {
			case SHIP:
				if (!cbp->ship && node_pool->liveIndex < node_pool->poolsize) {
					NODEP new = new_pool_item(node_pool);
					new->index = node_pool->liveIndex - 1;
					new->data = pp;
					new->next = NULL;
					node_pool->liveIndex++;
					cbp->ship = new;
				}
				break;
			case BOLT:
				if ((!cbp->bolts || pp != cbp->bolts->data) && node_pool->liveIndex < node_pool->poolsize) {
					NODEP new = new_pool_item(node_pool);
					new->index = node_pool->liveIndex - 1;
					new->data = pp;
					new->next = cbp->bolts;
					cbp->bolts = new;
				}
				break;
			case ASTEROID:
				//printf("Checking in an asteroid.\n");
				if ((!cbp->asteroids || pp != cbp->asteroids->data) && node_pool->liveIndex < node_pool->poolsize) {
					NODEP new = new_pool_item(node_pool);
					new->index = node_pool->liveIndex - 1;
					new->data = pp;
					new->next = cbp->asteroids;
					cbp->asteroids = new;
				}
				//printf("Checked in an asteroid.\n");
				break;
			case ENEMY:
				if ((!cbp->enemies || pp != cbp->enemies->data) && node_pool->liveIndex < node_pool->poolsize) {
					NODEP new = new_pool_item(node_pool);
					new->index = node_pool->liveIndex - 1;
					new->data = pp;
					new->next = cbp->enemies;
					cbp->enemies = new;
				}
				break;
			case BLAST:
				//No collisions with explosion entities for now.
				break;
		}
	}
}

void check_in_proj(PROJP pp)
{
	//printf(".");
	double cornerX = pp->posX + pp->offsetX;
	double cornerY = pp->posY + pp->offsetY;
	point_checkin(pp, (int)cornerX, (int)cornerY); //Check in the top-left corner.
	cornerX += pp->sizeX;
	point_checkin(pp, (int)cornerX, (int)cornerY); //Top right...
	cornerY += pp->sizeY;
	point_checkin(pp, (int)cornerX, (int)cornerY); //Bottom right...
	cornerX -= pp->sizeX;
	point_checkin(pp, (int)cornerX, (int)cornerY); //Bottom left.
}

//Adds the projectiles from a smartpool to the collision array.
void check_in_smartpool(SPP sp)
{
	//printf("Checking in a smartpool:\n");
	for (i = 0 ; i < sp->liveIndex; i++) {
		//Update the index so it can be killed via reference.
		((PROJP)sp->pool[i])->index = i;
		check_in_proj(sp->pool[i]);
	}
	//printf("\n");
}

//Returns 1 if the two projectiles are box colliding.
int box_colliding(PROJP proj1, PROJP proj2)
{
	//printf("Checking box collision.\n");
	return ((proj2->posX+proj2->offsetX > proj1->posX+proj1->offsetX && proj2->posX+proj2->offsetX < proj1->posX+proj1->offsetX + proj1->sizeX) ||
	        (proj2->posX+proj2->offsetX < proj1->posX+proj1->offsetX && proj1->posX+proj1->offsetX < proj2->posX+proj2->offsetX + proj2->sizeX)) &&
	       ((proj2->posY+proj2->offsetY > proj1->posY+proj1->offsetY && proj2->posY+proj2->offsetY < proj1->posY+proj1->offsetY + proj1->sizeY) ||
	        (proj2->posY+proj2->offsetY < proj1->posY+proj1->offsetY && proj1->posY+proj1->offsetY < proj2->posY+proj2->offsetY + proj2->sizeY));
}

void find_collbox_collisions(COLLBOXP cbp)
{
	NODEP temp1;
	NODEP temp2;
	//Handle asteroids hitting ship.
	while (cbp->ship != NULL) {
		//printf("Entering ship collision test.\n");
		temp2 = cbp->asteroids;
		while (temp2 != NULL) {
			//printf("Checking a ship/asteroid collision possibility.\n");
			if (box_colliding(ship, temp2->data))
				handleCollision(ship, temp2->data);
			temp2 = temp2->next;
			//printf("temp2 = %llx, temp2->next = %llx", (llui)temp2, (llui)temp2->next);
		}
		//printf("Exiting ship collision test. Cleaning up.\n");
		kill_item(node_pool, cbp->ship->index);
		cbp->ship = cbp->ship->next;
		//assert(cbp->ship->next != cbp->ship);
	}
	//printf("Handled all ship collisions.\n");
	//Handle all enemy bolt collisions.
	temp1 = cbp->enemies;
	while (temp1 != NULL) {
		temp2 = cbp->bolts;
		while (temp2 != NULL) {
			if (box_colliding(temp2->data, temp1->data))
				handleCollision(temp2->data, temp1->data);
			temp2 = temp2->next;
		}
		kill_item(node_pool, temp1->index);
		cbp->enemies = cbp->enemies->next;
		//temp1->next = NULL;
		temp1 = cbp->enemies;
	}
	//printf("Handled all enemy/bolt collisions.\n");
	//Handle all asteroid bolt collisions.
	temp1 = cbp->asteroids;
	while (temp1 != NULL) {
		temp2 = cbp->bolts;
		while (temp2 != NULL) {
			//printf("Box collision for asteroids.\n");
			//printf("temp2 = %llx\n", (llui)temp2);
			if (box_colliding(temp2->data, temp1->data))
				handleCollision(temp2->data, temp1->data);
			temp2 = temp2->next;
		}
		kill_item(node_pool, temp1->index);
		cbp->asteroids = cbp->asteroids->next;
		//temp1->next = NULL;
		temp1 = cbp->asteroids;
	}
	//printf("Handled all asteroid/bolt collisions.\n");
}

void do_collision()
{
	for (i = 0; i < SCREEN_W / 100; i++) {
		int j;
		for (j = 0; j < SCREEN_H / 100; j++) {
			//printf("Checking collision for box [%i][%i].\n", i, j);
			COLLBOXP temp = &collision_array[i][j];
			//printf("Accessing the array did not cause a crash.\n");
			find_collbox_collisions(temp);
		}
	}
}