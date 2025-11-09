@echo off
echo ╔═══════════════════════════════════════════════════════════╗
echo ║          OOPSCP - Build Script                           ║
echo ╚═══════════════════════════════════════════════════════════╝
echo.

REM Check if g++ is available
where g++ >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    echo [INFO] Using g++ compiler...
    echo [INFO] Compiling with C++17 standard...
    echo.
    
    g++ -std=c++17 -Wall -o oopscp.exe src/main.cpp src/FileUtils.cpp src/HashCalculator.cpp src/FileEntry.cpp src/Snapshot.cpp src/Repository.cpp src/FileScanner.cpp src/DiffEngine.cpp src/Deduplicator.cpp src/IntegrityChecker.cpp
    
    if %ERRORLEVEL% EQU 0 (
        echo.
        echo [SUCCESS] Build completed successfully!
        echo [INFO] Executable created: oopscp.exe
        echo.
        echo To run the program, type: oopscp.exe
    ) else (
        echo.
        echo [ERROR] Build failed. Please check for compilation errors.
    )
) else (
    echo [ERROR] g++ compiler not found!
    echo [INFO] Please install MinGW-w64 or use Visual Studio compiler.
    echo.
    echo Alternative: Use MSVC with this command:
    echo cl /EHsc /std:c++17 src/*.cpp /Fe:oopscp.exe
)

echo.
pause
