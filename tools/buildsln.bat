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
set "help_line.10=    <args>                  define additional cmake options you want to pass to the build"
set "help_line.11=                            for a list of available options, see https://cmake.org/cmake/help/latest/manual/cmake.1.html#build-a-project"

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

  @REM Assign passed arguments
  if "!arg!" equ "--build-type" set "build_type=!arg!" && continue
  if "!arg!" equ "--build-dir" set "build_dir=!arg!" && continue

  set "rest=!rest! %%a"
)

if defined build_type (
  @REM Check for valid build type
  if "!build_type!" neq "Debug" (
    if "!build_type!" neq "Release" (
      if "!build_type!" neq "RelWithDebInfo" (
        if "!build_type!" neq "MinSizeRel" (
          echo -- Invalid build type specified: !build_type!
          EXIT /B 33
        )
      )
    )
  )
  echo -- Build type is set to: !build_type!
  ) else (
  echo -- No build type specified. See -h, --help for more information.
  EXIT /B 33
)

@REM Set build directory
if defined build_dir (
  echo -- Build directory is set to: !build_dir!
  ) else (
  echo -- No build directory specified. See -h, --help for more information.
  EXIT /B 33
)

@REM Run cmake build
set "cmd=cmake --build ./!build_dir!/!build_type! --target ocxreader !rest!"
echo %cmd%
call %cmd%
