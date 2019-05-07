@ECHO off
FOR /F "tokens=*" %%g IN ('vswhere -latest -property productPath') do (SET DEVENVPATH=%%g)
echo Building libraries
"%DEVENVPATH%" /Build "Debug|x64" mandelbrotlib.sln
"%DEVENVPATH%" /Build "Debug|x86" mandelbrotlib.sln
echo Completed building libraries


