
//Pixel functions

//Modified versions from the SDL site:
//http://www.libsdl.org/cgi/docwiki.cgi/Pixel_Access 
inline void drawPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
inline Uint32 readPixel(SDL_Surface *surface, int x, int y);
inline void drawPixel(SDL_Surface *surface, int x, int y, Uint8 R, Uint8 G, Uint8 B);
inline void readPixel(SDL_Surface *surface, int x, int y, Uint8& R, Uint8& G, Uint8& B);

//From site: http://cone3d.gamedev.net/cgi-bin/index.pl?page=tutorials/gfxsdl/tut1
inline void drawPixelAlternative(SDL_Surface *screen, int x, int y, Uint8 R, Uint8 G, Uint8 B);


inline void drawPixelAlternative(SDL_Surface *screen, int x, int y, Uint8 R, Uint8 G, Uint8 B)
{
	Uint32 color = SDL_MapRGB(screen->format, R, G, B);
	switch (screen->format->BytesPerPixel)
	{
		case 1: // Assuming 8-bpp
		{
			Uint8 *bufp;
			bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
			*bufp = color;
		}
		break;
		case 2: // Probably 15-bpp or 16-bpp
		{
			Uint16 *bufp;
			bufp = (Uint16 *)screen->pixels + y*screen->pitch/2 + x;
			*bufp = color;
		}
		break;
		case 3: // Slow 24-bpp mode, usually not used
		{
			Uint8 *bufp;
			bufp = (Uint8 *)screen->pixels + y*screen->pitch + x * 3;
			if(SDL_BYTEORDER == SDL_LIL_ENDIAN)
			{
				bufp[0] = color;
				bufp[1] = color >> 8;
				bufp[2] = color >> 16;
			} 
			else 
			{
				bufp[2] = color;
				bufp[1] = color >> 8;
				bufp[0] = color >> 16;
			}
		}
		break;
		case 4: // Probably 32-bpp
		{
			Uint32 *bufp;
			bufp = (Uint32 *)screen->pixels + y*screen->pitch/4 + x;
			*bufp = color;
		}
		break;
		default:
		{
			printf("ERROR: %s:%u Wrong Bytes per Pixel\n",__FILE__,__LINE__);
			exit(1);
		}	
	}
}

inline Uint32 readPixel(SDL_Surface *surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;
	// Here p is the address to the pixel we want to retrieve 
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp) 
	{
		case 1:
			return *p;
		break;

		case 2:
			return *(Uint16 *)p;
		break;

		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return p[0] << 16 | p[1] << 8 | p[2];
			else
				return p[0] | p[1] << 8 | p[2] << 16;
		break;

		case 4:
			return *(Uint32 *)p;
		break;

		default:
			printf("ERROR: %s:%u Wrong Bytes per Pixel\n",__FILE__,__LINE__);
			exit(1);
			return 0;
	}
}

inline void readPixel(SDL_Surface *surface, int x, int y, Uint8& R, Uint8& G, Uint8& B)
{
	Uint32 pixel= readPixel(surface, x, y);
	SDL_GetRGB(pixel, surface->format, &R, &G, &B);
}

inline void drawPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	int bpp = surface->format->BytesPerPixel;
	// Here p is the address to the pixel we want to set 
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp) 
	{
		case 1:
			*p = pixel;
		break;

		case 2:
			*(Uint16 *)p = pixel;
		break;

		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN) 
			{
				p[0] = (pixel >> 16) & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = pixel & 0xff;
			} 
			else 
			{
				p[0] = pixel & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = (pixel >> 16) & 0xff;
			}
		break;

		case 4:
			*(Uint32 *)p = pixel;
		break;
		
		default:
			printf("ERROR: %s:%u Wrong Bytes per Pixel\n",__FILE__,__LINE__);
			exit(1);
	}
}

inline void drawPixel(SDL_Surface *surface, int x, int y, Uint8 R, Uint8 G, Uint8 B)
{
	drawPixel(surface, x, y, SDL_MapRGB(surface->format, R, G, B));
}
