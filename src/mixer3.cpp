//#include "stdafx.h"
#include "SDLStuff.h"
#include "SDLSound.h"
#include "BensonSound.h"

#include <fstream>
#include <string>

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
      const double msPerChar = 25;
    //gLog << "argc = " << argc << ", argv[1] = " << argv[1] << std::endl;
    //const std::string text = "012345678901234567890123456789";
    const std::string text = argv[1];
    benson.SayBensonText( text );
    const int msWait = text.size() * msPerChar;
    SDLSound().Play(&benson);
    sdl.MainLoop(msWait);
    return EXIT_SUCCESS;
  }
  benson.SayBensonText( "THIS IS A BENSON TEST" );
  //benson.SayBensonText( "ANOTHER ONE" );
  SDLSound().Play(&benson);
  sdl.MainLoop();
  return EXIT_SUCCESS;
}
