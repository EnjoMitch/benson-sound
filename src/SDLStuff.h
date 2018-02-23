#ifndef SDLSTUFF_H
#define SDLSTUFF_H

class SDL_Surface;
class SDLStuff
{
    public:
        SDLStuff();
        virtual ~SDLStuff();

        void MainLoop() const;

    protected:

    private:
        void Init();

        SDL_Surface * screen;
};

#endif // SDLSTUFF_H
