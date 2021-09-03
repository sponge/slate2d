# Slate2D
If you don't already know what this is, this is probably not for you!

[![Build status](https://ci.appveyor.com/api/projects/status/lq4xx4et7hv8oar0?svg=true)](https://ci.appveyor.com/project/sponge/slate2d)

Slate2D is my toy 2D engine. It is small, but not minimal. It has an API modelled after fantasy consoles, but without any designed
restrictions in place. Slate2D is programmer-centric; you'll want to be familiar with C/C++, and have the ability to build and debug
the engine yourself. Slate2D gives you the *blank slate* (get it??) to let you code.

Slate2D will feel somewhat familiar for anyone who has ever worked with engines like Source, Quake, or Doom. Convars, commands, and
binds are all present. Everything is provided through a library, `libslate2d`. This keeps the game's entry point relatively simple.
A game loop with mouse, keyboard, and controller support including rebindable keys only requires a few functions.

The default library host allows you to write games in JavaScript, through [QuickJS](https://bellard.org/quickjs/), but Slate2D can be
used with any language that can load a shared library and call C functions. Nim, Python, Node.js, and C# have been successfully used
in the past. If you've found some neat programming thing, and wish you had an easy way to draw some graphics with it, Slate2D can get
you off the ground.

## Instructions

Slate2D uses [Premake5](https://premake.github.io/) to generate project files. The engine should generally run on Windows, Mac, Linux,
and WASM/Emscripten, but only Windows and Mac are often checked. Run a premake command such as `premake5 vs2019` to generate the project,
or `premake5 --help` to see extra options specific to the project. An attempt has been made to make it build clean out of the box. This
has been confirmed to work on *two* different computers!

## Other features
- Asset loader to let you work with asset handles instead of files
  - PNG images: Including alpha channels
  - Spritesheets: Minimize draw calls by drawing subimages
  - Fonts: TTF and sprite-based, drawn out of an atlas
  - Audio: OGG and tracker formats
  - Tilemaps: Draw a grid of tiles based on an integer array
  - Shaders: Customize drawing of shapes and sprites
  - Canvas: Draw onto an off-screen texture
  - Text to Speech: The most important
- Built in object inspector for scripting objects
- Watch for changed files, and run a command on modification
- Optional automatic sprite atlas packer
- No documentation
- Inconsistent usage of C and C++ patterns, and code style
- It mostly works!

## Libraries used

- [SDL](https://www.libsdl.org) - windowing, OpenGL context, input, DLL support
- [QuickJS](https://bellard.org/quickjs/) - embedded scripting language  
- [PhysicsFS](https://icculus.org/physfs/) - virtual filesystem, zip loading
- [dear imgui](https://github.com/ocornut/imgui) - debugging tools  
- [SoLoud](http://sol.gfxile.net/soloud/) - audio engine  
- [rlgl](https://www.raylib.com/) - OpenGL wrapper
- [crunch](https://github.com/ChevyRay/crunch/) - sprite packing

## Shipped games

Despite the self-deprecating feature list, Slate2D has successfully been used in two game jam games!

![Hot Air Screenshot](https://img.itch.zone/aW1hZ2UvMzUxNTAyLzE3NTA0MjMucG5n/original/vuv6kk.png)
[**Hot Air**](https://spongeh.itch.io/hot-air) - a Balloon Fight style game.

![Clive Sturridge Screenshot](https://img.itch.zone/aW1hZ2UvMjg0ODQ1LzEzOTA0MzkucG5n/original/yuvfpk.png)
[**Clive Sturridge's Battlement Defence**](https://alligator.itch.io/clive) - a mix of tower defense and Rampart.
