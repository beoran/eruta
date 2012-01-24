--
-- Premake 4.x build configuration script
-- 

--
-- Define the project. Put the release configuration first so it will be the
-- default when folks build using the makefile. That way they don't have to 
-- worry about the /scripts argument and all that.
--

	solution "Eruta"
		configurations { "Debug", "Release", "Static" }
	
	project "Eruta"
		targetname  "eruta"
		language    "C"
		kind        "WindowedApp"
		flags       { "ExtraWarnings"}
		
		includedirs { "include", 
                  "/usr/local/include/lua", "/usr/local/include/chipmunk",
                  "/usr/local/include"    , "/usr/local/include/allegro5" ,                  
                  "/usr/include"          , "/usr/include/lua", 
                  "/usr/include/chipmunk" , "/usr/include/allegro5" 
                  }

		libdirs     { "/usr/lib/i386-linux-gnu" }
		links       { 
                  "allegro"           , "allegro_main"  ,
                  "allegro_font"      , 
                  "allegro_image"     , "allegro_acodec",
                  "allegro_primitives", "allegro_ttf"   ,
                  "allegro_audio"     , "allegro_dialog",
                  "allegro_memfile"   , "allegro_acodec",
                  "allegro_color"     , "allegro_physfs",
                  "chipmunk"          , "lua"           ,
                  "m"
                 }
		
		files {
			"*.txt", "**.lua", "include/**.h", "src/**.c",
		}

		configuration "Debug"
			targetdir   "bin"
			defines     "CONFIG_DEBUG"
			flags       { "Symbols" }
			
		configuration "Release"
			targetdir   "bin"
			defines     "NDEBUG"
			flags       { "OptimizeSize" }
			
		configuration "Static"
		  flags       { "StaticRuntime" }
		  

--[[	
    configuration "linux"
			defines     { "LUA_USE_POSIX", "LUA_USE_DLOPEN" }
			links       { "m", "dl" } 
			
		configuration "macosx"
			defines     { "LUA_USE_MACOSX" }
			
		configuration { "macosx", "gmake" }
			buildoptions { "-mmacosx-version-min=10.1" }
			linkoptions { "-lstdc++-static", "-mmacosx-version-min=10.1" }

		configuration { "not windows", "not solaris" }
			linkoptions { "-rdynamic" }
			
		configuration { "solaris" }
			linkoptions { "-Wl,--export-dynamic" }
]]


--
-- A more thorough cleanup.
--
	if _ACTION == "clean" then
		os.rmdir("bin")
		os.rmdir("build")
	end
	
