-- portal (project)
project "portal"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++20"
  staticruntime "On"

  targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
  objdir ("%{wks.location}/build/" .. outputdir .. "%{prj.name}")

  IncludeDir["portal"] = "%{wks.location}/portal/include"

  -- Not using pre compiled header yet --
  -- pchheader "pch.h"
  -- pchsource ("src/pch.cpp")

  files {
    "premake5.lua",

    "include/**.h",
    "include/**.hpp",

    "source/**.h",
    "source/**.c",
    "source/**.hpp",
    "source/**.cpp",
    "source/**.tpp",
  }

  includedirs {
	"%{IncludeDir.glm}",
    "%{IncludeDir.glad}",
    "%{IncludeDir.glfw}",
    "include/"
  }

  defines {
  }

  links {
    "glad", 
    "glfw"
  }

  filter "system:linux"
    pic "On"
  
  filter "system:macosx"
    pic "On"

  filter "configurations:Debug"
    runtime "Debug"
    symbols "On"
    
  filter "configurations:Release"
    defines "NDEBUG"
    runtime "Release"
    optimize "On"
