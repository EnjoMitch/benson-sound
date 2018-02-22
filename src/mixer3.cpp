
//#include "stdafx.h"
#ifdef _WIN32
    #include <SDL.h>
#else
    #include <SDL/SDL.h>
#endif

#include "SDLStuff.h"
#include "BensonSound.h"

//--------------------------------------------------------------------------------------------
// main()
//--------------------------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
  const SDLStuff sdl;
  BensonSound benson(sdl);
  benson.SayBensonText( "THIS IS A BENSON TEST" );
  //benson.SayBensonText( "ANOTHER ONE" );
  benson.Play();
  sdl.MainLoop();
  return EXIT_SUCCESS;
}
