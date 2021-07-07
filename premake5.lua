---@diagnostic disable: undefined-global

local ProjectName = "Pathfinders"

workspace "Saffron"
	architecture "x64"

	configurations {
		"Debug",
		"Release",
		"Dist"
	}

	flags {
		"MultiProcessorCompile"
	}

    location (WrkLoc)

	startproject (ProjectName)

local SaffronEngine2D = require("ThirdParty.SaffronEngine2D.premake5")

project (ProjectName)
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
	staticruntime "On"

	objdir (OutObj)
	location (OutLoc)

    filter "configurations:Debug or Release"
	    targetdir (OutBin)
        SaffronEngine2D.PreBuild("Debug", OutBin, PrjLoc)
        SaffronEngine2D.PostBuild("Debug", OutBin, PrjLoc)
        Utils.CopyAssetsToOutput("Debug", OutBin, PrjLoc)

        SaffronEngine2D.PreBuild("Release", OutBin, PrjLoc)
        SaffronEngine2D.PostBuild("Release", OutBin, PrjLoc)
        Utils.CopyAssetsToOutput("Release", OutBin, PrjLoc)

    filter "configurations:Dist"
        targetdir (OutBinDist)
        SaffronEngine2D.PreBuild("Dist", OutBinDist, PrjLoc)
        SaffronEngine2D.PostBuild("Dist", OutBinDist, PrjLoc)
        Utils.CopyAssetsToOutput("Dist", OutBinDist, PrjLoc)

    filter "configurations:Debug or Release or Dist"

    files {
        "Source/**.h",
		"Source/**.c",
		"Source/**.hpp",
		"Source/**.cpp",
    }

    includedirs {
        "Source",
    }
    
    SaffronEngine2D.Include()
    SaffronEngine2D.Link()
    SaffronEngine2D.AddDefines()

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"

    filter "configurations:Dist"
        optimize "On"
