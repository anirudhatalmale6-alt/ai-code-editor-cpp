@echo off
REM Build script for AI Code Editor on Windows
REM Requires: CMake, Qt6, and a C++ compiler (MSVC or MinGW)

echo ================================
echo AI Code Editor - Build Script
echo ================================
echo.

REM Check for CMake
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake not found. Please install CMake and add it to PATH.
    pause
    exit /b 1
)

REM Create build directory
if not exist "build" mkdir build
cd build

REM Configure with CMake
echo Configuring with CMake...
echo.

REM Try to find Qt6 automatically or use provided path
if defined QT_DIR (
    set CMAKE_PREFIX=%QT_DIR%
) else (
    REM Common Qt installation paths
    if exist "C:\Qt\6.6.0\msvc2019_64" (
        set CMAKE_PREFIX=C:\Qt\6.6.0\msvc2019_64
    ) else if exist "C:\Qt\6.5.0\msvc2019_64" (
        set CMAKE_PREFIX=C:\Qt\6.5.0\msvc2019_64
    ) else if exist "C:\Qt\6.4.0\msvc2019_64" (
        set CMAKE_PREFIX=C:\Qt\6.4.0\msvc2019_64
    ) else (
        echo WARNING: Qt6 not found in common locations.
        echo Please set QT_DIR environment variable to your Qt installation.
        echo Example: set QT_DIR=C:\Qt\6.6.0\msvc2019_64
        pause
        exit /b 1
    )
)

echo Using Qt from: %CMAKE_PREFIX%
echo.

cmake .. -DCMAKE_PREFIX_PATH="%CMAKE_PREFIX%" -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake configuration failed.
    pause
    exit /b 1
)

REM Build
echo.
echo Building...
cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Build failed.
    pause
    exit /b 1
)

echo.
echo ================================
echo Build completed successfully!
echo Executable: build\Release\AICodeEditor.exe
echo ================================
echo.

REM Copy Qt DLLs (for running outside Qt Creator)
echo Copying Qt dependencies...
windeployqt Release\AICodeEditor.exe

echo.
echo Ready to run!
pause
