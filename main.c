// GCHR by Michael Moffitt 2013
// mikejmoffitt.com

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

// Basically skips every other pixel, and line, reducing the image
// size to half in both X and Y dimensions
// Really mostly used for Ghost in the Machine, you likely won't want this
#define SKIP_ODD 0

// Transparency threshold:
#define ALPHA_LIM 128

// Value limits for shade bucketing:
#define WHITE 200
#define LIGHT_GREY 120
#define DARK_GREY 50

// Returns the correct gchr pixel value for a single pixel
char map_pixel(unsigned char value, unsigned char alpha, int leftpx)
{
	unsigned char retme;
	// hurr random guess worked first time
	if (alpha > ALPHA_LIM)
	{
		// what a stopgap implementation! Probably won't be revised?
		if (value > WHITE)
		{
			retme = 0x01;
		}
		else if (value > LIGHT_GREY)
		{
			retme = 0x02;
		}
		else if (value > DARK_GREY)
		{
			retme = 0x03;
		}
		else
		{
			retme = 0x04;
		}
	}
	else
	{
		retme = 0x00;
	}
	if (leftpx)
	{
		// shift for the other nybble of the char for left pixels of a byte
		retme <<= 4;
	}
	return retme;
}

// Take a value from 0-15 and return a single hex character
char get_hex(char value)
{
	unsigned char tmp = (unsigned char)(value % 16);
	char retme;
	sprintf(&retme, "%.1x", tmp);
	return retme;
}

// Put a single byte into the file in hexidecimal
void write_hex(ALLEGRO_FILE* f, char value, int final)
{
	al_fputs(f,"0x");
	al_fputc(f,get_hex(value>>4));
	al_fputc(f,get_hex(value%16));
	if (final == 0)
	{
		al_fputc(f,',');
	}
}

void make_gchr(char *fname, char* oname)
{
	int totalpx = 0;
	printf("Loading %s... ",fname);
	ALLEGRO_BITMAP* chr = al_load_bitmap(fname);
	if (chr == NULL)
	{
		printf("ERROR! Could not load file. Aborting.\n");
		return;
	}
	int w = al_get_bitmap_width(chr);
	int h = al_get_bitmap_height(chr);
	totalpx = (w * h) / 2; // Each px is 4 nybbles

	if (SKIP_ODD)
	{
		totalpx = totalpx / 4;
		
	}

	totalpx = totalpx + 1;
	char total_print[7] = {'0','x','0','0','0','0'};
	char *outname = (char*)malloc((strlen(oname)+strlen(".c")+1));
	char *outname2 = (char*)malloc((strlen(oname)+strlen(".h")+1));	

	strcpy(outname,oname);
	strcat(outname,".c");

	strcpy(outname2,oname);
	strcat(outname2,".h");

	if (outname == NULL)
	{
		printf("ERROR! Could not malloc for strcat. Aborting.\n");
		return;
	}
	ALLEGRO_FILE* outfile = al_fopen(outname,"w");
	ALLEGRO_FILE* outfile2 = al_fopen(outname2,"w");
	if (outfile == NULL)
	{
		printf("ERROR! Could not open %s for writing. Aborting.\n",outname);
		return;
	}

	// Split up filename to get actual file name without path

	char *token;

	token = strtok(fname,"/\\");

	char *arrname;
	arrname = "";
	while (token != NULL)
	{
		arrname = token;
		token = strtok(NULL,"/\\");
	}
	arrname = strtok(arrname,".");

	if (totalpx > 0xFFFF)
	{
		totalpx = 0xFFFF;
	}

	printf("Total pixels: %d\n",totalpx);

	total_print[2] = get_hex(totalpx >> 12);
	total_print[3] = get_hex(totalpx >> 8);
	total_print[4] = get_hex(totalpx >> 4);
	total_print[5] = get_hex(totalpx%16);

	// Dimensioning info
	al_fputs(outfile2,"#ifndef GCHR_");
	al_fputs(outfile2,arrname);
	al_fputs(outfile2,"_H\n");
	al_fputs(outfile2,"#define GCHR_");
	al_fputs(outfile2,arrname);
	al_fputs(outfile2,"_H\n\n");
	al_fputs(outfile2,"/* gchr converted image\n  Original filename: ");
	al_fputs(outfile2,fname);
	al_fputs(outfile2,"\n*/\n");
	al_fputs(outfile2,"const char *gchr_");
	al_fputs(outfile2,arrname);

	/*
	al_fputs(outfile2,"[");
	al_fputs(outfile2,total_print);
	al_fputs(outfile2,"];\n");
	
	*/

	al_fputs(outfile2,";\n");
	al_fputs(outfile,"#include \"");
	al_fputs(outfile,arrname);
	al_fputs(outfile,".png.h\"\n");
	al_fputs(outfile,"gchr_");
	al_fputs(outfile,arrname);
	al_fputs(outfile," = {");

	if (SKIP_ODD)
	{
		w = w / 2;
		h = h / 2;
	}


	write_hex(outfile,(w >> 8),0);
	write_hex(outfile,(w % 255),0);
	write_hex(outfile,(h >> 8),0);
	write_hex(outfile,(h % 255),0);

	if (SKIP_ODD)
	{
		w = w * 2;
		h = h * 2;
	}
	
	// Linebreak for moderate readability
	al_fputc(outfile,'\n');
	al_fputc(outfile,'\t');
	for(int y = 0; y < h; y++)
	{
		if (SKIP_ODD && y%2 == 0 || SKIP_ODD != 1)
		{
			for (int x = 0; x < w; x++)
			{
				char dpx = 0x00;
				ALLEGRO_COLOR px1 = al_get_pixel(chr,x,y);
				unsigned char value = 0;
				unsigned char alpha = 0;
				al_unmap_rgba(px1,&value,&value,&value,&alpha);
				dpx = dpx + ((map_pixel(value,alpha,1)));	
			
				x = x + 1;
				if (SKIP_ODD)
				{
					x = x + 1;
				}

				ALLEGRO_COLOR px2 = al_get_pixel(chr,x,y);
				al_unmap_rgba(px2,&value,&value,&value,&alpha);
				dpx = dpx + ((map_pixel(value,alpha,0)));
				if ((SKIP_ODD == 0) && (x == w-1 && y == h-1))
				{
					write_hex(outfile,dpx,1);
				}
				else if ((SKIP_ODD == 1) && (x == w-2 && (y == h-2 || y == h-1)))
				{
					write_hex(outfile,dpx,1);
				}
				else
				{
					write_hex(outfile,dpx,0);
				}
				if (SKIP_ODD)
				{
					x = x + 1;
				}
			}
			al_fputs(outfile,"\n\t");
		}
	}
	al_fputs(outfile,"};\n");
	al_fputs(outfile2,"\n\n#endif\n");
	printf(" made %s.\n",arrname);
	free(outname);
	free(outname2);
	al_fclose(outfile);
	al_fclose(outfile2);

	al_destroy_bitmap(chr);
}

int main(int argc, char **argv)
{
	al_init();
	al_init_primitives_addon();
	al_init_image_addon();
	ALLEGRO_DISPLAY* display = al_create_display(1,1);

	if (argc < 2)
	{
		printf("Usage: %s file1 file2 ... file[n]\n",argv[0]);
	}
	else if (argc == 2)
	{
		make_gchr(argv[1],argv[1]);
	}
	else
	{
		for (int i = 0; i < argc-1; i++)
		{
			make_gchr(argv[i+1],argv[i+1]);		
		}
	}

	al_destroy_display(display);
	return 0;
}
