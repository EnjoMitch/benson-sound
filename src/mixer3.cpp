//#include "stdafx.h"
#include "SDLStuff.h"
#include "SDLSound.h"
#include "BensonSound.h"

#include <fstream>

std::ofstream gLog("benson.log");
//--------------------------------------------------------------------------------------------
// main()
//--------------------------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
  const SDLStuff sdl;
  BensonSound benson(sdl);
  if (argc > 1)
  {
    //gLog << "argc = " << argc << ", argv[1] = " << argv[1] << std::endl;
    benson.SayBensonText( argv[1] );
    SDLSound().Play(&benson);
    sdl.MainLoop(3000);
    return EXIT_SUCCESS;
  }
  benson.SayBensonText( "THIS IS A BENSON TEST" );
  //benson.SayBensonText( "ANOTHER ONE" );
  SDLSound().Play(&benson);
  sdl.MainLoop();
  return EXIT_SUCCESS;
}
