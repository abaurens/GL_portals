-- portal (workspace)

require "premake/workspace-files"

workspace "portal"
  architecture("x86_64")
  startproject("portal")

  configurations {
    "Release",
    "Debug"
  }

IncludeDir = {}
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Enables OpenMP API for shared-memory parallel programming
--openmp "On"

--vectorextensions "SSE"
--vectorextensions "SSE2"
--vectorextensions "SSE3"
--vectorextensions "SSE4.1"
--vectorextensions "SSE4.2"

defines {
    --- temporary : stb will most likely be reworked
    "",
}

workspace_files {
  "premake5.lua",
  ".gitignore"
}

filter "configurations:windows"
  defines "_WIN32"

filter "configurations:Debug"
  defines "_DEBUG"
  runtime "Debug"
  symbols "On"
  
filter "configurations:Release"
  runtime "Release"
  optimize "On"

-- dependencies compiled from source
group "Dependencies"
  -- include("libs/dependency_source_code")

group ""
  include("portal")
