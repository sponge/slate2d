solution "game"
   configurations { "Debug", "Release" }
   location "build"
   platforms {"x86", "x64"}

   project "game"
      kind "ConsoleApp"
      language "C++"
      files { "src/**.c", "src/**.cpp", "src/**.h", "src/**.hh" }
      includedirs { "include", "nanovg" }
      targetdir "build"
      links { "nanovg" }

      configuration { "windows" }
         libdirs { "lib/Win32" }
         links { "SDL2", "SDL2main", "glew32s", "opengl32" }
         defines { "_CRT_SECURE_NO_WARNINGS" }

         postbuildcommands {
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
		defines { "_CRT_SECURE_NO_WARNINGS" } --,"FONS_USE_FREETYPE" } Uncomment to compile with FreeType support
		
		configuration "Debug"
			defines { "DEBUG" }
			flags { "Symbols", "ExtraWarnings"}

		configuration "Release"
			defines { "NDEBUG" }
			flags { "Optimize", "ExtraWarnings"}