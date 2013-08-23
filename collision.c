

void ship_aster_coll(PROJP ship, PROJP asteroid)
{
	printf("The ship takes a hit!\n");
}
void bolt_aster_coll(PROJP bolt, PROJP asteroid)
{
	//Stuff
}
void bolt_enemy_coll(PROJP bolt, PROJP enemy)
{
	//More stuff.
}

void handleCollision(PROJP proj1, PROJP proj2)
{
	//These are temporary.
	if (proj1->kind == SHIP) //There may be more thorough checks in the future.
		ship_aster_coll(proj2)
	if (proj1->kind == ENEMY) //Same.
		bolt_enemy_coll(proj1, proj2)
	if (proj1->kind == ASTEROID) //Ditto.
		bolt_aster_coll(proj1, proj2)
}

void setup_collision_architecture()
{
	//This opaque expression figures out the number of bytes I need to store my BSP info.
	int temp = 1 << COLLISIONDIVISION;
	BUCKETP buckets = malloc(temp);
	char *collisionPile = malloc((((temp << 1) - 2) >> 2) + 1);

	//Finish this later, naïve implementation for now.
}

//Naïve solution! I will make 8 by 6 100px^2 buckets, and iterate through them.

//Clears 
void clear_collboxes()
{
	for (i = 0; i < 48; i++) collision_array[i] = 0;
}

void check_in_ship()
{
	collision_array[pp->pos->x / 100][pp->pos->y / 100]->ship = ship;
}

void check_in_proj(PROJP pp, int kind)
{
	COLLBOXP cb = collision_array[pp->pos->x / 100][pp->pos->y / 100];
	switch(kind) {
		case BOLT:
			if (cb->bolts) pp->next = cb->bolts;
			cb->bolts = pp;
			break;
		case ASTEROID:
			if (cb->asteroids) pp->next = cb->asteroids;
			cb->asteroids = pp;
			break;
		case ENEMY:
			if (cb->enemies) pp->next = cb->enemies;
			cb->enemies = pp;
			break;
	}
}

//Adds the projectiles from a smartpool to the collision array.
void check_in_smartpool(SPP sp)
{
	for (i = 0 ; i < sp->liveIndex; i++) {
		//Update the index so it can be killed via reference.
		sp->pool[i]->temp = i;
		check_in_proj(sp->pool[i], kind);
	}
}

//Returns 1 if the two projectiles are box colliding.
int box_colliding(PROJP proj1, PROJP proj2)
{
	return ((proj2->pos->x > proj1->pos->x && proj2->pos->x < proj1->pos->x + proj1->size->x) ||
	        (proj2->pos->x < proj1->pos->x && proj1->pos->x < proj2->pos->x + proj2->size->x)) &&
	       ((proj2->pos->y > proj1->pos->y && proj2->pos->y < proj1->pos->y + proj1->size->y) ||
	        (proj2->pos->y < proj1->pos->y && proj1->pos->y < proj2->pos->y + proj2->size->y));
}

void find_collbox_collisions(COLLBOXP cbp)
{
	PROJP temp1;
	PROJP temp2;
	//Handle asteroids hitting ship.
	if (cbp->ship) {
		temp2 = cbp->asteroids;
		while (temp2)
			if (box_colliding(ship, temp2))
				handleCollision(ship, temp2);
			temp2 = temp2->next;
		}
	}
	//Handle all enemy bolt collisions.
	temp1 = cbp->enemies;
	while (temp1) {
		temp2 = cbp->bolts;
		while (temp2)
			if (box_colliding(temp1, temp2))
				handleCollision(temp1, temp2);
			temp2 = temp2->next;
		}
		cbp->enemies = cbp->enemies->next;
		temp1->next = NULL;
	}
	//Handle all asteroid bolt collisions.
	temp1 = cbp->asteroids;
	while (temp1) {
		temp2 = cbp->bolts;
		while (temp2)
			if (box_colliding(temp1, temp2))
				handleCollision(temp1, temp2);
			temp2 = temp2->next;
		}
		cbp->asteroids = cbp->asteroids->next;
		temp1->next = NULL;
	}
}