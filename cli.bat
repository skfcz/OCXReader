@echo off
setlocal EnableDelayedExpansion
set ERRORLEVEL=0

@REM https://stackoverflow.com/a/28112521
@REM Parse arguments that start with - and creates a series of variables that
@REM start with "option" and the names and values of all options given.
set "gensln_options="
set "found_gensln=false"
set "found_gensln_options=false"

set "buildsln_options="
set "found_buildsln=false"
set "found_buildsln_options=false"

set "run_options="
set "found_run=false"
set "found_run_options=false"

for %%a in (%*) do (
  if not defined option (
    set arg=%%a

    @REM Assign run arguments
    if "!arg!" equ "run" (
      set "found_run=true"
      set "found_run_options=true"
      @REM If encountered run, we assume that the rest of the arguments are for run
      set "found_gensln_options=false"
      set "found_buildsln_options=false"
    ) else (
      if "!found_run_options!" equ "true" (
        set "run_options=!run_options! %%a"
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
        set "buildsln_options=!buildsln_options! %%a"
      )
    )

    @REM Assign gensln arguments
    if "!arg!" equ "gensln" (
      set "found_gensln=true"
      set "found_gensln_options=true"
    ) else (
      if "!found_gensln_options!" equ "true" (
        set "gensln_options=!gensln_options! %%a"
      )
    )
  )
)

@REM Run gensln
if "!found_gensln!" equ "true" (
  echo -- Running gensln with options: !gensln_options!
  call ./tools/gensln.bat !gensln_options!
  if not "!ERRORLEVEL!" equ "0" (
    echo -- Failed to generate solution
    EXIT /B %ERRORLEVEL%
  )
)

@REM Run buildsln
if "!found_buildsln!" equ "true" (
  echo -- Running buildsln with options: !buildsln_options!
  call ./tools/buildsln.bat !buildsln_options!
  if not "!ERRORLEVEL!" equ "0" (
    echo -- Failed to build solution
    EXIT /B %ERRORLEVEL%
  )
)

@REM Run run
if "!found_run!" equ "true" (
  echo -- Running run with options: !run_options!
  call ./tools/run.bat !run_options!
  if not "!ERRORLEVEL!" equ "0" (
    echo -- Failed to run solution
    EXIT /B %ERRORLEVEL%
  )
)
