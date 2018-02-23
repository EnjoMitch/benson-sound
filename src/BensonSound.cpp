//#include "stdafx.h"
#include "BensonSound.h"
#include "Util.h"
#include "OperatingSystem.h"

#ifdef _WIN32
    #include <SDL.h>
    #include <SDL_audio.h>
#else
    #include <SDL/SDL.h>
    #include <SDL/SDL_audio.h>

    #define BYTE unsigned char
    #define DWORD unsigned int
#endif

//extern int  g_Samples;

const int BensonSound::NUM_SAMPLE_LR_PAIRS = (2048 * 256);   // Keep it a multiple of the callback sample buffer size
const int BensonSound::g_NewWaveSize = NUM_SAMPLE_LR_PAIRS;

BensonSound::BensonSound(const SDLStuff & sdlStuff)
: m_sdlStuff(sdlStuff)
{
    SoundInit();
}

BensonSound::~BensonSound()
{
    if (asWavePulse)    delete asWavePulse;
    if (asWaveBenson)   delete asWaveBenson;
}

//char  g_Hello[] = "ARRIVING CONSIDERABLY LATER THAN FORECAST ";

//                 123456789-123456789-123456789-123456789-123456789-
char  g_Hello[] = "ARRIVING CONSIDERABLY LATER THAN";  // 32 chars
char  g_TxtBenson[ 64 ];
short g_Pulse[] = { -32767, 32767, -32767, 32767 };

//--------------------------------------------------------------------------------------------
// CreateBasePulseSample()
//--------------------------------------------------------------------------------------------
void BensonSound::CreateBasePulseSample( void )
{
  asWavePulse  = (short*) malloc( NUM_SAMPLE_LR_PAIRS * sizeof( short ) * 2 * 2 );

  memset( asWavePulse,  0, NUM_SAMPLE_LR_PAIRS * sizeof( short ) * 2 * 2 );

  //int nSamplesThisChunk = 2048;
  int o = 0;

  short sHi = 16384;
  short sLo = -16384;

  // PDS: Multiples of 2048 to make the bigger waveform..
  for( int m = 0; m < 2048; m ++ )
  {
    //int nPulseIndex = 0;

    // PDS: Pulse..
    for( int c = 0; c < 64; c ++ )
    {
      asWavePulse[ o ++ ] =  sHi;  // L
      asWavePulse[ o ++ ] =  sHi;  // R
    }

    for( int c = 0; c < 64; c ++ )
    {
      asWavePulse[ o ++ ] =  sLo;  // L
      asWavePulse[ o ++ ] =  sLo;  // R
    }

    for( int c = 0; c < 64; c ++ )
    {
      asWavePulse[ o ++ ] =  sHi;  // L
      asWavePulse[ o ++ ] =  sHi;  // R
    }

    for( int c = 0; c < 64; c ++ )
    {
      asWavePulse[ o ++ ] =  sLo;  // L
      asWavePulse[ o ++ ] =  sLo;  // R
    }
  }
}

//--------------------------------------------------------------------------------------------
// SoundInit()
//--------------------------------------------------------------------------------------------
void BensonSound::SoundInit( void )
{
  CreateBasePulseSample();
  asWaveBenson = (short*) malloc( NUM_SAMPLE_LR_PAIRS * sizeof( short ) * 2 * 2 );
}

//--------------------------------------------------------------------------------------------
// SayBensonText()
//--------------------------------------------------------------------------------------------
void BensonSound::SayBensonText( const std::string & text )
{
  const char *pszText = text.c_str();
  memset( asWaveBenson,  0, NUM_SAMPLE_LR_PAIRS * sizeof( short ) * 2 * 2 );

  int o = 0;
  int nSamplesPerChar = ( NUM_SAMPLE_LR_PAIRS * 2 ) / 32;
  int nNumChars       = strlen( pszText );

  nSamplesPerChar = nSamplesPerChar / 16;

  // PDS: 32 chars at a time..
  for( int cIndex = 0; cIndex < 32; cIndex ++ )
  {
    if( cIndex >= nNumChars )
      return;

    char  c              = pszText[ cIndex ];
    float f              = Util::GetCharFreq( c );

    if( ( c == ' ' ) || ( f < 1.0f ) )
    {
      memset( &asWaveBenson[ o ], 0, nSamplesPerChar * sizeof( short ) * 2 );
      o += nSamplesPerChar;
      continue;
    }

    int   nSamplesFilled = Util::TransposeFill( asWavePulse, &asWaveBenson[ o ], f, nSamplesPerChar );

    // PDS: This is the actual number of samples (2 samples per sample pair - ie - this is NOT the number of pairs but the number of SHORTS
    o += nSamplesFilled;
  }
}

//--------------------------------------------------------------------------------------------
// FillBuffer()
//
// nSamplesThisChunk will be 2048 for mono, or 4096 for stereo
//--------------------------------------------------------------------------------------------
void BensonSound::FillBuffer( short *pChunk, int nSamplesThisChunk )
{
  //int     nChannels = 2;
  short  *pWaveSrc = asWaveBenson;

  if( g_WaveOffset < 0 )
    return;

  int nRemainingSamplesInWave = g_NewWaveSize - g_WaveOffset;
  int fSilenceAndRestart = 0;

  static DWORD dwBackoffStart = 0;

  if( ( dwBackoffStart == 0 ) && ( Util::AllZeroBytes( (unsigned char *) &pWaveSrc[ g_WaveOffset ], nSamplesThisChunk << 2 ) ) )
  {
    nRemainingSamplesInWave = 0;
    fSilenceAndRestart = 1;
    dwBackoffStart = OperatingSystem::GetTickCount();
  }

  if( dwBackoffStart > 0 )
  {
    if( OperatingSystem::GetTickCount() - dwBackoffStart < 3000 )
      return;

    dwBackoffStart = 0;
    fSilenceAndRestart = 1;
    nRemainingSamplesInWave = 0;
  }

  if( nRemainingSamplesInWave >= nSamplesThisChunk )
  {
    memcpy( pChunk, &pWaveSrc[ g_WaveOffset ], ( nSamplesThisChunk << 1 ) );
    g_WaveOffset += nSamplesThisChunk;
  }
  else
  {
    memcpy( pChunk, &pWaveSrc[ g_WaveOffset ], ( nRemainingSamplesInWave << 1 ) );

    g_WaveOffset = -1;

    fSilenceAndRestart = 1;
  }

  if( fSilenceAndRestart )
  {
    // PDS: Silence once we've reached the end..
    memset( &pChunk[ nRemainingSamplesInWave ], 0, ( nRemainingSamplesInWave << 1 )  );

    SayBensonText( "GREETINGS FROM SPARKY" );
    g_WaveOffset = 0;
  }
}

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
  int     nSamples = nDataLen >> 1;
  int     nRemainingSamples = nSamples;
  Uint8  *pChunk   = pbStream;

  BensonSound * benson = static_cast<BensonSound*>(userdata);
  benson->FillBuffer( (short*) pChunk, nRemainingSamples);

  //g_fTotalSampleCount += nRemainingSamples;
  //g_nSampleChunkCount ++;
}

//--------------------------------------------------------------------------------------------
// play()
//--------------------------------------------------------------------------------------------
void BensonSound::Play()
{
    SDL_AudioSpec  spec;
    const int g_Samples = 2048;
  //Uint8         * sound_buffer  = new Uint8[g_Samples * 2 * 2];
  //Uint32 sound_len     = g_Samples * 2 * 2;

  spec.freq     = 44100;
  spec.format   = AUDIO_S16SYS;
  //spec.channels = 1;               // PDS: Mono for now..
  spec.channels = 2;               // PDS: Stereo now..
  spec.silence  = 0;
  spec.samples  = g_Samples;
  spec.padding  = 0;
  spec.size     = 0;
  spec.userdata = this;

  spec.callback = Callback16;

  if( SDL_OpenAudio( &spec, NULL ) < 0 )
  {
    printf( "Can: %s\n", SDL_GetError ());
    exit (-1);
  }

  SDL_PauseAudio( 0 );
}
