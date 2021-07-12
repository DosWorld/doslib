
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#if defined(USE_SDL2)
# if defined(__APPLE__) /* Brew got the headers wrong here */
#  include <SDL.h>
# else
#  include <SDL2/SDL.h>
# endif
#endif

/* NTS: Do not assume the full 256-color palette, 256-color Windows uses 20 of them, leaving us with 236 of them.
 *      We *could* just render with 256 colors but of course that means some colors get combined, so, don't.
 *      Not a problem so much if using Windows GDI but if we're going to play with DirectX or the earlier hacky
 *      Windows 3.1 equivalents, we need to worry about that. */

static char	fatal_tmp[256];

#if defined(USE_SDL2)
SDL_Window*	sdl_window = NULL;
SDL_Surface*	sdl_window_surface = NULL;
SDL_Surface*	sdl_game_surface = NULL;
SDL_Palette*	sdl_game_palette = NULL;
SDL_Color	sdl_pal[256];
Uint32		sdl_ticks_base = 0; /* use Uint32 type provided by SDL2 here to avoid problems */
bool		sdl_signal_to_quit = false;
#endif

#pragma pack(push,1)
struct IFEPaletteEntry {
	uint8_t	r,g,b;
};
#pragma pack(pop)

void IFEFatalError(const char *msg,...);
void IFEUpdateFullScreen(void);
void IFECheckEvents(void);

/* WARNING: Will wrap around after 49 days. You're not playing this game that long, are you?
 *          Anyway to avoid Windows-style crashes at 49 days, call IFEResetTicks() with the
 *          return value of IFEGetTicks() to periodically count from zero. */
#if defined(USE_SDL2)
uint32_t IFEGetTicks(void) {
	return uint32_t(SDL_GetTicks() - sdl_ticks_base);
}
#endif

#if defined(USE_SDL2)
void IFEResetTicks(const uint32_t base) {
	sdl_ticks_base = base; /* NTS: Use return value of IFEGetTicks() */
}
#endif

#if defined(USE_SDL2)
void IFEInitVideo(void) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		IFEFatalError("SDL2 failed to initialize");

	if (sdl_window == NULL && (sdl_window=SDL_CreateWindow("",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,640,480,SDL_WINDOW_SHOWN)) == NULL)
		IFEFatalError("SDL2 window creation failed");
	if (sdl_window_surface == NULL && (sdl_window_surface=SDL_GetWindowSurface(sdl_window)) == NULL)
		IFEFatalError("SDL2 window surface failed");
	if (sdl_game_surface == NULL && (sdl_game_surface=SDL_CreateRGBSurfaceWithFormat(0,640,480,8,SDL_PIXELFORMAT_INDEX8)) == NULL)
		IFEFatalError("SDL2 game surface (256-color) failed");
	if (sdl_game_palette == NULL && (sdl_game_palette=SDL_AllocPalette(256)) == NULL)
		IFEFatalError("SDL2 game palette");

	/* first color should be black, SDL2 will init palette to white for some reason */
	memset(sdl_pal,0,sizeof(SDL_Color));
	if (SDL_SetPaletteColors(sdl_game_palette,sdl_pal,0,1) != 0)
		IFEFatalError("SDL2 game palette set colors");

	/* apply palette to surface */
	if (SDL_SetSurfacePalette(sdl_game_surface,sdl_game_palette) != 0)
		IFEFatalError("SDL2 game palette set");

	/* make sure screen is cleared black */
	SDL_FillRect(sdl_game_surface,NULL,0);
	IFEUpdateFullScreen();
	IFECheckEvents();
}
#endif

#if defined(USE_SDL2)
void IFEShutdownVideo(void) {
	if (sdl_game_surface != NULL) {
		SDL_FreeSurface(sdl_game_surface);
		sdl_game_surface = NULL;
	}
	if (sdl_game_palette != NULL) {
		SDL_FreePalette(sdl_game_palette);
		sdl_game_palette = NULL;
	}
	if (sdl_window != NULL) {
		SDL_DestroyWindow(sdl_window);
		sdl_window_surface = NULL;
		sdl_window = NULL;
	}
	SDL_Quit();
}
#endif

#if defined(USE_SDL2)
void IFESetPaletteColors(const unsigned int first,const unsigned int count,IFEPaletteEntry *pal) {
	unsigned int i;

	if (first >= 256u || count > 256u || (first+count) > 256u)
		IFEFatalError("SetPaletteColors out of range first=%u count=%u",first,count);

	for (i=0;i < count;i++) {
		sdl_pal[i+first].r = pal[i].r;
		sdl_pal[i+first].g = pal[i].g;
		sdl_pal[i+first].b = pal[i].b;
		sdl_pal[i+first].a = 0xFFu;
	}

	if (SDL_SetPaletteColors(sdl_game_palette,sdl_pal,first,count) != 0)
		IFEFatalError("SDL2 game palette set colors");
}
#endif

#if defined(USE_SDL2)
void IFEUpdateFullScreen(void) {
	if (SDL_BlitSurface(sdl_game_surface,NULL,sdl_window_surface,NULL) != 0)
		IFEFatalError("Game to window BlitSurface");

	if (SDL_UpdateWindowSurface(sdl_window) != 0)
		IFEFatalError("Window surface update");
}
#endif

void IFETestRGBPalette() {
	struct IFEPaletteEntry pal[256];
	unsigned int i;

	for (i=0;i < 64;i++) {
		pal[i].r = i*4u;
		pal[i].g = i*4u;
		pal[i].b = i*4u;

		pal[i+64u].r = i*4u;
		pal[i+64u].g = 0;
		pal[i+64u].b = 0;

		pal[i+128u].r = 0;
		pal[i+128u].g = i*4u;
		pal[i+128u].b = 0;

		pal[i+192u].r = 0;
		pal[i+192u].g = 0;
		pal[i+192u].b = i*4u;
	}

	IFESetPaletteColors(0,256,pal);
}

#if defined(USE_SDL2)
bool IFEUserWantsToQuit(void) {
	return sdl_signal_to_quit;
}
#endif

#if defined(USE_SDL2)
unsigned char *IFEScreenDrawPointer(void) {
	return (unsigned char*)(sdl_game_surface->pixels);
}
#endif

#if defined(USE_SDL2)
unsigned int IFEScreenDrawPitch(void) {
	return (unsigned int)(sdl_game_surface->pitch);
}
#endif

#if defined(USE_SDL2)
unsigned int IFEScreenWidth(void) {
	return (unsigned int)(sdl_game_surface->w);
}
#endif

#if defined(USE_SDL2)
unsigned int IFEScreenHeight(void) {
	return (unsigned int)(sdl_game_surface->h);
}
#endif

#if defined(USE_SDL2)
bool IFEBeginScreenDraw(void) {
	if (SDL_MUSTLOCK(sdl_game_surface) && SDL_LockSurface(sdl_game_surface) != 0)
		return false;
	if (sdl_game_surface->pixels == NULL)
		IFEFatalError("SDL2 game surface pixels == NULL"); /* that's a BUG if this happens! */
	if (sdl_game_surface->pitch < 0)
		IFEFatalError("SDL2 game surface pitch is negative");

	return true;
}
#endif

#if defined(USE_SDL2)
void IFEEndScreenDraw(void) {
	if (SDL_MUSTLOCK(sdl_game_surface))
		SDL_UnlockSurface(sdl_game_surface);
}
#endif

#if defined(USE_SDL2)
void IFEProcessEvent(SDL_Event &ev) {
	if (ev.type == SDL_QUIT) {
		sdl_signal_to_quit = true;
	}
}
#endif

#if defined(USE_SDL2)
void IFECheckEvents(void) {
	SDL_Event ev;

	if (SDL_PollEvent(&ev))
		IFEProcessEvent(ev);
}
#endif

#if defined(USE_SDL2)
void IFEWaitEvent(const int wait_ms) {
	SDL_Event ev;

	if (SDL_WaitEventTimeout(&ev,wait_ms))
		IFEProcessEvent(ev);
}
#endif

void IFETestRGBPalettePattern(void) {
	unsigned int x,y,w,h,pitch;
	unsigned char *base;

	if (!IFEBeginScreenDraw())
		IFEFatalError("BeginScreenDraw TestRGBPalettePattern");
	if ((base=IFEScreenDrawPointer()) == NULL)
		IFEFatalError("ScreenDrawPointer==NULL TestRGBPalettePattern");

	w = IFEScreenWidth();
	h = IFEScreenHeight();
	pitch = IFEScreenDrawPitch();
	for (y=0;y < h;y++) {
		unsigned char *row = base + (y * pitch);
		for (x=0;x < w;x++) {
			if ((x & 0xF0) != 0xF0)
				row[x] = (y & 0xFF);
			else
				row[x] = 0;
		}
	}

	IFEEndScreenDraw();
	IFEUpdateFullScreen();
}

void IFEFatalError(const char *msg,...) {
	va_list va;

	va_start(va,msg);
	vsnprintf(fatal_tmp,sizeof(fatal_tmp)/*includes NUL byte*/,msg,va);
	va_end(va);

	IFEShutdownVideo();

	fprintf(stderr,"Fatal error: %s\n",fatal_tmp);
	exit(127);
}

void IFENormalExit(void) {
	IFEShutdownVideo();
	exit(0);
}

int main(int argc,char **argv) {
	//not used yet
	(void)argc;
	(void)argv;

	IFEInitVideo();

	IFEResetTicks(IFEGetTicks());
	while (IFEGetTicks() < 1000) {
		if (IFEUserWantsToQuit()) IFENormalExit();
		IFEWaitEvent(100);
	}

	IFETestRGBPalette();
	IFETestRGBPalettePattern();
	while (IFEGetTicks() < 3000) {
		if (IFEUserWantsToQuit()) IFENormalExit();
		IFEWaitEvent(100);
	}

	IFENormalExit();
	return 0;
}

