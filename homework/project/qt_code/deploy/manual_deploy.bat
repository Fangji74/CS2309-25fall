@echo off
setlocal enabledelayedexpansion

REM Set your Qt installation path
set QT_PATH=C:\Qt\5.15.2\mingw81_64

REM Set your exe path
set EXE_SOURCE=D:\qt_project\project_node_draw\build\Desktop_Qt_5_15_2_MinGW_64_bit-Release\release\project_node_draw.exe

REM Set deployment path
set DEPLOY_PATH=D:\qt_project\project_node_draw\deploy_final

echo.
echo Cleaning old deployment...
if exist "%DEPLOY_PATH%" (
    echo   Deleting old directory...
    rmdir /s /q "%DEPLOY_PATH%"
)
mkdir "%DEPLOY_PATH%"
cd /d "%DEPLOY_PATH%"

echo Copying executable...
copy "%EXE_SOURCE%" "%DEPLOY_PATH%\project_node_draw.exe"
if errorlevel 1 (
    echo   ERROR: Cannot find executable!
    echo   Please check path:%EXE_SOURCE%
    pause
    exit /b 1
)

echo Copying Qt core DLLs...
for %%d in (Qt5Core Qt5Gui Qt5Widgets) do (
    if exist "%QT_PATH%\bin\%%d.dll" (
        copy "%QT_PATH%\bin\%%d.dll" .
        echo   Copied:%%d.dll
    ) else (
        echo   WARNING: Cannot find %%d.dll
    )
)

echo Copying MinGW runtime DLLs...
for %%d in (libgcc_s_seh-1 libstdc++-6 libwinpthread-1) do (
    if exist "%QT_PATH%\bin\%%d.dll" (
        copy "%QT_PATH%\bin\%%d.dll" .
        echo   Copied: %%d.dll
    )
)

echo Copying platform plugin...
mkdir platforms 2>nul
if exist "%QT_PATH%\plugins\platforms\qwindows.dll" (
    copy "%QT_PATH%\plugins\platforms\qwindows.dll" platforms\
    echo   Copied: platforms\qwindows.dll
) else (
    echo   ERROR: Cannot find platform plugin!
    echo   Please check path: %QT_PATH%\plugins\platforms\
)

echo Copying image format plugins...
mkdir imageformats 2>nul
for %%f in (qjpeg qpng qgif qico qbmp) do (
    if exist "%QT_PATH%\plugins\imageformats\%%f.dll" (
        copy "%QT_PATH%\plugins\imageformats\%%f.dll" imageformats\
        echo   Copied: imageformats\%%f.dll
    )
)

echo Copying style plugins...
mkdir styles 2>nul
if exist "%QT_PATH%\plugins\styles\qwindowsvistastyle.dll" (
    copy "%QT_PATH%\plugins\styles\qwindowsvistase.dll" styles\ 2>nul
    copy "%QT_PATH%\plugins\styles\qwindowsvistastyle.dll" styles\
    echo  Copied: styles\qwindowsvistastyle.dll
)

echo.
echo ===============================================
echo  SUCCESS: Deployment completed!
echo  Directory: %DEPLOY_PATH%
echo ===============================================
echo.
dir "%DEPLOY_PATH%"
echo Testing the application...
echo.
start project_node_draw.exe
pause