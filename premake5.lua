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
    sysincludedirs { "libs/sdl", "libs/nanovg", "libs/tmx", "libs/imgui", "libs/physfs", "libs/glew", "libs/soloud/include" }
    debugdir "."
    targetdir "bin/%{cfg.buildcfg}"
    links { "nanovg", "tmx", "imgui", "physfs", "glew", "soloud", "libmodplug" }
    flags { "C++14" }

    configuration { "windows" }
      libdirs { "libs/sdl/lib/Win32" }
      links { "SDL2", "SDL2main", "opengl32" }
      defines { "_CRT_SECURE_NO_WARNINGS" }
      postbuildcommands {
        '{COPY} "%{wks.location}../libs/sdl/lib/win32/SDL2.dll" "%{cfg.targetdir}"'
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
    sysincludedirs { "libs/nanovg", "libs/tmx", "libs/imgui", "libs/lua", "libs/luasocket" }
    targetdir "bin/%{cfg.buildcfg}"
    flags { "C++14" }
    links { "nanovg", "tmx", "imgui", "lua", "luasocket" }
    configuration { "windows" }
      links { "ws2_32" }

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
      flags { "C++14" }
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

    project "lua"
      language "C"
      kind "StaticLib"
      files { "libs/lua/**.c", "libs/lua/**.h" }
      targetdir "build/%{cfg.buildcfg}"
      warnings "Off"

    project "luasocket"
      language "C"
      kind "StaticLib"
      sysincludedirs { "libs/lua" }
      targetdir "build/%{cfg.buildcfg}"
      links { "lua" }
      warnings "Off"

      files { "libs/luasocket/luasocket.h", "libs/luasocket/mime.h", "libs/luasocket/luasocket.c", "libs/luasocket/timeout.c", "libs/luasocket/buffer.c", "libs/luasocket/io.c", "libs/luasocket/auxiliar.c",
              "libs/luasocket/options.c", "libs/luasocket/inet.c", "libs/luasocket/except.c", "libs/luasocket/select.c", "libs/luasocket/tcp.c", "libs/luasocket/udp.c", "libs/luasocket/mime.c" }
      
      configuration "Debug"
        defines { "LUASOCKET_DEBUG" }

      configuration "windows"
        files { "libs/luasocket/wsocket.c" }

      configuration "macosx"
        files { "libs/luasocket/unixstream.c", "libs/luasocket/unixdgram.c", "libs/luasocket/usocket.c", "libs/luasocket/buffer.c", "libs/luasocket/auxiliar.c", "libs/luasocket/options.c", "libs/luasocket/timeout.c", "libs/luasocket/io.c", "libs/luasocket/usocket.c", "libs/luasocket/unix.c"}
        defines {
          "UNIX_HAS_SUN_LEN",
        }

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
      includedirs
      {
        "libs/soloud/src/**",
        "libs/soloud/include", 
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

		defines { "MODPLUG_STATIC", "HAVE_SETENV", "HAVE_SINF" }

		files
		{
		"libs/soloud/ext/libmodplug/src/**.cpp*"
	  }

		includedirs
		{
		"libs/soloud/ext/libmodplug/src/**"
		}

		targetname "libmodplug"
    warnings "Off"