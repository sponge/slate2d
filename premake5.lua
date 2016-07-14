solution "game"
   configurations { "Debug", "Release" }
   location "build"
   platforms {"x86"}

   project "game"
      kind "ConsoleApp"
      language "C++"
      files { "src/**.c", "src/**.cpp", "src/**.h", "src/**.hh" }
      includedirs { "include", "nanovg", "tmx" }
      debugdir "."
      targetdir "bin/%{cfg.buildcfg}"
      links { "nanovg", "tmx" }

      configuration { "windows" }
         libdirs { "lib/Win32" }
         links { "SDL2", "SDL2main", "glew32s", "opengl32", "libxml2" }
         defines { "_CRT_SECURE_NO_WARNINGS" }

         postbuildcommands {
            '{COPY} "%{wks.location}../lib/win32/iconv.dll" "%{cfg.targetdir}"',
            '{COPY} "%{wks.location}../lib/win32/libxml2.dll" "%{cfg.targetdir}"',
            '{COPY} "%{wks.location}../lib/win32/SDL2.dll" "%{cfg.targetdir}"',
         }

      -- configuration { "linux" }
      --    linkoptions { "`pkg-config --libs glfw3`" }
      --    links { "GL", "GLU", "m", "GLEW" }
      --    defines { "NANOVG_GLEW" }

      -- configuration { "macosx" }
      --    libdirs { "lib/osx" }
      --    links { "glfw3", "GLEW" }
      --    linkoptions { "-framework OpenGL", "-framework Cocoa", "-framework IOKit", "-framework CoreVideo" }

      configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols", "ExtraWarnings" }

      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize", "ExtraWarnings" }

   	project "nanovg"
		language "C"
		kind "StaticLib"
		includedirs { "nanovg" }
		files { "nanovg/**.c", "nanovg/**.h" }
		targetdir("build")
		defines { "_CRT_SECURE_NO_WARNINGS" }

      project "tmx"
      language "C"
      kind "StaticLib"
      includedirs { "include", "tmx" }
      files { "tmx/**.c", "tmx/**.h" }
      targetdir("build")
      defines { "_CRT_SECURE_NO_WARNINGS" }
		
		configuration "Debug"
			defines { "DEBUG" }
			flags { "Symbols", "ExtraWarnings"}

		configuration "Release"
			defines { "NDEBUG" }
			flags { "Optimize", "ExtraWarnings"}