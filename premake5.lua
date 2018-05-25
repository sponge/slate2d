solution "game"
  configurations { "Debug", "Release" }
  location "build"
  warnings "Extra"

  configuration { "macosx" }
    defines { "MACOS" }
    platforms { "x64" }
    buildoptions {"-Wno-unused-parameter"}

  configuration { "windows" }
    platforms { "x86", "x64" }
    disablewarnings { "4100" }

  configuration { "linux" }
    defines { "LINUX" }
    toolset "clang"
    platforms { "x64" }
  
  configuration "Debug"
    defines { "DEBUG" }
    symbols "On"
    optimize "Off"

  configuration "Release"
    defines { "NDEBUG" }
    symbols "Off"
    optimize "Full"

  project "engine"
    kind "ConsoleApp"
    language "C++"
    files { "src/**.c", "src/**.cpp", "src/**.h", "src/**.hh" }
    sysincludedirs { "libs/sdl", "libs/nanovg", "libs/tmx", "libs/imgui", "libs/physfs", "libs/glew", "libs/soloud/include" }
    debugdir "."
    targetdir "build/bin/%{cfg.buildcfg}"
    links { "nanovg", "tmx", "imgui", "physfs", "glew", "soloud", "libmodplug" }
    cppdialect "C++14"

    configuration "Release"
      kind "WindowedApp"

    configuration { "windows" }
      links { "SDL2", "SDL2main", "opengl32" }
      defines { "_CRT_SECURE_NO_WARNINGS" }

      filter "platforms:x86"
        libdirs { "libs/sdl/lib/Win32" }
        postbuildcommands {
          '{COPY} "%{wks.location}../libs/sdl/lib/win32/SDL2.dll" "%{cfg.targetdir}"'
        }

      filter "platforms:x64"
        libdirs { "libs/sdl/lib/x64" }
        postbuildcommands {
          '{COPY} "%{wks.location}../libs/sdl/lib/x64/SDL2.dll" "%{cfg.targetdir}"'
        }

    configuration { "macosx" }
      links { "OpenGL.framework", "SDL2.framework", "CoreFoundation.framework", "IOKit.framework", "CoreServices.framework", "Cocoa.framework" }
      linkoptions {"-stdlib=libc++", "-F /Library/Frameworks"}

    configuration { "linux" }
      linkoptions { "-stdlib=libc++" }

  project "game"
    kind "SharedLib"
    language "C++"
    files { "game/**.c", "game/**.cpp", "game/**.h", "game/**.hh" }
    sysincludedirs { "libs/tmx", "libs/imgui" }
    targetdir "build/bin/%{cfg.buildcfg}"
    cppdialect "C++14"
    links { "tmx", "imgui" }

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
      configuration { "macosx", "linux" }
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
      configuration { "windows" }
        libdirs { "libs/sdl/lib/Win32" }
        links { "SDL2" }

      configuration { "macosx" }
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
      configuration { "macosx" }
        defines { "HAVE_SETENV" }