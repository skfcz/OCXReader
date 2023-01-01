@echo off
setlocal EnableDelayedExpansion
set ERRORLEVEL=0

@REM https://stackoverflow.com/a/64809327
@REM Print utility
set "print{[=for /f "tokens=1* delims==" %%a in ('set " & set "]}=') do (if "%%b" equ "‎" (echo.) else (echo %%b))"

@REM Define help message
set "help_line.01=Allowed options:"
set "help_line.02=‎"
set "help_line.03=required arguments:"
set "help_line.04=    --vcpkg <path>          the path to the vcpkg directory (e.g. C:/dev/vcpkg)"
set "help_line.05=‎"
set "help_line.06=optional arguments:"
set "help_line.07=    -h [ --help ]           produce help message"
set "help_line.08=    --build-type <arg>      manually set target build type (default: Release)"
set "help_line.09=                            can be one of the following: Debug, Release, RelWithDebInfo, MinSizeRel"
set "help_line.10=    --build-dir <arg>       manually set build directory (default: build)"
set "help_line.11=    --cmake-options <args>  define any addintional cmake options you want to pass to the build"
set "help_line.12=                            for a list of available options, see https://cmake.org/cmake/help/latest/manual/cmake.1.html#generate-a-project-buildsystem"

@REM https://stackoverflow.com/a/28112521
@REM Parse arguments that start with - and creates a series of variables that
@REM start with "option" and the names and values of all options given.
set "option="
set "cmake_options="
set "found_cmake_options=false"
for %%a in (%*) do (
  if not defined option (
    set arg=%%a

    @REM Send help.
    if "!arg!" equ "--help" ( 
      %print{[% help_line %]}%
      EXIT /B 0
    )
    if "!arg!" equ "-h" (
      %print{[% help_line %]}%
      EXIT /B 0
    )

    @REM Assign passed arguments
    if "!arg:~0,2!" equ "--" set "option=!arg!"
  ) else (
    if "!option!" equ "--cmake-options" (
      set "cmake_options=!cmake_options! %%a"
      set "found_cmake_options=true"
    ) else (
      if "!found_cmake_options!" equ "true" (
        set "cmake_options=!cmake_options! %%a"
      ) else (
        set "option!option!=%%a"
        set "option="
      )
    )
  )
)

@REM Check if all the required arguments have been passed.
set missing_arg_msg=Missing arguments. See -h, --help for more information.
echo -- Checking arguments
if defined option--vcpkg (
  set "vcpkg_dir=!option--vcpkg!"
  @REM Check if vcpkg directory exists
  if not exist !vcpkg_dir!/scripts/buildsystems/vcpkg.cmake (
    echo -- Cannot find vcpkg.cmake in !vcpkg_dir!/scripts/buildsystems/
    set ERRORLEVEL=33
    GOTO :EXITSCRIPT
  )
  echo -- vcpkg directory is set to: !vcpkg_dir!
  ) else (
  echo %missing_arg_msg%
  set ERRORLEVEL=33
  GOTO :EXITSCRIPT
)

if defined option--build-type (
  @REM Check for valid build type
  if "!option--build-type!" equ "Debug" (
    set "build_type=Debug"
  ) else if "!option--build-type!" equ "Release" (
    set "build_type=Release"
  ) else if "!option--build-type!" equ "RelWithDebInfo" (
    set "build_type=RelWithDebInfo"
  ) else if "!option--build-type!" equ "MinSizeRel" (
    set "build_type=MinSizeRel"
  ) else (
    echo -- Invalid build type: %option--build-type%
    set ERRORLEVEL=33
    GOTO :EXITSCRIPT
  )
  echo -- Build type is set to: !build_type!
  ) else (
  set "build_type=Release"
  echo -- No build type specified. Defaulting to: !build_type!
)

if defined option--build-dir (
  set "build_dir=!option--build-dir!"
  echo -- Build directory is set to: !build_dir!
  ) else (
  set "build_dir=build"
  echo -- No build directory specified. Defaulting to: ./!build_dir!
)
echo -- Checking arguments: done

@REM Execute build
echo -- Start build
cmake -S . -B !build_dir! -DCMAKE_BUILD_TYPE=!build_type! -DCMAKE_TOOLCHAIN_FILE=!vcpkg_dir!/scripts/buildsystems/vcpkg.cmake !cmake_options!

:EXITSCRIPT
if %ERRORLEVEL% NEQ 0 (
  echo The command failed with exit code %ERRORLEVEL%
)
EXIT /B %ERRORLEVEL%
