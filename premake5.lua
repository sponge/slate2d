newoption {
  trigger = "emscripten",
  description = "use with gmake2 to build emscripten ready makefile",
  default = false
}

solution "game"
  configurations { "Debug", "Release" }
  location "build"
  warnings "Extra"

  filter { "system:macosx" }
    defines { "MACOS" }
    platforms { "x64" }
    buildoptions {"-Wno-unused-parameter"}

  filter { "system:windows" }
    platforms { "x86", "x64" }
    disablewarnings { "4100" }

  filter { "system:linux" }
    defines { "LINUX" }
    toolset "clang"
    platforms { "x64" }
  
  filter { "configurations:Debug" }
    defines { "DEBUG" }
    symbols "On"
    optimize "Off"

  filter { "configurations:Release" }
    defines { "NDEBUG" }
    symbols "Off"
    optimize "Full"

  project "engine"
    kind "ConsoleApp"
    language "C++"
    files { "src/**.c", "src/**.cpp", "src/**.h", "src/**.hh" }
    removefiles { "src/imgui_impl_sdl_es2.cpp" }
    sysincludedirs { "libs/sdl", "libs/nanovg", "libs/tmx", "libs/imgui", "libs/physfs", "libs/glew", "libs/soloud/include" }
    debugdir "."
    targetdir "build/bin/%{cfg.buildcfg}"
    links { "nanovg", "tmx", "imgui", "physfs", "glew", "soloud", "libmodplug" }
    cppdialect "C++14"

    filter { "configurations:Release" }
      kind "WindowedApp"

    filter { "system:windows" }
      links { "SDL2", "SDL2main", "opengl32" }
      defines { "_CRT_SECURE_NO_WARNINGS" }

      filter { "platforms:x86", "system:windows" }
        libdirs { "libs/sdl/lib/Win32" }
        postbuildcommands {
          '{COPY} "%{wks.location}../libs/sdl/lib/win32/SDL2.dll" "%{cfg.targetdir}"'
        }

      filter { "platforms:x64", "system:windows" }
        libdirs { "libs/sdl/lib/x64" }
        postbuildcommands {
          '{COPY} "%{wks.location}../libs/sdl/lib/x64/SDL2.dll" "%{cfg.targetdir}"'
        }

    filter { "system:macosx" }
      links { "OpenGL.framework", "SDL2.framework", "CoreFoundation.framework", "IOKit.framework", "CoreServices.framework", "Cocoa.framework" }
      linkoptions {"-stdlib=libc++", "-F /Library/Frameworks"}

    filter { "system:linux" }
      linkoptions { "-stdlib=libc++" }

    filter { "action:gmake2", "options:emscripten" }
      targetextension ".bc"
      files { "src/imgui_impl_sdl_es2.cpp" }
      removefiles { "src/imgui_impl_sdl_gl3.cpp" }
      links { "game" }
      postbuildcommands { "mkdir html; emcc -O2 --preload-file ../base --preload-file ../plat -s USE_SDL=2 %{cfg.targetdir}/engine.bc -o html/engine.html" }

  project "game"
    kind "SharedLib"
    language "C++"
    files { "game/**.c", "game/**.cpp", "game/**.h", "game/**.hh" }
    sysincludedirs { "libs/tmx", "libs/imgui" }
    targetdir "build/bin/%{cfg.buildcfg}"
    cppdialect "C++14"
    links { "tmx", "imgui" }
    filter { "action:gmake2", "options:emscripten" }
        kind "StaticLib"
        targetdir "build/%{cfg.buildcfg}"

  group "libraries"

    project "nanovg"
      language "C"
      kind "StaticLib"
      includedirs { "libs/nanovg" }
      files { "libs/nanovg/**.c", "libs/nanovg/**.h" }
      targetdir "build/%{cfg.buildcfg}"
      defines { "_CRT_SECURE_NO_WARNINGS" }
      warnings "Off"

    project "tmx"
      language "C++"
      kind "StaticLib"
      files { "libs/tmx/**.c", "libs/tmx/**.h", "libs/tmx/**.cpp" }
      targetdir "build/%{cfg.buildcfg}"
      defines { "_CRT_SECURE_NO_WARNINGS" }
      cppdialect "C++14"
      filter { "system:macosx or system:linux" }
        buildoptions {"-stdlib=libc++"}

    project "imgui"
      language "C++"
      kind "StaticLib"
      files { "libs/imgui/**.cpp", "libs/imgui/**.h" }
      targetdir "build/%{cfg.buildcfg}"
      warnings "Off"

    project "physfs"
      language "C"
      kind "StaticLib"
      undefines { "DEBUG" } -- fixes a weird issue on mac
      defines { "_CRT_SECURE_NO_WARNINGS", "PHYSFS_SUPPORTS_ZIP", "PHYSFS_SUPPORTS_QPAK", "PHYSFS_INTERNAL_ZLIB"}
      files { "libs/physfs/**.c", "libs/physfs/**.h" }
      targetdir "build/%{cfg.buildcfg}"
      warnings "Off"

    project "glew"
      language "C++"
      kind "StaticLib"
      defines { "GLEW_STATIC" }
      includedirs { "libs/glew" }
      files { "libs/glew/**.c", "libs/glew/**.h" }
      targetdir "build/%{cfg.buildcfg}"
      warnings "Off"

    project "soloud"
      language "C++"
      kind "StaticLib"
      targetdir "build/%{cfg.buildcfg}"
      targetname "soloud_static"
      warnings "Off"
      sysincludedirs { "libs/sdl" }
      defines { "MODPLUG_STATIC", "WITH_MODPLUG", "WITH_SDL2_STATIC" }
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
      filter { "system:windows" }
        libdirs { "libs/sdl/lib/Win32" }
        links { "SDL2" }

      filter { "system:macosx" }
        links { "SDL2.framework" }
        linkoptions {"-F /Library/Frameworks"}

    project "libmodplug"
      kind "StaticLib"
      targetdir "build/%{cfg.buildcfg}"
      language "C++"
      defines { "MODPLUG_STATIC", "HAVE_SINF" }
      files { "libs/soloud/ext/libmodplug/src/**.cpp*" }
      includedirs { "libs/soloud/ext/libmodplug/src/**" }
      targetname "libmodplug"
      warnings "Off"
      characterset "MBCS"
      filter { "system:macosx" }
        defines { "HAVE_SETENV" }
      filter { "action:gmake2", "options:emscripten" }
        defines { "HAVE_STDINT_H", "HAVE_SETENV" } 
