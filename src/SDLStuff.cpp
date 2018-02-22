#include "SDLStuff.h"

#ifdef __linux__
    #include <SDL/SDL.h>
    #include <SDL/SDL_audio.h>
#elif _WIN32
    #include <SDL.h>
    #include <SDL_audio.h>
    #include <Windows.h>
#else

#endif

SDL_Surface   *screen;

SDLStuff::SDLStuff()
{
    Init();
}

SDLStuff::~SDLStuff()
{
    SDL_Quit();
}


//--------------------------------------------------------------------------------------------
// init_sdl()
//
// Do all the init stuff
//--------------------------------------------------------------------------------------------
void SDLStuff::Init()
{
    if (SDL_Init (SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0)
        exit (-1);
    atexit (SDL_Quit);
    screen = SDL_SetVideoMode (640, 480, 16, SDL_HWSURFACE);
    if (screen == NULL)
        exit (-1);
}

void SDLStuff::MainLoop() const
{
  SDL_Event event;
  bool running = true;

  while( running )
  {
    while( SDL_PollEvent( &event ) )
    {
      // GLOBAL KEYS / EVENTS
      switch (event.type)
      {
        case SDL_KEYDOWN:
          switch (event.key.keysym.sym)
          {
            case SDLK_ESCAPE:
              running = false;
              break;

            case SDLK_RETURN:
              break;

            default: break;
          }
          break;

        case SDL_QUIT:
          running = false;
          break;
      }

      SDL_Delay(1);
    }

    SDL_Delay(1);
  }
}
