@REM Define help message
set "help_line.01=Allowed options:"
set "help_line.02= "
set "help_line.03=required arguments:"
set "help_line.04=    --vcpkg <path>          the path to the vcpkg directory (e.g. C:/dev/vcpkg)"
set "help_line.05= "
set "help_line.06=optional arguments:"
set "help_line.07=    -h [ --help ]           produce help message"
set "help_line.08=    --build-type <arg>      manually set target build type (default: Release)"
set "help_line.09=                            can be one of the following: Debug, Release, RelWithDebInfo, MinSizeRel"
set "help_line.10=    --build-dir <arg>       manually set build directory (default: build)"
set "help_line.11=    --cmake-options <args>  define additional cmake options you want to pass to the project generation"
set "help_line.12=                            for a list of available options, see https://cmake.org/cmake/help/latest/manual/cmake.1.html#generate-a-project-buildsystem"

@REM https://stackoverflow.com/a/28112521
@REM Parse arguments that start with - and creates a series of variables that
@REM start with "option" and the names and values of all options given.
set "option="
set "cmake_options="
set "cmake_options_cache="
for %%a in (%*) do (
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

  if not defined option (
    @REM Assign passed arguments
    if "!arg:~0,2!" equ "--" set "option=!arg!"
  ) else (
    if "!option!" equ "--cmake-options" (
      @REM Parse CMake cache options using -DCMAKE_*=* syntax -> Workaround for
      @REM keeping "=" in the value of the option.
      if "!arg:~0,7!" equ "-DCMAKE" (
        set "cmake_options_cache=!arg!"
        set "arg=
      )
      if "!cmake_options_cache!" neq "" (
        if "!arg!" neq "" (
          set "arg=!cmake_options_cache!^=!arg!"
          set "cmake_options_cache="
        )
      )
      if "!arg!" neq "" (
        set "cmake_options=!arg!"
      )
    ) else (
      set "option!option!=!arg!"
      set "option="
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
    EXIT /B 33
  )
  echo -- vcpkg directory is set to: !vcpkg_dir!
  ) else (
  echo %missing_arg_msg%
  EXIT /B 33
)

if defined option--build-type (
  set "build_type=!option--build-type!"
  @REM Check for valid build type
  if "!build_type!" neq "Debug" (
    if "!build_type!" neq "Release" (
      if "!build_type!" neq "RelWithDebInfo" (
        if "!build_type!" neq "MinSizeRel" (
          echo -- Invalid build type: !build_type!
          EXIT /B 33
        )
      )
    )
  )
  echo -- Build type is set to: !build_type!
  ) else (
  set "build_type=Release"
  echo -- No build type specified. Defaulting to: !build_type!
)

@REM Set build directory
if defined option--build-dir (
  set "build_dir=!option--build-dir!"
  echo -- Build directory is set to: !build_dir!
  ) else (
  set "build_dir=./build"
  echo -- No build directory specified. Defaulting to: !build_dir!
)

echo -- Checking arguments: done

@REM Generate project
echo -- Start build
set "cmd=cmake -S . -B !build_dir!/!build_type! -DCMAKE_BUILD_TYPE=!build_type! -DCMAKE_TOOLCHAIN_FILE=!vcpkg_dir!/scripts/buildsystems/vcpkg.cmake !cmake_options!"
echo %cmd%
call %cmd%
