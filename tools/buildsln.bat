@REM Define help message
set "help_line.01=Allowed options:"
set "help_line.02= "
set "help_line.03=required arguments:"
set "help_line.04=    --build-type <arg>      set target build type, pass this argument when running as a standalone command (without gensln)"
set "help_line.05=                            can be one of the following: Debug, Release, RelWithDebInfo, MinSizeRel"
set "help_line.06=    --build-dir <arg>       set build directory, pass this argument when running as a standalone command (without gensln)"
set "help_line.07= "
set "help_line.08=optional arguments:"
set "help_line.09=    -h [ --help ]           produce help message"
set "help_line.10=    --cmake-options <args>  define additional cmake options you want to pass to the build"
set "help_line.11=                            for a list of available options, see https://cmake.org/cmake/help/latest/manual/cmake.1.html#build-a-project"

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
  if not defined build_type (
    echo -- No build type specified. See -h, --help for more information.
    EXIT /B 33
  )
)

@REM Set build directory
if defined option--build-dir (
  set "build_dir=!option--build-dir!"
  echo -- Build directory is set to: !build_dir!
  ) else (
  if not defined build_dir (
    echo -- No build directory specified. See -h, --help for more information.
    EXIT /B 33
  )
)

@REM Run cmake build
set "cmd=cmake --build !build_dir!/!build_type! --config !build_type! --target ocxreader !cmake_options!"
echo %cmd%
call %cmd%
