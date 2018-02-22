//#include "stdafx.h"
#include "SDL.h"
#include "SDL_audio.h"
#include <stdlib.h>
#include <math.h>
#include <windows.h>

float g_nSampleChunkCount      = 0.0;
float g_fSampleCount           = 0.0;
float g_fTotalSampleCount      = 0.0;
float g_fTotalSampleCount2     = 0.0;

extern int  g_Samples;

short *asWavePulse;
short *asWaveBenson;

#define NUM_SAMPLE_LR_PAIRS (2048 * 256)   // Keep it a multiple of the callback sample buffer size

int g_NewWaveSize = NUM_SAMPLE_LR_PAIRS;

/*
SAMPLE_PULSE:
	DC.L	$807f 807f		;4b3c2

Setup:
	MOVE.L	#SAMPLE_PULSE,AUD0LCH	;50228: 23fc0004b3c200dff0a0
	MOVE.W	#$0002,AUD0LEN		;50232: 33fc000200dff0a4  ; 2 WORDS!
	MOVE.W	#$007c,AUD0PER		;5023a: 33fc007c00dff0a6
	MOVE.W	#$0000,AUD0VOL		;50242: 33fc000000dff0a8
Benson


	MOVE.W	D1,D5			;51d82: 3a01
	ASL.W	#5,D5			;51d84: eb45
	SUB.W	D1,D5			;51d86: 9a41
	SUB.W	D1,D5			;51d88: 9a41
	MOVE.W	D5,AUD0PER		;51d8a: 33c500dff0a6
	MOVE.W	#$001c,AUD0VOL		;51d90: 33fc001c00dff0a8

	CMPI.B	#$20,D1			;51d98: 0c010020
turn volume off for spaces


ASCII LO = '0' = $30
====================
	MOVE.W	$30,D5			;51d82: 3a01

 0000 0000 0011 0000

	ASL.W	#5,D5			;51d84: eb45

 0000 0110 0000 0000 (ASL5)

	SUB.W	D1,D5			;51d86: 9a41
	SUB.W	D1,D5			;51d88: 9a41

 0000 0110 0000 0000 (0600)
-0000 0000 0011 0000 (0030)
-0000 0000 0011 0000 (0030) -> 05A0 = #1440

	MOVE.W	D5,AUD0PER		;51d8a: 33c500dff0a6

ASCII HI = 'Z' = $5A
====================

	MOVE.W	$5A,D5			;51d82: 3a01

 0000 0000 0101 1010

	ASL.W	#5,D5			;51d84: eb45

 0000 1011 0100 0000 (ASL5)

	SUB.W	D1,D5			;51d86: 9a41
	SUB.W	D1,D5			;51d88: 9a41

 0000 1011 0100 0000 (0B40)
-0000 0000 0101 1010 (005A)
-0000 0000 0101 1010 (005A) -> A8C = #2700

	MOVE.W	D5,AUD0PER		;51d8a: 33c500dff0a6

  Benson Amiga periods range from 1400 -> 2700 decimal

  4 samples in a cycle
  T = 1 cycle

  1440 = (T/4) / 0.279365
  1440 * 0.279365 = T/4
  T = (1440 * 0.279365) * 4
  T = 1609us per waveform
  f = 1/T = 1 / (1.609ms)    1/(0.001609s)
  f = 621Hz

  2700 = (T/4) / 0.279365
  2700 * 0.279365 = T/4
  T = (2700 * 0.279365) * 4
  T = 3017us per waveform
  f = 1/T = 1 / (3.017ms)    1/(0.003017s)
  f = 331Hz

*/

//--------------------------------------------------------------------------------------------
// Interpolate()
//--------------------------------------------------------------------------------------------
double Interpolate( double a, double b, double fRatio )
{
  return ( b - a ) * fRatio + a;
} 

//char  g_Hello[] = "ARRIVING CONSIDERABLY LATER THAN FORECAST ";

//                 123456789-123456789-123456789-123456789-123456789-
char  g_Hello[] = "ARRIVING CONSIDERABLY LATER THAN";  // 32 chars

char  g_TxtBenson[ 64 ];

short g_Pulse[] = { -32767, 32767, -32767, 32767 };


//--------------------------------------------------------------------------------------------
// CreateBasePulseSample()
//--------------------------------------------------------------------------------------------
void CreateBasePulseSample( void )
{
  asWavePulse  = (short*) malloc( NUM_SAMPLE_LR_PAIRS * sizeof( short ) * 2 * 2 );

  memset( asWavePulse,  0, NUM_SAMPLE_LR_PAIRS * sizeof( short ) * 2 * 2 );

  int nSamplesThisChunk = 2048;
  int o = 0;

  short sHi = 16384;
  short sLo = -16384;

  // PDS: Multiples of 2048 to make the bigger waveform..
  for( int m = 0; m < 2048; m ++ )
  {
    int nPulseIndex = 0;

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
// TransposeFill()
//--------------------------------------------------------------------------------------------
int TransposeFill( short *psSource, short *psDest, double freqWanted, int nSamplesToFill )
{
  memset( psDest,  0, nSamplesToFill * sizeof( short ) * 2 );

  double freqBase  = 331.0F;
  double fRatio    = freqWanted / freqBase;

  int k = 0; 

  //for( double f = 0; f < (double) nSamplesToFill; f += fRatio ) 
  for( double f = 0; ; f += fRatio ) 
  { 
    // Left..
    psDest[ k ++ ] = Interpolate( psSource[ (int) f * 2 ], psSource[ (int) ( f * 2 ) + 2 ],  ( f * 2 ) - (float) floor( f * 2 ) ); 

    // Right
    psDest[ k ++ ] = Interpolate( psSource[ (int) ( f * 2 ) + 1 ], psSource[ (int) ( f * 2 ) + 3 ],  ((f*2)+1) - (float) floor( (f*2) + 1 ) ); 

    if( k >= nSamplesToFill )
      break;
  }

  return k;
} 

//--------------------------------------------------------------------------------------------
// GetCharFreq()
//
// 0 - 621 Hz
// Z - 331 Hz  -> 30Hz per char
//--------------------------------------------------------------------------------------------
float GetCharFreq( char c )
{
  if( ( c < '0' ) || ( c > 'Z' ) )
    return 0;

  float f = 331.0f + ( ( c - '0' ) * 9.66667f );

  return f;
}

//--------------------------------------------------------------------------------------------
// SoundInit()
//--------------------------------------------------------------------------------------------
void SoundInit( void )
{
  CreateBasePulseSample();

  asWaveBenson = (short*) malloc( NUM_SAMPLE_LR_PAIRS * sizeof( short ) * 2 * 2 );
}

//--------------------------------------------------------------------------------------------
// SayBensonText()
//--------------------------------------------------------------------------------------------
void SayBensonText( char *pszText )
{
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
    float f              = GetCharFreq( c );

    if( ( c == ' ' ) || ( f < 1.0f ) )
    {
      memset( &asWaveBenson[ o ], 0, nSamplesPerChar * sizeof( short ) * 2 );
      o += nSamplesPerChar;
      continue;
    }

    int   nSamplesFilled = TransposeFill( asWavePulse, &asWaveBenson[ o ], f, nSamplesPerChar );

    // PDS: This is the actual number of samples (2 samples per sample pair - ie - this is NOT the number of pairs but the number of SHORTS
    o += nSamplesFilled;
  }
}

//--------------------------------------------------------------------------------------------
// AllZeroBytes
//--------------------------------------------------------------------------------------------
BOOL AllZeroBytes( BYTE *p, int nLen )
{
  for( int i = 0; i < nLen; i ++ )
  {
    if( p[ i ] != 0 )
      return FALSE;
  }

  return TRUE;
}

int g_WaveOffset = 0;

//--------------------------------------------------------------------------------------------
// FillBuffer()
//
// nSamplesThisChunk will be 2048 for mono, or 4096 for stereo
//--------------------------------------------------------------------------------------------
void FillBuffer( short *pChunk, int nSamplesThisChunk )
{
  int     nChannels = 2;
  short  *pWaveSrc = asWaveBenson;

  if( g_WaveOffset < 0 )
    return;

  int nRemainingSamplesInWave = g_NewWaveSize - g_WaveOffset;
  int fSilenceAndRestart = 0;

  static DWORD dwBackoffStart = 0;

  if( ( dwBackoffStart == 0 ) && ( AllZeroBytes( (BYTE *) &pWaveSrc[ g_WaveOffset ], nSamplesThisChunk << 2 ) ) )
  {
    nRemainingSamplesInWave = 0;
    fSilenceAndRestart = 1;
    dwBackoffStart = ::GetTickCount();
  }

  if( dwBackoffStart > 0 )
  {
    if( ::GetTickCount() - dwBackoffStart < 3000 )
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

//--------------------------------------------------------------------------------------------
// Callback16()
//
// PDS: Requests 'len' bytes of samples - this is NOT the number of samples!!
//--------------------------------------------------------------------------------------------
void Callback16( void *userdata, Uint8 *pbStream, int nDataLen )
{
  int     nSamples = nDataLen >> 1;
  int     nRemainingSamples = nSamples;
  Uint8  *pChunk   = pbStream;

  FillBuffer( (short*) pChunk, nRemainingSamples );

  g_fTotalSampleCount += nRemainingSamples;
  g_nSampleChunkCount ++;
}

