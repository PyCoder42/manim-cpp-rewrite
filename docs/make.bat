@ECHO OFF
setlocal

set REPO_ROOT=%~dp0..
set DOCS_TOOL=%REPO_ROOT%\tools\docs\build_docs.sh
set I18N_TOOL=%REPO_ROOT%\tools\docs\check_i18n_catalogs.sh

if "%1"=="" goto help
if "%1"=="help" goto help
if "%1"=="check" goto check
if "%1"=="check-i18n" goto check_i18n
if "%1"=="build" goto build
if "%1"=="html" goto build
if "%1"=="clean" goto clean

echo Unknown target: %1
goto help

:check
bash "%DOCS_TOOL%" --check-only "%REPO_ROOT%"
exit /b %ERRORLEVEL%

:check_i18n
bash "%I18N_TOOL%" "%REPO_ROOT%"
exit /b %ERRORLEVEL%

:build
bash "%DOCS_TOOL%" "%REPO_ROOT%"
exit /b %ERRORLEVEL%

:clean
if exist "%REPO_ROOT%\docs\book\book" rmdir /s /q "%REPO_ROOT%\docs\book\book"
if exist "%REPO_ROOT%\docs\api\build" rmdir /s /q "%REPO_ROOT%\docs\api\build"
exit /b 0

:help
echo Documentation targets:
echo   make.bat check       - verify docs configs exist
echo   make.bat check-i18n  - verify locale catalogs/templates are present
echo   make.bat build       - build mdBook and Doxygen docs
echo   make.bat html        - alias for build
echo   make.bat clean       - remove generated docs output
exit /b 0
