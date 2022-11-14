#include "SDLSound.h"
#include "BensonSound.h"

#if defined(_WIN32) || defined(__APPLE__)
    //#include <SDL.h>
    #include <SDL_audio.h>
#else
    //#include <SDL/SDL.h>
    #include <SDL/SDL_audio.h>

    #define BYTE unsigned char
    #define DWORD unsigned int
#endif

//float g_nSampleChunkCount      = 0.0;
//float g_fSampleCount           = 0.0;
//float g_fTotalSampleCount      = 0.0;
//float g_fTotalSampleCount2     = 0.0;
//--------------------------------------------------------------------------------------------
// Callback16()
//
// PDS: Requests 'len' bytes of samples - this is NOT the number of samples!!
//--------------------------------------------------------------------------------------------
static void Callback16( void *userdata, Uint8 *pbStream, int nDataLen )
{
  const int     nSamples = nDataLen >> 1;
  const int     nRemainingSamples = nSamples;
  Uint8  *pChunk   = pbStream;

  BensonSound * benson = static_cast<BensonSound*>(userdata);
  benson->FillBuffer(pChunk, nRemainingSamples);

  //g_fTotalSampleCount += nRemainingSamples; // What is it used for?
  //g_nSampleChunkCount ++;
}

//--------------------------------------------------------------------------------------------
// play()
//--------------------------------------------------------------------------------------------
void SDLSound::Play(BensonSound * benson)
{
    SDL_AudioSpec  spec;
  //Uint8         * sound_buffer  = new Uint8[g_Samples * 2 * 2];
  //Uint32 sound_len     = g_Samples * 2 * 2;

  spec.freq     = 44100;
  spec.format   = AUDIO_S16SYS;
  //spec.channels = 1;               // PDS: Mono for now..
  spec.channels = 2;               // PDS: Stereo now..
  spec.silence  = 0;
  spec.samples  = BensonSound::SAMPLE_SIZE;
  spec.padding  = 0;
  spec.size     = 0;
  spec.userdata = (void*)benson;

  spec.callback = Callback16;

  if( SDL_OpenAudio( &spec, NULL ) < 0 )
  {
    printf( "Can: %s\n", SDL_GetError ());
    exit (-1);
  }

  SDL_PauseAudio( 0 );
}
