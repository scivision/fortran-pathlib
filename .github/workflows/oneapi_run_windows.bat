REM batch file since Github Actions doesn't support shell cmd well,
REM and cmd is needed for oneAPI Windows

call "C:\Program Files (x86)\Intel\oneAPI\setvars.bat"
if %errorlevel% neq 0 exit /b %errorlevel%

call "C:\Program Files (x86)\Intel\oneAPI\compiler\latest\env\vars.bat
if %errorlevel% neq 0 exit /b %errorlevel%

echo "configure %GITHUB_REPOSITORY%"
cmake --preset default --install-prefix %RUNNER_TEMP%
if %errorlevel% neq 0 (
  type build\CMakeFiles\CMakeError.log & exit /b %errorlevel%
)

echo "build %GITHUB_REPOSITORY%"
cmake --build --preset default --parallel
if %errorlevel% neq 0 exit /b %errorlevel%

echo "test %GITHUB_REPOSITORY%"
ctest --preset default
if %errorlevel% neq 0 exit /b %errorlevel%

echo "install project"
cmake --install build
if %errorlevel% neq 0 exit /b %errorlevel%

echo "Example config"
cd example
cmake --preset default -DCMAKE_PREFIX_PATH:PATH=%RUNNER_TEMP%
if %errorlevel% neq 0 exit /b %errorlevel%

echo "Example build"
cmake --build --preset default --parallel
if %errorlevel% neq 0 exit /b %errorlevel%

echo "Example test"
ctest --preset default
if %errorlevel% neq 0 exit /b %errorlevel%
