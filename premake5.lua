solution "game"
   configurations { "Debug", "Release" }
   location "build"
   platforms {"x86", "x64"}

   project "game"
      kind "ConsoleApp"
      language "C++"
      files { "src/**.c", "src/**.cpp", "src/**.h", "src/**.hh" }
      sysincludedirs { "include", "nanovg", "tmx", "imgui", "physfs" }
      debugdir "."
      targetdir "bin/%{cfg.buildcfg}"
      links { "nanovg", "tmx", "imgui", "physfs" }

      configuration { "windows" }
         libdirs { "lib/Win32" }
         links { "SDL2", "SDL2main", "glew32s", "opengl32", "libxml2" }
         defines { "_CRT_SECURE_NO_WARNINGS" }

         postbuildcommands {
            '{COPY} "%{wks.location}../lib/win32/iconv.dll" "%{cfg.targetdir}"',
            '{COPY} "%{wks.location}../lib/win32/libxml2.dll" "%{cfg.targetdir}"',
            '{COPY} "%{wks.location}../lib/win32/SDL2.dll" "%{cfg.targetdir}"',
         }

      configuration { "macosx" }
         --libdirs { "lib/osx" }
         links { "OpenGL.framework", "SDL2.framework", "xml2" }
         buildoptions {"-std=c++14", "-stdlib=libc++"}
         linkoptions {"-stdlib=libc++", "-F /Library/Frameworks"}

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
      sysincludedirs { "include" }
      files { "tmx/**.c", "tmx/**.h" }
      targetdir("build")
      defines { "_CRT_SECURE_NO_WARNINGS" }
		
		configuration "Debug"
			defines { "DEBUG" }
			flags { "Symbols", "ExtraWarnings"}

		configuration "Release"
			defines { "NDEBUG" }
			flags { "Optimize", "ExtraWarnings"}

	project "imgui"
	  language "C++"
	  kind "StaticLib"
	  files { "imgui/**.cpp", "imgui/**.h" }
      targetdir("build")

    project "physfs"
      language "C"
      kind "StaticLib"
      defines { "_CRT_SECURE_NO_WARNINGS", "DPHYSFS_SUPPORTS_ZIP", "PHYSFS_SUPPORTS_QPAK", "PHYSFS_INTERNAL_ZLIB"}
      files { "physfs/**.c", "physfs/**.h" }
      targetdir("build")