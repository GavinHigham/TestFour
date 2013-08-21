ship_aster_coll(PROJP asteroid)
{
	printf("The ship takes a hit!\n");
}

bolt_aster_coll(PROJP asteroid)
{
	
}

handleCollision(PROJP proj1, PROJP proj2)
{
	switch(proj1->kind) {
		case 1:
			//Some code to execute
			break;
	 
		case 2:
			//Some other code
			break;
	 
		default:
			//Catch-all
			break;
	}
}

setup_collision_architecture()
{
	//This opaque expression figures out the number of bytes I need to store my BSP info.
	int temp = 1 << COLLISIONDIVISION;
	BUCKETP buckets = malloc(temp);
	char *collisionPile = malloc((((temp << 1) - 2) >> 2) + 1);

	//Finish this later, naïve implementation for now.
}

//Naïve solution! I will make 8 by 6 100px^2 buckets, and iterate through them.
setup_dumb_collision()
{
	//This will do stuff later.
}