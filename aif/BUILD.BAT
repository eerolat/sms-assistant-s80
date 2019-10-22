@echo off
if "%1"=="" goto help

goto %1

:mbm
bmconv smasmbm.lst
goto end

:aif
call aiftool smasemb smassist.mbm
call aiftool smasno smassist.mbm

:saveaif
copy smasno.aif ..\group\smassist.aif
copy smasno.aif \nokiacpp\epoc32\release\wins\udeb\z\system\apps\smassist\smassist.aif
copy smasno.aif \nokiacpp\epoc32\release\armi\urel\smassist.aif
goto end

:all
call build mbm
call build aif
call build saveaif
goto end

:help
echo Use this to build and save parts of the AIF for SMS Assistant
echo build mbm	build the .mbm file
echo build aif	invoke AIFTOOL to build the AIFs
echo build saveaif	save the AIFs with their projects
echo build all	do all these steps in sequence

:end