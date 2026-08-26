workspace "AdAgent"
    architecture "x64"

    configurations {
        "Debug",
        "Release"
    }

    startproject "Agent"

    include "Core"
    include "Agent"