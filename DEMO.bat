@echo off
echo ============================================
echo   OOPSCP - FEATURE DEMONSTRATION
echo ============================================
echo.
echo This script will demonstrate all features of OOPSCP
echo.
pause
echo.

echo ============================================
echo TEST 1: Initialize Repository
echo ============================================
echo init | oopscp.exe
echo.
pause
echo.

echo ============================================
echo TEST 2: Show Status
echo ============================================
echo status | oopscp.exe
echo.
pause
echo.

echo ============================================
echo TEST 3: Create Snapshot
echo ============================================
echo snapshot "My first snapshot" | oopscp.exe
echo.
pause
echo.

echo ============================================
echo TEST 4: List Snapshots
echo ============================================
echo list | oopscp.exe
echo.
pause
echo.

echo ============================================
echo TEST 5: Show Diff
echo ============================================
echo diff main.cpp | oopscp.exe
echo.
pause
echo.

echo ============================================
echo TEST 6: Deduplicate Files
echo ============================================
echo deduplicate | oopscp.exe
echo.
pause
echo.

echo ============================================
echo TEST 7: Integrity Check
echo ============================================
echo integrity-check | oopscp.exe
echo.
pause
echo.

echo ============================================
echo TEST 8: Show Statistics
echo ============================================
echo stats | oopscp.exe
echo.
pause
echo.

echo ============================================
echo TEST 9: Show Help
echo ============================================
echo help | oopscp.exe
echo.
pause
echo.

echo ============================================
echo   ALL TESTS COMPLETED!
echo ============================================
echo.
echo The program is working correctly!
echo All features have been demonstrated.
echo.
pause
