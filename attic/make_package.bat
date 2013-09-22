@echo off
rem *****************************************************
rem *      Packaging batch for CRRCsim-Win32            *
rem *      ---------------------------------            *
rem *                                                   *
rem * Creates a subdirectory "crrcsim-win32" with all   *
rem * files needed for a binary package.                *
rem *****************************************************

echo.
echo You are about to create a CRRCsim package directory.
echo The current contents of crrcsim-win32 will be deleted.
echo Press CNTR + C to stop now or any other key to continue.
echo.
pause

rem Create subdirectory hierarchy
if EXIST crrcsim-win32 del /S /Q crrcsim-win32
mkdir crrcsim-win32
mkdir crrcsim-win32\models
mkdir crrcsim-win32\textures
mkdir crrcsim-win32\sounds
mkdir crrcsim-win32\scenery
mkdir crrcsim-win32\Documentation
mkdir crrcsim-win32\Documentation\input_method
mkdir crrcsim-win32\Documentation\input_method\PARALLEL_1_TO_3
mkdir crrcsim-win32\Documentation\input_method\SERIAL2

rem Copy contents of root directory
echo Copying files from root dir...
copy CRRCsim-win32.exe crrcsim-win32
copy README crrcsim-win32
copy LICENSE crrcsim-win32

rem Copy data files
echo Copying data files...
copy models\*.xml crrcsim-win32\models
copy textures crrcsim-win32\textures
copy sounds crrcsim-win32\sounds
copy scenery crrcsim-win32\scenery

rem Copy documentation
echo Copying documentation...
copy Documentation\*.* crrcsim-win32\Documentation
copy Documentation\input_method\PARALLEL_1_TO_3\*.* crrcsim-win32\Documentation\input_method\PARALLEL_1_TO_3
copy Documentation\input_method\SERIAL2\*.* crrcsim-win32\Documentation\input_method\SERIAL2
