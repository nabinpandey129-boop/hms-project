@echo off
rem Rebuild the page (needs Python; skipped if Python is not installed)
where python >nul 2>nul && python web\make_page.py
g++ -std=c++17 -O2 -o hms.exe main.cpp -lws2_32
if errorlevel 1 (echo Build failed) else (echo Built hms.exe - run .\hms.exe)
pause
