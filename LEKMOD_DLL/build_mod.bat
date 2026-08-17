@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86
set "VCTargetsPath=C:\Program Files (x86)\MSBuild\Microsoft.Cpp\v4.0"
echo VCTargetsPath=%VCTargetsPath%
if not exist "%VCTargetsPath%\Microsoft.Cpp.Default.props" (
  echo MISSING Default.props
  exit /b 2
)
"C:\Program Files (x86)\MSBuild\12.0\Bin\MSBuild.exe" "C:\Users\EnormousApplePie\Documents\GitHub\Lekmod\LEKMOD_DLL\CvGameCoreDLL_Expansion2\CvGameCoreDLL_Expansion2.vs2013.sln" /p:Configuration=Mod /p:Platform=x86 /m /v:minimal
echo MSBUILD_EXIT=%ERRORLEVEL%
exit /b %ERRORLEVEL%
