PROJP init_proj()
{
	PROJP pp = malloc(sizeof(PROJ));
	pp->posX = 0;
	pp->posY = 0;
	pp->velX = 0;
	pp->velY = 0;
	pp->sizeX = 0;
	pp->sizeY = 0;
	pp->offsetX = 0;
	pp->offsetY = 0;
	pp->animFrame = rand() % 60;
	pp->health = 100;
	pp->dead = 0;
	return pp;
}

void update_proj_position(PROJP pp)
{
	pp->posX += pp->posX;
	pp->posY += pp->posY;
}

//Returns 1 if a PROJ has no health.
int no_health(PROJP pp)
{
	return (pp->health <= 0);
}

//Returns 1 if a projectile is offscreen, 0 otherwise.
//Margin is how many pixels beyond the screen the proj should be to return 1.
int proj_offscreen(PROJP pp, int screen_w, int screen_h, int margin)
{
	return ((pp->posX > (screen_w + margin)) || (pp->posX < (0 - margin)) ||
		(pp->posY > (screen_h + margin)) || (pp->posY < (0 - margin)) );
}

//Mostly the same as above.
//This one takes only one argument so it can be passed as a function pointer with filled-in values.
int offscreen(PROJP pp)
{
	return proj_offscreen(pp, SCREEN_W, SCREEN_H, MARGIN);
}

void proj_update(SPP sp, int i)
{
	if (sp->pool[i]->dead || offscreen(sp->pool[i])) kill_item(sp, i);
	tmp = sp->pool[i];
	update_proj_position(tmp);

	tmp->animFrame++;
	tmp->animFrame %= 60;
}

