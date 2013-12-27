// A little function ripped directly out of gitm/src/Controller.cpp 
// Returns a pointer to an ALLEGRO_BITMAP* based on the gchr data
// from the const char array

ALLEGRO_BITMAP* Controller::getChr(const char *data, unsigned char scale)
{
	// Offset for 
	const char readOffset = 4;
	if (scale < 0)
	{
		scale = 1;
	}
	// Anywhere from 0 to 255 
	int w = data[1] + 255*(data[0]); 
	// Anywhere from 0 to 255 
	int h = data[3] + 255*(data[2]);

	printf("Width: %i\nHeight: %i\n",w,h);

	ALLEGRO_BITMAP* px = al_create_bitmap(scale,scale);
	al_set_target_bitmap(px);
	al_clear_to_color(al_map_rgb(255,255,255));
	
	ALLEGRO_COLOR colors[4];
	colors[0] = al_map_rgb(255,255,255);
	colors[1] = al_map_rgb(160,160,160);
	colors[2] = al_map_rgb(80,80,80);
	colors[3] = al_map_rgb(0,0,0);
	
	ALLEGRO_BITMAP* returnMe = al_create_bitmap(w*scale,h*scale);
	al_set_target_bitmap(returnMe);

	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w/2; x++)
		{
			// Separate the single byte into two nybbles
			int index = ((x) + (y*(w/2))) + readOffset;

			char px2 = (data[index])%16;
			char px1 = (data[index]) >> 4;

			// Current pixel (discarding first two bytes for size)
			if (px1 > 0)
			{ 
				al_draw_tinted_bitmap(px,colors[(px1-1)%4],(x*2)*scale,y*scale,0);
			}
			if (px2 > 0)
			{
				al_draw_tinted_bitmap(px,colors[(px2-1)%4],((x*2)+1)*scale,y*scale,0);
			}
			
			// Console printing for fun
			if (px1 > 0)
			{
				std::cout << (int)px1;
			}
			else
			{
				printf(" ");
			}
			if (px2 > 0)
			{
				std::cout << (int)px2;
			}
			else
			{
				printf(" ");
			}
		}
	}
	al_destroy_bitmap(px);
	return returnMe;
}