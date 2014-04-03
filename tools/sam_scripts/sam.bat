@echo off
REM Launcher for Windows
REM Install with sam.py in TuttleOFX root directory

set TUTTLE_ROOT=%~dp0

set TUTTLE_HOME=%UserProfile%\cache_tuttleofxRelease
if not exist %TUTTLE_HOME% mkdir %TUTTLE_HOME%

python.exe %TUTTLE_ROOT%/sam.py %*
