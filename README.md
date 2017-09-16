# *Frostbite*

**Frostbite* is *Breeanna Ebert*'s implementation of [*Design Document*](http://graphics.cs.cmu.edu/courses/15-466-f17/game1-designs/rmukunda/) for game1 in 15-466-f17.

![screenshot](https://github.com/turbotortoise/15-466-f17-base1/screenshots/Game1Screenshot.png/to/img.png)*

## Asset Pipeline

*For the Asset Pipeline, I exported each element in the provided .svg file into its own .png file. I loaded each one into the game.*

## Architecture

*The code is divided into initialization, game state, and draw state. All variables are initialized, updated within the game state, and drawn in the draw state.*

## Reflection

*The state of the animals and trees once the player destroys these objects is ambiguous. The animals should respawn, and the trees should grow over time. I can't simply instantiate and destroy objects, especially if I need to respawn the object into its original space.
I resolved this by creating states for the trees, and an alive and dead state for the animals. If the tree has recently been cut, meaning that it hasn't reached full height yet, then it appears as a cut tree. Once it reaches full height, then the tree sprite appears. If an animal is dead, is simply is moved off screen, and its speed is decreased.
I also did not spawn meat and lumber when these objects are destroyed, so they are simply automatically added to inventory.*

*Interaction with the wizard was not clear, and the wizard doesn't seem to be related too much towards the theme of the game. I wasn't sure if the player receives both a health boost and slower temperature decline, or whether the player had to choose one. I chose to activate both.*


# About Base1

This game is based on Base1, starter code for game1 in the 15-466-f17 course. It was developed by Jim McCann, and is released into the public domain.

## Requirements

 - modern C++ compiler
 - glm
 - libSDL2
 - libpng

On Linux or OSX these requirements should be available from your package manager without too much hassle.

## Building

This code has been set up to be built with [FT jam](https://www.freetype.org/jam/).

### Getting Jam

For more information on Jam, see the [Jam Documentation](https://www.perforce.com/documentation/jam-documentation) page at Perforce, which includes both reference documentation and a getting started guide.

On unixish OSs, Jam is available from your package manager:
```
	brew install ftjam #on OSX
	apt get ftjam #on Debian-ish Linux
```

On Windows, you can get a binary [from sourceforge](https://sourceforge.net/projects/freetype/files/ftjam/2.5.2/ftjam-2.5.2-win32.zip/download),
and put it somewhere in your `%PATH%`.
(Possibly: also set the `JAM_TOOLSET` variable to `VISUALC`.)

### Bulding
Open a terminal (on windows, a Visual Studio Command Prompt), change to this directory, and type:
```
	jam
```

### Building (local libs)

Depending on your OSX, clone 
[kit-libs-linux](https://github.com/ixchow/kit-libs-linux),
[kit-libs-osx](https://github.com/ixchow/kit-libs-osx),
or [kit-libs-win](https://github.com/ixchow/kit-libs-win)
as a subdirectory of the current directory.

The Jamfile sets up library and header search paths such that local libraries will be preferred over system libraries.
