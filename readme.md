# RoseN - C/C++ SDL2 Game Engine

## Introduction

RoseN (Shorthand for RoseNgine) is a simple code-only game engine created for small 2D games for game jams.

## Setup

- SDL2: For SDL2 setup instructions, see [SDL2 files setup](include/readme.md)
- MinGW: Download and install [MinGW-w64](https://www.mingw-w64.org/downloads/) for Windows C/C++ compilation
- Emscripten: Download and install [Emscripten](https://emscripten.org/docs/getting_started/downloads.html) for web builds
- Make: Install Make for Windows from [GnuWin32](http://gnuwin32.sourceforge.net/packages/make.htm) or use the Make that comes with MinGW

### macOS

1. Install Homebrew (if not already installed):

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

2. Install required dependencies:

```bash
brew install sdl2
brew install sdl2_image
brew install sdl2_ttf
brew install sdl2_mixer
```

3. Install Clang (usually comes with Xcode Command Line Tools):

```bash
xcode-select --install
```

## Building

Run `make` to see build options:

- `make` - Windows builds
- `make -f Makefile-mac` - macOS builds
- `make web` - Web build (requires Emscripten)

## Running the Game

Run the debug version:

```bash
./bin/debug/rosengine
```

Or run the release version:

```bash
./bin/release/rosengine
```
