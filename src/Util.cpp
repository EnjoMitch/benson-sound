#include "Util.h"
#include <cmath>
#include <cstring>

Util::Util(){}

//--------------------------------------------------------------------------------------------
// AllZeroBytes
//--------------------------------------------------------------------------------------------
bool Util::AllZeroBytes( unsigned char * p, int nLen )
{
  for( int i = 0; i < nLen; i ++ )
  {
    if( p[ i ] != 0 )
      return false;
  }

  return true;
}


//--------------------------------------------------------------------------------------------
// GetCharFreq()
//
// 0 - 621 Hz
// Z - 331 Hz  -> 30Hz per char
//--------------------------------------------------------------------------------------------
float Util::GetCharFreq( char c )
{
  if( ( c < '0' ) || ( c > 'Z' ) )
    return 0;

  float f = 331.0f + ( ( c - '0' ) * 9.66667f );

  return f;
}

//--------------------------------------------------------------------------------------------
// TransposeFill()
//--------------------------------------------------------------------------------------------
int Util::TransposeFill( short *psSource, short *psDest, double freqWanted, int nSamplesToFill )
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
// Interpolate()
//--------------------------------------------------------------------------------------------
double Util::Interpolate( double a, double b, double fRatio )
{
  return ( b - a ) * fRatio + a;
}
