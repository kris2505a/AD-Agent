project "Agent"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++23"

    targetdir("%{wks.location}/Binaries/%{cfg.buildcfg}")
    objdir("%{wks.location}/Intermediate/%{cfg.buildcfg}/%{prj.name}")

    files {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs {
        "%{wks.location}/Core/src"
    }

    defines {
        "UNICODE",
        "_UNICODE",
        "NOMINMAX"
    }

    links {
        "Core"
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

    filter {}