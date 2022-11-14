#include "SDLStuff.h"

#ifdef _WIN32
    #include <SDL.h>
    #include <Windows.h>
#else
    #include <SDL/SDL.h>
#endif

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

void SDLStuff::MainLoop(int exitAfterMS) const
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
    if (exitAfterMS > 0)
    {
        SDL_Delay(exitAfterMS);
        running = false;
    }
  }
}
