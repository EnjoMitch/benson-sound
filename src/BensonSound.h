#ifndef BENSONSOUND_H
#define BENSONSOUND_H

#include <string>

class SDLStuff;
class BensonSound
{
    public:
        BensonSound(const SDLStuff & sdlStuff);
        virtual ~BensonSound();

        void SayBensonText( const std::string & text ); // string secures code from buffer overflow
        void Play();

        void FillBuffer( short *pChunk, int nSamplesThisChunk );

    protected:

    private:
        void SoundInit();
        void CreateBasePulseSample();

        const SDLStuff & m_sdlStuff;
        short * asWavePulse = 0;
        short * asWaveBenson = 0;
        int g_WaveOffset = 0;
        static const int NUM_SAMPLE_LR_PAIRS;
        static const int g_NewWaveSize;
        static const int WAVE_LENGTH;
};

#endif // BENSONSOUND_H
