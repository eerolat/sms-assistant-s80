@echo off

echo **********************************
echo SMS Assistant release script
echo (c) Tuomas Eerola 2001-2005
echo
echo Assumptions:
echo 1) SMS Assistant project in \SMSAssist
echo 2) SDK installed in \
echo **********************************

echo *** Deleting ARMI files ***

cd \epoc32\release\armi\urel
del assist.*
cd \epoc32\data\z\system\apps\assist
del assist.*

echo *** Making sure write protects are off ***

cd \smsassist
attrib -r *.* /s

echo *** Building application icons ***

cd \smsassist\datasrc
call build all

echo *** Doing EN build ***

cd \smsassist\inc
del assist.loc
copy assist.en assist.loc

cd \smsassist\group
del abld.bat
call bldmake bldfiles

call abld build armi urel

cd \epoc32\data\z\system\apps\assist
move assist.rsc assist.ren

echo *** Doing FI build ***

cd \smsassist\inc
del assist.loc
copy assist.fi assist.loc

cd \smsassist\group
call abld resource armi urel

cd \epoc32\data\z\system\apps\assist
move assist.rsc assist.rfi

echo *** Doing PL build ***

cd \smsassist\inc
del assist.loc
copy assist.pl assist.loc

cd \smsassist\group
call abld resource armi urel

cd \epoc32\data\z\system\apps\assist
move assist.rsc assist.rpl

echo *** Doing GE build ***

cd \smsassist\inc
del assist.loc
copy assist.ge assist.loc

cd \smsassist\group
call abld resource armi urel

cd \epoc32\data\z\system\apps\assist
move assist.rsc assist.rge

echo *** Doing AM build ***

cd \smsassist\inc
del assist.loc
copy assist.am assist.loc

cd \smsassist\group
call abld resource armi urel

cd \epoc32\data\z\system\apps\assist
move assist.rsc assist.ram

echo *** Doing BE build ***

cd \smsassist\inc
del assist.loc
copy assist.bl assist.loc

cd \smsassist\group
call abld resource armi urel

cd \epoc32\data\z\system\apps\assist
move assist.rsc assist.rbl

echo *** Doing IT build ***

cd \smsassist\inc
del assist.loc
copy assist.it assist.loc

cd \smsassist\group
call abld resource armi urel

cd \epoc32\data\z\system\apps\assist
move assist.rsc assist.rit

echo *** Setting development variant to EN ***

cd \smsassist\inc
del assist.loc
copy assist.en assist.loc

echo *** Creating SIS file ***

cd \smsassist\group
call makesis assist.pkg SMAssist%1Series80.sis

echo *** Ready ***