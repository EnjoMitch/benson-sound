
//#include "stdafx.h"
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
SDL_AudioSpec  spec;
Uint32         sound_len;
Uint8         *sound_buffer;
int            sound_pos = 0;
int            counter;
int g_Samples = 2048;

void Callback16( void *userdata, Uint8 *pbStream, int nDataLen );
void SoundInit( void );
void SayBensonText( char *pszText );

//--------------------------------------------------------------------------------------------
// init_sdl()
//
// Do all the init stuff
//--------------------------------------------------------------------------------------------
void init_sdl (void)
{
  if (SDL_Init (SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0)
    exit (-1);
  atexit (SDL_Quit);
  screen = SDL_SetVideoMode (640, 480, 16, SDL_HWSURFACE);
  if (screen == NULL)
    exit (-1);
}

//--------------------------------------------------------------------------------------------
// play()
//--------------------------------------------------------------------------------------------
void play( void )
{
  sound_buffer  = new Uint8[g_Samples * 2 * 2];
  sound_len     = g_Samples * 2 * 2;

  spec.freq     = 44100;
  spec.format   = AUDIO_S16SYS;
  //spec.channels = 1;               // PDS: Mono for now..
  spec.channels = 2;               // PDS: Stereo now..
  spec.silence  = 0;
  spec.samples  = g_Samples;
  spec.padding  = 0;
  spec.size     = 0;
  spec.userdata = 0;

  spec.callback = Callback16;

  if( SDL_OpenAudio( &spec, NULL ) < 0 )
  {
    printf( "Can: %s\n", SDL_GetError ());
    exit (-1);
  }

  SDL_PauseAudio( 0 );
}

void SDLMainLoop()
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

//--------------------------------------------------------------------------------------------
// main()
//--------------------------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
  //DWORD dwLastSound = 0;

  init_sdl();

  SoundInit();

  SayBensonText( "THIS IS A BENSON TEST" );

  play();

  SDLMainLoop();

  SDL_Quit();

  return EXIT_SUCCESS;
}



