project "Core"
    kind "SharedLib"
    language "C++"
    cppdialect "C++23"

    targetdir("%{wks.location}/Binaries/%{cfg.buildcfg}")
    objdir("%{wks.location}/Intermediate/%{cfg.buildcfg}/%{prj.name}")

    files {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs {
        "src"
    }

    defines {
        "UNICODE",
        "_UNICODE",
        "NOMINMAX",
        "PRJ_CORE"
    }

    links {
        "activeds",
        "adsiid",
        "ole32",
        "oleaut32"
    }

    filter "configurations:Debug" 
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

    filter ""