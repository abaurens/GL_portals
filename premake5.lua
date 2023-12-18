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
  "_CRT_SECURE_NO_WARNINGS", -- Disable windows warnings about stdlib functions
  "_USE_MATH_DEFINES", -- We want to have access to M_PI trough <math.h>
  "GLM_FORCE_SWIZZLE", -- We want to enable GLM swizzle features
  "GLFW_INCLUDE_NONE", -- We're using glad, so we don't want GLFW to include OpenGL
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
  include("libs/glm")
  include("libs/glad")
  include("libs/glfw")
  include("libs/json")

group ""
  include("portal")
