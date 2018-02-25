#ifndef BENSONSOUND_H
#define BENSONSOUND_H

#include <string>
#include <memory>

class SDLStuff;
class BensonSound
{
    public:
        BensonSound(const SDLStuff & sdlStuff);
        virtual ~BensonSound();

        void SayBensonText( const std::string & text ); // string secures code from buffer overflow
        void Play();

        void FillBuffer( unsigned char * pChunk, int nSamplesThisChunk );

    protected:

    private:
        void SoundInit();
        void CreateBasePulseSample();

        const SDLStuff & m_sdlStuff;
        std::unique_ptr<short> uptrAsWavePulse;
        std::unique_ptr<short> uptrAsWaveBenson;
        int g_WaveOffset = 0;
        unsigned dwBackoffStart = 0;
        static const int NUM_SAMPLE_LR_PAIRS;
        static const int g_NewWaveSize;
        static const int WAVE_LENGTH;
};

#endif // BENSONSOUND_H
