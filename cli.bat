@echo off
setlocal EnableDelayedExpansion
set ERRORLEVEL=0

@REM https://stackoverflow.com/a/64809327
@REM Print utility
set "print{[=for /f "tokens=1* delims==" %%a in ('set " & set "]}=') do (if "%%b" equ " " (echo.) else (echo %%b))"

@REM Define help message
set "help_line.01=Allowed options:"
set "help_line.02= "
set "help_line.03=Generic options:"
set "help_line.04=    -h [ --help ]         produce help message"
set "help_line.05= "
set "help_line.06=Project setup options:"
set "help_line.07=    gensln <args>         configure the project. Run gensln --help for a list of available options."
set "help_line.08=    buildsln <args>       build the project. Run buildsln --help for a list of available options."
set "help_line.09=    installsln <args>     install the project. Run installsln --help for a list of available options."
set "help_line.10= "
set "help_line.11=Project run options:"
set "help_line.12=    run <args>            run the application. Run run --help for a list of available options."

@REM Trigger help if no arguments are passed
if "%~1" equ "" (
  %print{[% help_line %]}%
  EXIT /B 0
)

@REM https://stackoverflow.com/a/28112521
@REM Parse arguments that start with - and creates a series of variables that
@REM start with "option" and the names and values of all options given.
set "gensln_options="
set "found_gensln=false"
set "found_gensln_options=false"

set "buildsln_options="
set "found_buildsln=false"
set "found_buildsln_options=false"

set "install_options="
set "found_install=false"
set "found_install_options=false"

set "run_options="
set "found_run=false"
set "found_run_options=false"

set "cmake_options_cache="

set "argument_count=0"
for %%a in (%*) do (
  set arg=%%a

  @REM Parse CMake cache options using -DCMAKE_*=* syntax -> Workaround for
  @REM keeping "=" in the value of the option.
  if "!arg:~0,7!" equ "-DCMAKE" (
    set "cmake_options_cache=!arg!"
    set "arg="
  )
  if "!cmake_options_cache!" neq "" (
    if "!arg!" neq "" (
      set "arg=!cmake_options_cache!^=!arg!"
      set "cmake_options_cache="
    )
  )

  if "!arg!" neq "" (
    @REM Send help.
    if !argument_count! equ 0 (
      if "!arg!" equ "--help" (
        %print{[% help_line %]}%
        EXIT /B 0
      )
      if "!arg!" equ "-h" (
        %print{[% help_line %]}%
        EXIT /B 0
      )
    )

    @REM Assign run arguments
    if "!arg!" equ "run" (
      set "found_run=true"
      set "found_run_options=true"
      @REM If encountered run, we assume that the rest of the arguments are for run
      set "found_gensln_options=false"
      set "found_buildsln_options=false"
      set "found_install_options=false"
    ) else (
      if "!found_run_options!" equ "true" (
        set "run_options=!run_options! !arg!"
      )
    )

    @Rem Assign installsln arguments
    if "!arg!" equ "installsln" (
      set "found_install=true"
      set "found_install_options=true"
      @REM If encountered installsln, we assume that the rest of the arguments are for installsln
      set "found_gensln_options=false"
      set "found_buildsln_options=false"
    ) else (
        if "!found_install_options!" equ "true" (
        set "install_options=!install_options! !arg!"
      )
    )

    @REM Assign buildsln arguments
    if "!arg!" equ "buildsln" (
      set "found_buildsln=true"
      set "found_buildsln_options=true"
      @REM If encountered buildsln, we assume that the rest of the arguments are for buildsln
      set "found_gensln_options=false"
    ) else (
      if "!found_buildsln_options!" equ "true" (
        set "buildsln_options=!buildsln_options! !arg!"
      )
    )

    @REM Assign gensln arguments
    if "!arg!" equ "gensln" (
      set "found_gensln=true"
      set "found_gensln_options=true"
    ) else (
      if "!found_gensln_options!" equ "true" (
        set "gensln_options=!gensln_options! !arg!"
      )
    )

    @REM Increment argument count
    set "argument_count=!argument_count! + 1"
  )
)

@REM Run gensln
if "!found_gensln!" equ "true" (
  echo -- Running gensln with options:!gensln_options!
  call ./tools/gensln.bat !gensln_options!
  if not "!ERRORLEVEL!" equ "0" (
    echo -- Failed to generate solution
    EXIT /B %ERRORLEVEL%
  )
)

@REM Run buildsln
if "!found_buildsln!" equ "true" (
  echo -- Running buildsln with options:!buildsln_options!
  call ./tools/buildsln.bat !buildsln_options!
  if not "!ERRORLEVEL!" equ "0" (
    echo -- Failed to build solution
    EXIT /B %ERRORLEVEL%
  )
)

@REM Run installsln
if "!found_install!" equ "true" (
  echo -- Running installsln with options:!install_options!
  call ./tools/installsln.bat !install_options!
  if not "!ERRORLEVEL!" equ "0" (
    echo -- Failed to install solution
    EXIT /B %ERRORLEVEL%
  )
)

@REM Run run
if "!found_run!" equ "true" (
  echo -- Running run with options:!run_options!
  call ./tools/run.bat !run_options!
  if not "!ERRORLEVEL!" equ "0" (
    echo -- Failed to run solution
    EXIT /B %ERRORLEVEL%
  )
)
