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

    protected:

    private:
        void SoundInit();

        const SDLStuff & m_sdlStuff;
};

#endif // BENSONSOUND_H
