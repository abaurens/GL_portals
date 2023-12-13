-- __Project_Name__ (project)
project "__Project_Name__"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++20"
  staticruntime "On"

  targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
  objdir ("%{wks.location}/build/" .. outputdir .. "%{prj.name}")

  IncludeDir["__Project_Name__"] = "%{wks.location}/__Project_Name__/include"

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
	-- IncludeDir["dependency"]
    "include/"
  }

  defines {
  }

  links {
    -- "dependency"
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
