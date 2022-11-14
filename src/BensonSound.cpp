#include "BensonSound.h"
#include "Util.h"
#include "OperatingSystem.h"
#include <cstring>
#include <algorithm>

//extern int  g_Samples;

const int BensonSound::SAMPLE_SIZE = 2048;
const int BensonSound::NUM_SAMPLE_LR_PAIRS = (SAMPLE_SIZE * 256);   // Keep it a multiple of the callback sample buffer size
const int BensonSound::g_NewWaveSize = NUM_SAMPLE_LR_PAIRS;
const int BensonSound::WAVE_LENGTH = NUM_SAMPLE_LR_PAIRS * sizeof( short ) * 2 * 2;

BensonSound::~BensonSound(){}
BensonSound::BensonSound(const SDLStuff & sdlStuff)
: m_sdlStuff(sdlStuff)
, vecAsWavePulse(WAVE_LENGTH)
, vecAsWaveBenson(WAVE_LENGTH)
{
    SoundInit();
}

//char  g_Hello[] = "ARRIVING CONSIDERABLY LATER THAN FORECAST ";

//                 123456789-123456789-123456789-123456789-123456789-
//char  g_Hello[] = "ARRIVING CONSIDERABLY LATER THAN";  // 32 chars
//char  g_TxtBenson[ 64 ];
//short g_Pulse[] = { -32767, 32767, -32767, 32767 };

//--------------------------------------------------------------------------------------------
// CreateBasePulseSample()
//--------------------------------------------------------------------------------------------
void BensonSound::CreateBasePulseSample( void )
{
  short * const asWavePulse = vecAsWavePulse.data();
  //int nSamplesThisChunk = 2048;
  int o = 0;

  const short sAmpl = 16384;
  const short sHi = sAmpl;
  const short sLo = -sAmpl;

  // PDS: Multiples of 2048 to make the bigger waveform..
  for( int m = 0; m < 2048; m ++ )
  {
    //int nPulseIndex = 0;
    // PDS: Pulse..
    o = BensonFillWavePulseGetIdx(o, sHi, asWavePulse);
    o = BensonFillWavePulseGetIdx(o, sLo, asWavePulse);
    o = BensonFillWavePulseGetIdx(o, sHi, asWavePulse);
    o = BensonFillWavePulseGetIdx(o, sLo, asWavePulse);
  }
}

int BensonSound::BensonFillWavePulseGetIdx(int o, short pulseVal, short * wavePulseToFill)
{
    for( int c = 0; c < 64; c ++ )
    {
      wavePulseToFill[ o ++ ] = pulseVal;  // L
      wavePulseToFill[ o ++ ] = pulseVal;  // R
    }
    return o;
}

//--------------------------------------------------------------------------------------------
// SoundInit()
//--------------------------------------------------------------------------------------------
void BensonSound::SoundInit( void )
{
  CreateBasePulseSample();
}

//--------------------------------------------------------------------------------------------
// SayBensonText()
//--------------------------------------------------------------------------------------------
void BensonSound::SayBensonText( const std::string & textPar )
{
    std::string text = textPar;
    std::transform(text.begin(), text.end(),text.begin(), ::toupper);
  const char * const pszText = text.c_str();
  short * const asWaveBenson = vecAsWaveBenson.data();
  memset( asWaveBenson,  0, WAVE_LENGTH);

  int o = 0;
  int nSamplesPerChar = ( NUM_SAMPLE_LR_PAIRS * 2 ) / 32;
  const int nNumChars       = strlen( pszText );

  nSamplesPerChar = nSamplesPerChar / 16;

  // PDS: 32 chars at a time..
  for( int cIndex = 0; cIndex < 32; cIndex ++ )
  {
    if( cIndex >= nNumChars )
      return;

    const char  c = pszText[ cIndex ];
    const float f = Util::GetCharFreq( c );

    if( ( c == ' ' ) || ( f < 1.0f ) )
    {
      memset( &asWaveBenson[ o ], 0, nSamplesPerChar * sizeof( short ) * 2 );
      o += nSamplesPerChar;
      continue;
    }

    int   nSamplesFilled = Util::TransposeFill( vecAsWavePulse.data(), &asWaveBenson[ o ], f, nSamplesPerChar );

    // PDS: This is the actual number of samples (2 samples per sample pair - ie - this is NOT the number of pairs but the number of SHORTS
    o += nSamplesFilled;
  }
}

//--------------------------------------------------------------------------------------------
// FillBuffer()
//
// nSamplesThisChunk will be 2048 for mono, or 4096 for stereo
//--------------------------------------------------------------------------------------------
void BensonSound::FillBuffer( unsigned char * pChunk, int nSamplesThisChunk )
{
  //int     nChannels = 2;
  const short * const pWaveSrc = vecAsWaveBenson.data();

  if( g_WaveOffset < 0 )
    return;

  int nRemainingSamplesInWave = g_NewWaveSize - g_WaveOffset;
  int fSilenceAndRestart = 0;

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
    //memset( &pChunk[ nRemainingSamplesInWave ], 0, ( nRemainingSamplesInWave << 1 )  );

    SayBensonText( "GREETINGS FROM SPARKY" );
    g_WaveOffset = 0;
  }
}

