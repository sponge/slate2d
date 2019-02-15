# Slate2D
If you don't already know what this is, this is probably not for you!

Slate2D is my toy 2D engine. It is small, but not minimal. It has an API modelled after fantasy consoles, but without any designed
restrictions in place. Slate2D is programmer-centric; you'll want to be familiar with C/C++, and have the ability to build and debug
the engine yourself. Slate2D gives you the *blank slate* (get it??) to let you code.

Slate2D will feel somewhat familiar for anyone who has ever worked with engines like Source, Quake, or Doom. Convars, commands, and
binds are all present. The game is loaded into the engine by way of shared library, and rendering is abstracted out into high-level
render commands, which are handled by the engine. This means your game DLL can be relatively pure, and won't rely on linking to
graphics or system libraries.

The default game DLL integrates the [Wren](https://github.com/wren-lang/wren) scripting language. Although the intent originally was
to write game code in C, I found the Wren language and really enjoyed writing code in it. It's easy enough to swap out, though. Other
scripting languages like Duktape or Lua could be used, or any language that can build a shared library and call C functions. If
you've found some neat programming thing, and wish you had an easy way to draw some graphics with it, Slate2D can get you off the
ground.

## Instructions

Slate2D uses [Premake5](https://premake.github.io/) to generate project files. While the engine is intended to be cross-platform,
right now only Windows and WASM/Emscripten builds are viable. Mac builds are bitrotted. Run a premake command such as
`premake5 vs2017` to generate the project, or `premake5 --help` to see extra options specific to the project. An attempt has been made
to make it build clean out of the box. This has been confirmed to work on *two* different computers!

## Other features
- Asset loader to let you work with asset handles instead of files
  - PNG images
  - Spritesheets: Minimize draw calls by loading a sprite atlas
  - Fonts: TTF and sprite-based
  - Audio: OGG and tracker formats
  - Tilemaps: Load and draw TMX maps
  - Shaders: Customize drawing of shapes and sprites
  - Canvas: Draw onto an off-screen texture
  - Text to Speech: The most important
- Built in object inspector for Wren objects.
- Watch for changed files, and run a command on modification.
- Optional automatic sprite atlas packer.
- No documentation.
- Inconsistent usage of C and C++ patterns, and code style.
- It mostly works!

## Libraries used

- [SDL](https://www.libsdl.org) - windowing, OpenGL context, input, DLL support
- [Wren](https://www.wren.io) - embedded scripting language  
- [PhysicsFS](https://icculus.org/physfs/) - virtual filesystem, zip loading
- [dear imgui](https://github.com/ocornut/imgui) - debugging tools  
- [SoLoud](http://sol.gfxile.net/soloud/) - audio engine  
- [TMX C Loader](https://github.com/baylej/tmx) - loading [Tiled](https://www.mapeditor.org/) maps
- [GLEW](http://glew.sourceforge.net/) - GL extension support
- [rlgl](https://www.raylib.com/) - OpenGL wrapper
- [crunch](https://github.com/ChevyRay/crunch/) - sprite packing

## Shipped games

Slate2D has still successfully been used in two game jam games!

![Hot Air Screenshot](https://img.itch.zone/aW1hZ2UvMzUxNTAyLzE3NTA0MjMucG5n/original/vuv6kk.png)
[**Hot Air**](https://spongeh.itch.io/hot-air) - a Balloon Fight style game.

![Clive Sturridge Screenshot](https://img.itch.zone/aW1hZ2UvMjg0ODQ1LzEzOTA0MzkucG5n/original/yuvfpk.png)
[**Clive Sturridge's Battlement Defence**](https://alligator.itch.io/clive) - a mix of tower defense and Rampart.
