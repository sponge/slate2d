require('vstudio')

premake.override(premake.vstudio.vc2010, "languageStandard", function(base, prj)
  if prj.filename == "quickjs" or prj.filename == "jsgame" then
    premake.w('<LanguageStandard_C>stdc11</LanguageStandard_C>')
  end
  base(prj)
end)

newoption {
  trigger = "emscripten",
  description = "use with gmake2 to build emscripten ready makefile",
  default = false
}

newoption {
  trigger = "default-game",
  description = "default game data dir, sets DEFAULT_GAME for engine and emscripten packaging",
  default = "plat"
}

newoption {
  trigger = "static",
  description = "compiles slate2d as a static library",
  default = false
}

workspace "Slate2D"
  configurations { "Debug", "Release" }
  location "build"
  symbols "On"
  targetdir "build/out/%{cfg.architecture}_%{cfg.buildcfg}"
  objdir "build/out/obj"
  startproject "jsgame"
  toolset "clang"

  filter "system:windows"
    platforms { "x86", "x64" }
    systemversion "latest"

  filter "system:macosx"
    defines { "MACOS", "GL_SILENCE_DEPRECATION" }
    platforms { "x64", "arm64" }

  filter "system:linux"
    platforms "x64"
  
  filter "configurations:Debug"
    defines "DEBUG"
    optimize "Off"

  filter "configurations:Release"
    defines "RELEASE"
    optimize "Full"

  project "libslate2d"
    kind "SharedLib"
    targetname "slate2d"
    language "C++"
    files { "src/**.c", "src/**.cpp", "src/**.h", "src/**.hh" }
    -- only used in emscripten build
    removefiles "src/imgui_impl_sdl_es2.cpp"
    sysincludedirs { "src/external", "libs/sdl", "libs/imgui", "libs/physfs", "libs/soloud/include", "libs/crunch" }
    -- physfs uses the exe path by default, but the game data files are in the top folder
    targetdir "build/bin/%{cfg.architecture}_%{cfg.buildcfg}"
    links { "imgui", "physfs", "soloud", "crunch" }
    cppdialect "C++14"
    -- define so engine and emscripten packaging are always in sync on the same base game folder 
    defines { "DEFAULT_GAME=\"" .. _OPTIONS["default-game"] .. "\"", "SLT_COMPILE_DLL" }

    -- link SDL bins in the source tree on windows
    filter "system:windows"
      links { "SDL2", "opengl32" }
      defines { "_CRT_SECURE_NO_WARNINGS", "_CRT_NONSTDC_NO_DEPRECATE" }

    filter { "platforms:x86", "system:windows" }
      libdirs "libs/sdl/lib/Win32"
      -- need to copy x86 sdl runtime to output
      postbuildcommands {
        '{COPY} "%{wks.location}../libs/sdl/lib/Win32/SDL2.dll" "%{cfg.targetdir}" ',
        '{COPY} "%{wks.location}../libs/openmpt/Win32/*.dll" "%{cfg.targetdir}" '
      }

    filter { "platforms:x64", "system:windows" }
      libdirs "libs/sdl/lib/x64"
      -- need to copy x64 sdl runtime to output
      postbuildcommands {
        '{COPY} "%{wks.location}../libs/sdl/lib/x64/SDL2.dll" "%{cfg.targetdir}" ',
        '{COPY} "%{wks.location}../libs/openmpt/x64/*.dll" "%{cfg.targetdir}" '
      }

    -- use SDL2 from homebrew
    filter "system:macosx"
      links { "SDL2", "CoreFoundation.framework", "IOKit.framework", "CoreServices.framework", "Cocoa.framework", "OpenGL.framework" }
      linkoptions "-stdlib=libc++"

    filter { "system:macosx", "platforms:arm64" }
      linkoptions "-L /opt/homebrew/lib"

    filter { "system:macosx", "platforms:x64" }
      linkoptions "-L /usr/local/lib"

    -- emscripten uses opengl es2, not gl3
    filter { "action:gmake2", "options:emscripten" }
      targetextension ".bc"
      files "src/imgui_impl_sdl_es2.cpp"
      removefiles "src/imgui_impl_sdl_gl3.cpp"
      -- build the html bundle, overlay the default game folder on top of base
      postbuildcommands {
        "mkdir html", 
        "emcc -O2 --preload-file ../base --preload-file ../" .. _OPTIONS["default-game"] .. " -s ALLOW_MEMORY_GROWTH=1 -s USE_SDL=2 %{cfg.targetdir}/engine.bc -o html/index.html --shell-file ../src/emshell.html",
      }
      
    filter "system:linux"
      links { "SDL2", "dl", "pthread", "GL" }

    filter "options:static"
      kind "StaticLib"
      defines "SLT_STATIC"

  project "jsgame"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++latest"
    targetname "jslate2d"
    files { "jsgame/**.cpp", "jsgame/**.h" }
    sysincludedirs { "libs/quickjs", "libs/imgui" }
    targetdir "build/bin/%{cfg.architecture}_%{cfg.buildcfg}"
    debugargs { "+set", "fs.basepath", path.getabsolute(".")}
    links { "imgui", "SDL2main", "libslate2d", "quickjs" }

    filter { "platforms:x86", "system:windows" }
      libdirs "libs/sdl/lib/Win32"

    filter { "platforms:x64", "system:windows" }
      libdirs "libs/sdl/lib/x64"

    filter "system:windows"
      defines { "_CRT_SECURE_NO_WARNINGS", "_CRT_NONSTDC_NO_DEPRECATE" }
      links { "ws2_32" }

    -- use SDL2 from homebrew
    filter { "system:macosx", "platforms:arm64" }
      linkoptions {"-stdlib=libc++", "-L /opt/homebrew/lib" }

    filter { "system:macosx", "platforms:x64" }
      linkoptions {"-stdlib=libc++", "-L /usr/local/lib" }
      
    filter "options:static"
      defines "SLT_STATIC"
     
  group "libraries"

    project "imgui"
      language "C++"
      kind "StaticLib"
      files { "libs/imgui/**.cpp", "libs/imgui/**.h" }
      warnings "Off"
      
      filter "system:linux"
        pic "On"

    project "physfs"
      language "C"
      kind "StaticLib"
      defines { "PHYSFS_SUPPORTS_ZIP", "PHYSFS_SUPPORTS_7Z", "PHYSFS_DECL=" }
      files { "libs/physfs/**.c", "libs/physfs/**.cpp", "libs/physfs/**.h", "libs/physfs/**.m" }
      warnings "Off"

      filter "system:macosx"
        undefines "DEBUG" -- fixes a weird issue on mac

    project "soloud"
      language "C++"
      kind "StaticLib"
      targetname "soloud_static"
      warnings "Off"
      sysincludedirs "libs/sdl"
      defines { "MODPLUG_STATIC", "WITH_OPENMPT", "WITH_SDL2_STATIC" }
      files {
        "libs/soloud/src/audiosource/**.c*",
        "libs/soloud/src/filter/**.c*",
        "libs/soloud/src/core/**.c*",
        "libs/soloud/src/backend/sdl2_static/**.c*"
	    }
      includedirs {
        "libs/soloud/src/**",
        "libs/soloud/include",
        "libs/sdl/SDL" 
      }

    project "crunch"
      language "C++"
      cppdialect "C++14"
      kind "StaticLib"
      includedirs { "libs/crunch", "src/" }
      sysincludedirs { "libs/physfs", "libs/imgui", "libs/sdl" }
      files { "libs/crunch/**.cpp", "libs/crunch/**.h", "libs/crunch/**.hpp" }
      warnings "Off"

    project "quickjs"
      kind "StaticLib"
      files { "libs/quickjs/**.c", "libs/quickjs/**.h" }
      defines { "CONFIG_VERSION=\"2020-11-08\"" }
      warnings "Off"
      filter { "not system:windows" }
        excludes "libs/quickjs/quickjs-debugger-transport-win.c"
      filter { "system:windows" }
        excludes "libs/quickjs/quickjs-debugger-transport-unix.c"
