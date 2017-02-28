solution "game"
  configurations { "Debug", "Release" }
  location "build"
  warnings "Extra"

  configuration { "macosx" }
    platforms { "x64" }
    buildoptions {"-Wno-unused-parameter"}

  configuration { "windows" }
    platforms { "x86" }
    disablewarnings { "4100" }
  
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
    sysincludedirs { "include", "nanovg", "tmx", "imgui", "physfs", "entityx", "glew" }
    debugdir "."
    targetdir "bin/%{cfg.buildcfg}"
    links { "nanovg", "tmx", "imgui", "physfs", "glew", "entityx" }
    flags { "C++14" }

    configuration { "windows" }
      libdirs { "lib/Win32" }
      links { "SDL2", "SDL2main", "opengl32" }
      defines { "_CRT_SECURE_NO_WARNINGS" }
      postbuildcommands {
        '{COPY} "%{wks.location}../lib/win32/SDL2.dll" "%{cfg.targetdir}"'
      }

    configuration { "macosx" }
      links { "OpenGL.framework", "SDL2.framework", "CoreFoundation.framework", "IOKit.framework", "CoreServices.framework", "Cocoa.framework" }
      linkoptions {"-stdlib=libc++", "-F /Library/Frameworks"}

  project "game"
    kind "SharedLib"
    language "C++"
    files { "game/**.c", "game/**.cpp", "game/**.h", "game/**.hh" }
    sysincludedirs { "include", "nanovg", "tmx", "imgui" }
    targetdir "bin/%{cfg.buildcfg}"
    links { "nanovg", "tmx", "imgui" }

    configuration { "windows" }
      libdirs { "lib/Win32" }
      links { "SDL2", "SDL2main", "opengl32" }
      defines { "_CRT_SECURE_NO_WARNINGS" }

    configuration { "macosx" }
      links { "OpenGL.framework", "SDL2.framework", "CoreFoundation.framework", "IOKit.framework", "CoreServices.framework", "Cocoa.framework" }
      linkoptions {"-stdlib=libc++", "-F /Library/Frameworks"}

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
    configuration { "macosx" }
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

  project "entityx"
    language "C++"
    kind "StaticLib"
    includedirs { "entityx" }
    files { "entityx/**.cc", "entityx/**.h" }
    targetdir "build/%{cfg.buildcfg}"
    flags { "C++14" }
    warnings "Off"
    configuration { "macosx" }
      buildoptions {"-stdlib=libc++"}