$ErrorActionPreference = "Stop"

winget install Kitware.CMake
winget install FFmpeg.FFmpeg
winget install GLFW.GLFW
winget install Doxygen.Doxygen
winget install Rustlang.Rustup

Write-Host "Install MSVC Build Tools + Windows SDK via Visual Studio Installer if missing."
