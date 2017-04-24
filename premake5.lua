solution "game"
  configurations { "Debug", "Release" }
  location "build"
  warnings "Extra"

  configuration { "macosx" }
    defines { "MACOS" }
    platforms { "x64" }
    buildoptions {"-Wno-unused-parameter"}

  configuration { "windows" }
    platforms { "x86" }
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

  -- FIXME: fix up excessive links and includes on here and game as i port everything over to a dll
  project "engine"
    kind "ConsoleApp"
    language "C++"
    files { "src/**.c", "src/**.cpp", "src/**.h", "src/**.hh" }
    sysincludedirs { "sdl", "nanovg", "tmx", "imgui", "physfs", "glew" }
    debugdir "."
    targetdir "bin/%{cfg.buildcfg}"
    links { "nanovg", "tmx", "imgui", "physfs", "glew" }
    flags { "C++14" }

    configuration { "windows" }
      libdirs { "sdl/lib/Win32" }
      links { "SDL2", "SDL2main", "opengl32" }
      defines { "_CRT_SECURE_NO_WARNINGS" }
      postbuildcommands {
        '{COPY} "%{wks.location}../sdl/lib/win32/SDL2.dll" "%{cfg.targetdir}"'
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
    sysincludedirs { "nanovg", "tmx", "imgui", "lua", "luasocket" }
    targetdir "bin/%{cfg.buildcfg}"
    flags { "C++14" }
    links { "nanovg", "tmx", "imgui", "lua", "luasocket" }
    configuration { "windows" }
      links { "ws2_32" }

  project "nanovg"
    language "C"
    kind "StaticLib"
    includedirs { "nanovg" }
    files { "nanovg/**.c", "nanovg/**.h" }
    targetdir "build/%{cfg.buildcfg}"
    defines { "_CRT_SECURE_NO_WARNINGS" }
    warnings "Off"

  project "tmx"
    language "C++"
    kind "StaticLib"
    sysincludedirs { "include" }
    files { "tmx/**.c", "tmx/**.h", "tmx/**.cpp" }
    targetdir "build/%{cfg.buildcfg}"
    defines { "_CRT_SECURE_NO_WARNINGS" }
    flags { "C++14" }
    configuration { "macosx", "linux" }
      buildoptions {"-stdlib=libc++"}

  project "imgui"
    language "C++"
    kind "StaticLib"
    files { "imgui/**.cpp", "imgui/**.h" }
    targetdir "build/%{cfg.buildcfg}"
    warnings "Off"

  project "physfs"
    language "C"
    kind "StaticLib"
    defines { "_CRT_SECURE_NO_WARNINGS", "PHYSFS_SUPPORTS_ZIP", "PHYSFS_SUPPORTS_QPAK", "PHYSFS_INTERNAL_ZLIB"}
    files { "physfs/**.c", "physfs/**.h" }
    targetdir "build/%{cfg.buildcfg}"
    warnings "Off"

  project "glew"
    language "C++"
    kind "StaticLib"
    defines { "GLEW_STATIC" }
    includedirs { "glew" }
    files { "glew/**.c", "glew/**.h" }
    targetdir "build/%{cfg.buildcfg}"
    warnings "Off"

  project "lua"
    language "C"
    kind "StaticLib"
    files { "lua/**.c", "lua/**.h" }
    targetdir "build/%{cfg.buildcfg}"
    warnings "Off"

  project "luasocket"
    language "C"
    kind "StaticLib"
    sysincludedirs { "lua" }
    targetdir "build/%{cfg.buildcfg}"
    links { "lua" }
    warnings "Off"

    files { "luasocket/luasocket.h", "luasocket/mime.h", "luasocket/luasocket.c", "luasocket/timeout.c", "luasocket/buffer.c", "luasocket/io.c", "luasocket/auxiliar.c",
						"luasocket/options.c", "luasocket/inet.c", "luasocket/except.c", "luasocket/select.c", "luasocket/tcp.c", "luasocket/udp.c", "luasocket/mime.c" }
    
    configuration "Debug"
      defines { "LUASOCKET_DEBUG" }

    configuration "windows"
      files { "luasocket/wsocket.c" }

    configuration "macosx"
      files { "luasocket/unixstream.c", "luasocket/unixdgram.c", "luasocket/usocket.c", "luasocket/buffer.c", "luasocket/auxiliar.c", "luasocket/options.c", "luasocket/timeout.c", "luasocket/io.c", "luasocket/usocket.c", "luasocket/unix.c"}
      defines {
        "UNIX_HAS_SUN_LEN",
	    }