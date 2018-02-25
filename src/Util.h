#ifndef UTIL_H
#define UTIL_H


class Util
{
    public:
        Util();
        virtual ~Util(){}

        static bool AllZeroBytes( unsigned char * p, int nLen );
        static float GetCharFreq( char c );
        static int TransposeFill( const short * psSource, short * psDest, double freqWanted, int nSamplesToFill );
        static double Interpolate( double a, double b, double fRatio );

    protected:

    private:
};

#endif // UTIL_H
