# benson-sound
Recreation of "Benson" sound - sound emitted by onboard computer in Mercenary game series. 

The goal of the project is to create a portable replacement of what is available in the MDDClone version of the game, which will then be reused in MDDClone-SDL version

Author: Paul Spark

You can read more about Mercenary here:
https://en.wikipedia.org/wiki/Mercenary_(video_game)

You can find more detailed information about the game on a fan page:
http://mercenarysite.free.fr/merce.htm

Or play the games using two available ports:
http://mercenarysite.free.fr/mddclone.htm

# Building

## Installing dependencies 

Reqires the SDL 1.2 library. 
Installation instructions:

### Ubuntu / Debian / derrivative

```bash
sudo apt install cmake libsdl1.2-dev
```

### Mac OSX

```bash
brew install cmake sdl
```

### MinGW

Requires to be installed via GUI:

```bash
mingw-w64-x86_64-toolchain make mingw-w64-x86_64-cmake mingw-w64-x86_64-SDL
```

## Compilation

```bash
mkdir -p build && cd build
cmake ..
make
make install # installs locally
```

# Usage

```bash
./benson-sound "word1 word2 word3"
```

```bash
./benson-sound "abcdefghijklmnopqrstuwxyz"; ./benson-sound "zywutsrqponmlkjihgfedcba"
```

# Demo

```bash
./benson-sound
```

