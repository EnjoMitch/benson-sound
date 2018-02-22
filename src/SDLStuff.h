#ifndef SDLSTUFF_H
#define SDLSTUFF_H


class SDLStuff
{
    public:
        SDLStuff();
        virtual ~SDLStuff();

        void MainLoop() const;

    protected:

    private:
        void Init();
};

#endif // SDLSTUFF_H
