rem -*- coding:cp850-dos -*-
@echo off
rem This file is "a bit special" and the above line is for set character encoding
rem in emacs.
rem If you open this file with another editor different to emacs (Linux/Windows)
rem or edit (Windows-DOS), character encoding is not recognized.

rem Increase the environment size to make sure all variables fit.
rem Call ourselves with a special first command line parameter...
if not "%1"=="EnvSizeIncreased" %COMSPEC% /e:8192 /c %0 EnvSizeIncreased %1 %2 %3 %4 %5 %6 %7 %8 %9
rem ... and exit.
if not "%1"=="EnvSizeIncreased" goto :end
rem If we were called with the special command line parameter, we get
rem here: Remove the special command line parameter in case we have
rem other command line paramters.
if "%1"=="EnvSizeIncreased" shift

rem Disable scary warning that DOS paths shouldn't be used.
set CYGWIN=nodosfilewarning

rem Firmware version number
echo.
echo Getting Firmware latest version number
if not exist temp md temp
if exist temp\latest.txt del /f /q temp\latest.txt
bin\wget.exe --no-check-certificate -P temp https://github.com/bittor7x0/VDR-M7x0/raw/firmware-builder-files/latest.txt
rem for /f "tokens=* delims=" %%A in (temp\latest.txt) do set fw_version=%%A
rem if "%fw_version%"=="" set fw_version=0.8
rem Force nightly-build
set fw_version=nightly
del /f /q temp\latest.txt

rem The Wizard starts here.
rem This is a useful set to reduce typing
set wizapp=start /w bin/WizApp.exe

rem Set the bitmap and the return batch file for this wizard,
rem and initialise all ather wa variables in case any old values are
rem left over.
set wabat=%TEMP%\wabat.bat
rem remove any leftovers
set waprefix=
set waeol=
set wainput=
set wafile=
set walistsep=
set walabels=Back;Next;;Cancel
set wasig=VDR-NG-EM %fw_version% Firmware Builder
set wasound=
set waico=bin\logo-tux.ico
set waoutput=
set waoutnum=
rem Initialise the variables this wizard needs.
set i0=1
set i1=1
set undo=
set language=0
set type=m740
set fw_type=ofi
set version=pro
set waoutnum_type=0
set waoutnum_fwtype=0
set waoutnum_fat32=1
set waoutnum_version=0
set waoutnum_dhcp=0
set wabmp=bin\logo-vdr-ng-em.bmp

:page1
set page=:page1
set wafile=
set walabels=Back;Next;;Cancel
set watitle=VDR-NG-EM %fw_version% firmware for Siemens Gigaset M740AV, M750T EPG and M750S
set watext=~~~~~~~You must choose language by default in firmware:
rem The ^ is necessary to escape the ampersand on NT,
rem The ampersand puts in a hotkey.
set wainput= ^&English; E^&spa¤ol; ^&Deutsch; ^&Fran‡ais
set waoutnum=0
%wizapp% NOBACK RB
if errorlevel 2 goto :cancel
if errorlevel 1 goto :page2
rem Retrieve the user's choice
call "%wabat%"
rem and store it
rem English (0), Spanish (1), German (2), French (3)
set language=%waoutnum%

:page2
set page=:page2
if %language%==1 (
	rem Texts in Spanish
	set watitle=Firmware VDR-NG-EM %fw_version% para el Siemens Gigaset M740AV, M750T EPG y M750S
        set watext=Acuerdo de licencia de software~~Este firmware se distribuye bajo la licencia GPL v3.~Cualquier uso, reproducci¢n o redistribuci¢n del software que no cumpla con este Contrato de Licencia est  expresamente prohibido por la ley.~~Pulse el bot¢n [Acepto] para generar el firmware.
	set walabels=Atr s;Acepto;;No acepto;
	set wasig=Generador Firmware VDR-NG-EM %fw_version%
) else (
	rem Texts in English
        set watext=Software license agreement~~This firmware is distributed under GPL v3 license.~Any use, reproduction or redistribution of the software that is not in accordance with this License Agreement is expressly prohibited by law.~~Press [I agree] to generate the firmware.
	set walabels=Back;I agree;;I don't agree;
	set wasig=VDR-NG-EM %fw_version% Firmware Builder
)
rem wafile contains the contents for the license box.
set wafile=licenses\firmware.txt
set wainput=
%wizapp% FT
if errorlevel 2 goto :cancel
if errorlevel 1 goto :page1
rem Don't forget to set wafile to empty; if both wafile and wainput are
rem set, wafile wins.
set wafile=

:page2b
set page=:page2b
if %language%==1 (
	rem Texts in Spanish
	set walabels=Borrar;No borrar;;Cancelar
        set watext=¨Borrar archivos descargados con anterioridad?~~Si ya ha generado el firmware alguna vez en este mismo directorio, puede borrar los archivos descargados con anterioridad para asegurar que genera un firmware actualizado a la £ltima versi¢n.
	rem Here we use a file text box with only a few lines of text, so we use
	rem wainput instead of wafile.
	set wainput=Pulse [Borrar] para borrar los archivos ~de versiones anteriores.~~Pulse [No borrar] para continuar sin borrar ~los archivos de versiones anteriores.~~Si es la primera vez que instalas el firm, ~la respuesta a esta pregunta es irrelevante.
) else (
	rem Texts in English
        set walabels=Delete;Don't delete;;Cancel
        set watext=Delete downloaded files before?~~If the firmware has already generated some time in the same directory, you can delete the files previously downloaded to ensure that generates a firmware updated to latest version.
	rem Here we use a file text box with only a few lines of text, so we use
	rem wainput instead of wafile.
	set wainput=Press [Delete] to delete the files ~from previous versions.~~Press [Don't delete] to continue without deleting ~the files from previous versions.~~If this is the first time you install the firmware, ~the answer to this question is irrelevant.
)
%wizapp% FT
if errorlevel 2 goto :cancel
if errorlevel 1 goto :delete_old_files
goto :page3

:delete_old_files
echo.
echo Delete old files
if exist downloads rd /q /s downloads
if exist temp rd /q /s temp

:page3
set page=:page3
if not exist downloads\%fw_version% md downloads\%fw_version%
if not exist temp\%fw_version% md temp\%fw_version%
copy /Y nightly-files\*.* downloads\%fw_version%

if %language%==1 (
	rem Texts in Spanish
	set walabels=Atr s;Continuar;;Cancelar
	set watext=~~~~~~~Seleccione su versi¢n de hardware:
) else (
	rem Texts in English
	set walabels=Back;Next;;Cancel
	set watext=~~~~~~~Select your hardware version:
)
set watext=%watext%~~~
rem Set the radiobutton options
set wainput=M740AV;M750T EPG;M750S
rem Set the initially checked choice
set waoutnum=%waoutnum_type%
set waoutput=
%wizapp% RB
if errorlevel 2 goto :cancel
if errorlevel 1 goto :page2b
rem Retrieve the user's choice
call "%wabat%"
rem and store it
set waoutnum_type=%waoutnum%
if %waoutnum%==0 (
	set type=m740
	set type2=M740AV
)
if %waoutnum%==1 (
	set type=m740
	set type2=M750T_EPG
)
if %waoutnum%==2 (
	set type=m750s
	set type2=M750S
)

:page4
set page=:page4
if %language%==1 (
	rem Texts in Spanish
	set watext=Seleccione el tipo de firmware:~~WSW: Se instala desde el men£ de los cuatro apagados. Recomendado para nuevos usuarios y/o si vas a actualizar desde un firmware distinto al VDR.~~OFI: Es un archivo ejecutable que se auto-instala al ejecutarlo por telnet o usando el plugin Filebrowser.
) else (
	rem Texts in English
	set watext=Select firmware type:~~WSW: Installs from the 4x unplug menu.~Recommended for new users and/or if you are upgrading from a different firmware to VDR.~~OFI: It is an executable file that installs itself to run with telnet or using the Filebrowser plugin.
)
rem Set the radiobutton options
set wainput=WSW;OFI
rem Set the initially checked choice
set waoutnum=%waoutnum_fwtype%
set waoutput=
%wizapp% RB
if errorlevel 2 goto :cancel
if errorlevel 1 goto :page3
rem Retrieve the user's choice
call "%wabat%"
rem and store it
set waoutnum_fwtype=%waoutnum%
if %waoutnum%==0 set fw_type=wsw
if %waoutnum%==1 set fw_type=ofi

:page5
set page=:page5
if %language%==1 (
	rem Texts in Spanish
	set watext=Seleccione la versi¢n del firmware:~~Pro: Incluye soporte de red con servidor FTP, DLNA, NFS, SSH, telnet, SMTP, cliente NTP para sincronizar la hora con internet, CIFS y NFS, interfaz web, streaming, ...~~Lite: Versi¢n optimizada y dise¤ada para usuarios que no usan la red y quieren un sistema lo m s ligero posible.
) else (
	rem Texts in English
	set watext=Select firmware version:~~Pro: Includes network support with FTP server, DLNA, NFS, SSH, telnet, SMTP, NTP client to synchronize time with Internet, CIFS and NFS, web interface, streaming, ...~~Lite: Version optimized and designed for users who do not use the network and want a system as light as possible.
)
rem Set the radiobutton options
set wainput=Pro;Lite
rem Set the initially checked choice
set waoutnum=%waoutnum_version%
set waoutput=
%wizapp% RB
if errorlevel 2 goto :cancel
if errorlevel 1 goto :page4
rem Retrieve the user's choice
call "%wabat%"
rem and store it
set waoutnum_version=%waoutnum%
if %waoutnum%==0 set version=pro
if %waoutnum%==1 set version=lite

:page6
set page=:page6
if %language%==1 (
	rem Texts in Spanish
        set watext=­Preparado para comenzar la generaci¢n del firmware!~~~~~~Pulse el bot¢n [Continuar] para comenzar la generaci¢n o el bot¢n [Atr s] para modificar alg£n dato.
	rem Here we use a file text box with only a few lines of text, so we use
	rem wainput instead of wafile.
	set wainput=Versi¢n de hardware: %type2%~Tipo de firmware: %fw_type%~Versi¢n: %version%
) else (
	rem Texts in English
        set watext=Ready to start firmware generation!~~~~~~Press [Next] to start generation or [Back] to change any data.
	rem Here we use a file text box with only a few lines of text, so we use
	rem wainput instead of wafile.
	set wainput=Hardware version: %type2%~Firmware type: %fw_type%~Version: %version%
)
%wizapp% FT
if errorlevel 2 goto :cancel
if errorlevel 1 goto :page5
if %language%==1 (
	rem Texts in Spanish
	set watext=~~~~~Descargando...~~Ten paciencia, este proceso puede durar varios minutos
) else (
	rem Texts in English
	set watext=~~~~~Downloading...~~Be patient, this process may take several minutes
)
rem Here we are, after the user pressed the Install button. Let's install!
rem Display a progress bar.
rem This instance of the program runs asynchronously. Subsequent calls of
rem wizapp PB will talk to this instance.
%wizapp% PB OPEN

set fw_url=nothing
set fw_file=nothing
set fw_up_file=nothing
set rfs=rootfs_dir_%type%_%version%

if %type%==m740 (
	set fw_url=https://github.com/bittor7x0/VDR-M7x0/raw/master/toolchain/downloads/fw-siemens-m740-2.4.tar.xz
	set fw_file=Gigaset_M740_AV_V_2.4.tar.xz
	set fw_up_file=2.4_t.wsw
	set count=786432
)

if %type%==m750s (
	set fw_url=https://github.com/bittor7x0/VDR-M7x0/raw/master/toolchain/downloads/fw-siemens-m750s-2.4.tar.xz
	set fw_file=Gigaset_M750_S_V_2.4.tar.xz
	set fw_up_file=2.4_sat.wsw
	set count=1048576
)

rem Here start download.
rem NOTE: Progress bar is a rough amount.
%wizapp% PB UPDATE 5

IF NOT EXIST downloads\%fw_file% (
	echo.
	echo Download orig firmware
	bin\wget.exe --no-check-certificate %fw_url% -O downloads/%fw_file%
)
%wizapp% PB UPDATE 7

IF NOT EXIST downloads\%fw_up_file% (
	echo.
	echo Unpack orig firmware
    bin\7z.exe x -so downloads\%fw_file% | bin\7z.exe x -si -ttar -odownloads
)
%wizapp% PB UPDATE 10

IF NOT EXIST downloads\%fw_version%\o7o_flasher (
	echo.
	echo Download o7o-flasher
	bin\wget.exe --no-check-certificate -P downloads/%fw_version% https://github.com/bittor7x0/VDR-M7x0/raw/firmware-builder-files/%fw_version%/o7o_flasher
)
%wizapp% PB UPDATE 15

IF NOT EXIST downloads\%fw_version%\siemens-linux-kernel-%type%_%version%.img (
	echo.
	echo Download m7x0-kernel-%type%_%version%
	bin\wget.exe --no-check-certificate -P downloads/%fw_version% https://github.com/bittor7x0/VDR-M7x0/raw/firmware-builder-files/%fw_version%/siemens-linux-kernel-%type%_%version%.img
)
%wizapp% PB UPDATE 25

IF NOT EXIST downloads\%fw_version%\file_tab.%rfs%.lst (
	echo.
	echo Download list file
	bin\wget.exe --no-check-certificate -P downloads/%fw_version% https://github.com/bittor7x0/VDR-M7x0/raw/firmware-builder-files/%fw_version%/file_tab.%rfs%.lst
)
%wizapp% PB UPDATE 30

IF NOT EXIST downloads\%fw_version%\%rfs%.tgz (
	echo.
	echo Download %rfs%
	bin\wget.exe --no-check-certificate -P downloads/%fw_version% https://github.com/bittor7x0/VDR-M7x0/raw/firmware-builder-files/%fw_version%/%rfs%.tgz
)
%wizapp% PB UPDATE 40

IF EXIST temp\%fw_version%\%rfs% (
	echo.
	echo Delete temp\%fw_version%\%rfs%
	rd /q /s temp\%fw_version%\%rfs%
)

echo.
echo Unpack m7x0-rootfs
bin\7z.exe x -so downloads/%fw_version%/%rfs%.tgz | bin\7z.exe x -si -ttar -otemp/%fw_version%

%wizapp% PB UPDATE 50

IF NOT EXIST downloads\%fw_version%\jffs2-%type%_%version%.img (
	echo.
	echo Download jffs2-%type%_%version%
	bin\wget.exe --no-check-certificate -P downloads/%fw_version% https://github.com/bittor7x0/VDR-M7x0/raw/firmware-builder-files/%fw_version%/jffs2-%type%_%version%.img
)
%wizapp% PB UPDATE 55

IF NOT EXIST temp\%fw_file%_orig_fw.be.cram (
	echo.
	echo Grab orig cramfs. This may take a while ...
	bin\dd.exe bs=8 if=downloads\%fw_up_file% of=temp\%fw_up_file%_orig_fw.be.cram skip=177 count=%count%
)
%wizapp% PB UPDATE 65

IF NOT EXIST temp\%fw_file%_orig_fw.le.cram (
	echo.
	echo Swapping orig cramfs
	bin\cramfsswap.exe temp\%fw_up_file%_orig_fw.be.cram temp\%fw_up_file%_orig_fw.le.cram
)
%wizapp% PB UPDATE 70

if %type%==m750s (
	echo.
	echo -----------------------------------------------
	echo THX DISCVISION FOR TRUNCATING THE M750S CRAMFS.
	echo THIS BREAKS ALMOST EVERYTHING TO GET THE BLOBBY
	echo SHIT OUT OF YOUR CREEPY FIRMWARE!!!!!
	echo -----------------------------------------------
	bin\dd.exe if=/dev/zero of=temp\dummy_sat.bin bs=1 count=5780
	bin\cat.exe temp\dummy_sat.bin >> temp\%fw_up_file%_orig_fw.le.cram
)

IF NOT EXIST temp\unpacked_%type% (
	echo.
	echo Extract orig rootfs files
	bin\cramfsck.exe -r -x temp/unpacked_%type% temp/%fw_up_file%_orig_fw.le.cram
)
%wizapp% PB UPDATE 80

echo.
echo Copy orig blobby modules with mips-linux-uclibc-objcopy
IF EXIST temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\* del /f /q temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\*

if %type%==m750s (
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' temp\unpacked_%type%\lib\modules\2.4.21-xfs\emma_av.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\emma_av.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' -K TUNER0_TYPE -K TUNER1_TYPE -K header temp\unpacked_%type%\lib\modules\2.4.21-xfs\tuner.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\tuner.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' -K TDA10086_NO_PRINT -K lnb_type temp\unpacked_%type%\lib\modules\2.4.21-xfs\tda10086.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\tda10086.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' -K ac3_delay_bytes -K ac3_delay_ms -K dvr_mem_r -K dvr_mem_w -K enable_psif -K enable_ts_serial0 -K enable_ts_serial1 -K one_tuner -K simulation temp\unpacked_%type%\lib\modules\2.4.21-xfs\demux.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\demux.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' temp\unpacked_%type%\lib\modules\2.4.21-xfs\emma_bios.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\emma_bios.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' -K enable_tuner temp\unpacked_%type%\lib\modules\2.4.21-xfs\pic16f73.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\pic16f73.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' temp\unpacked_%type%\lib\modules\2.4.21-xfs\emma_osd.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\emma_osd.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' -K av_switch_mode -K blank_mode -K is_dv_board -K no_iic -K pip -K vee_ctr6 -K vee_ctr9 -K video_end_threshold temp\unpacked_%type%\lib\modules\2.4.21-xfs\video.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\video.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' -K debug temp\unpacked_%type%\lib\modules\2.4.21-xfs\lirc_dev_s.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\lirc_dev_s.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' -K emma_mem_max -K emma_mem_min temp\unpacked_%type%\lib\modules\2.4.21-xfs\emma_rtos.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\emma_rtos.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' temp\unpacked_%type%\lib\modules\2.4.21-xfs\iic_int.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\iic_int.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' temp\unpacked_%type%\lib\modules\2.4.21-xfs\dvbdev.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\dvbdev.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' -K fb_bpp -K fb_mem_max -K fb_mem_min temp\unpacked_%type%\lib\modules\2.4.21-xfs\emma_fb.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\emma_fb.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' temp\unpacked_%type%\lib\modules\2.4.21-xfs\emma_teletext.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\emma_teletext.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' temp\unpacked_%type%\lib\modules\2.4.21-xfs\pds5002cl.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\pds5002cl.o
) else (
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' -K emma_mem_max -K emma_mem_min temp\unpacked_%type%\lib\modules\2.4.21-xfs\emma_rtos.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\emma_rtos.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' temp\unpacked_%type%\lib\modules\2.4.21-xfs\emma_bios.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\emma_bios.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' temp\unpacked_%type%\lib\modules\2.4.21-xfs\emma_osd.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\emma_osd.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' temp\unpacked_%type%\lib\modules\2.4.21-xfs\emma_av.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\emma_av.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' temp\unpacked_%type%\lib\modules\2.4.21-xfs\emma_teletext.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\emma_teletext.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' -K fb_bpp -K fb_mem_max -K fb_mem_min temp\unpacked_%type%\lib\modules\2.4.21-xfs\emma_fb.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\emma_fb.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' temp\unpacked_%type%\lib\modules\2.4.21-xfs\iic_int.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\iic_int.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' -K debug temp\unpacked_%type%\lib\modules\2.4.21-xfs\lirc_dev_s.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\lirc_dev_s.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' temp\unpacked_%type%\lib\modules\2.4.21-xfs\dvbdev.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\dvbdev.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' -K enable_tuner temp\unpacked_%type%\lib\modules\2.4.21-xfs\pic16f73.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\pic16f73.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' temp\unpacked_%type%\lib\modules\2.4.21-xfs\pds5002cl.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\pds5002cl.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' -K TAIWAN_FLAG -K TAIWAN_FREQ -K TDA10046_NO_PRINT temp\unpacked_%type%\lib\modules\2.4.21-xfs\tda10046.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\tda10046.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' -K TUNER0_TYPE -K TUNER1_TYPE -K header temp\unpacked_%type%\lib\modules\2.4.21-xfs\tuner.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\tuner.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' -K ac3_delay_bytes -K ac3_delay_ms -K dvr_mem_r -K dvr_mem_w -K enable_psif -K enable_ts_serial0 -K enable_ts_serial1 -K one_tuner -K simulation temp\unpacked_%type%\lib\modules\2.4.21-xfs\demux.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\demux.o
	bin\mips-linux-uclibc-objcopy.exe -O elf32-tradbigmips --strip-unneeded --remove-section=.comment --remove-section=.note --remove-section=.pdr --remove-section=.mdebug.abi32 --remove-section=.note.gnu.build-id --remove-section=.gnu.attributes --remove-section=.reginfo -w -K '__param*' -K '__mod*' -K av_switch_mode -K blank_mode -K is_dv_board -K no_iic -K pip -K vee_ctr6 -K vee_ctr9 -K video_end_threshold temp\unpacked_%type%\lib\modules\2.4.21-xfs\video.o temp\%fw_version%\%rfs%\lib\modules\2.4.21-xfs\tainted\video.o
)
%wizapp% PB UPDATE 90

rem Here download is finished and ask about configuration.

if errorlevel 2 goto :cancel

%wizapp% PB CLOSE

rem Basic setup for lite version
if %version%==lite goto setvideo

:page7
set page=:page7
if %language%==1 (
	rem Texts in Spanish
	set watext=Configuraci¢n de red:~~~¨Utiliza DHCP?~~Conteste "No" si desea configurar manualmente la red, o "S¡" si desea que la configuraci¢n se realice autom ticamente mediante el protocolo DHCP.
	rem Set the radiobutton options
	set wainput=S¡;No
) else (
	rem Texts in Spanish
	set watext=Network settings:~~Do you use DHCP?~~Answer "No" if you want to manually configure the network, or "Yes" if you want automatic configuration is done using DHCP protocol.
	rem Set the radiobutton options
	set wainput=Yes;No
)
rem Set the initially checked choice
set waoutnum=%waoutnum_dhcp%
set waoutput=
%wizapp% RB
if errorlevel 2 goto :cancel
if errorlevel 1 goto :page6
rem Retrieve the user's choice
call "%wabat%"
rem and store it
set waoutnum_dhcp=%waoutnum%
if /i %waoutnum%==0 (
	bin\sed.exe -i -e "s/^net=.*/net=DHCP/" temp\%fw_version%\%rfs%\etc\rc.local.conf
	goto skipnet
) else (
	bin\sed.exe -i -e "s/^net=.*/net=inet/" temp\%fw_version%\%rfs%\etc\rc.local.conf
	goto setnet
)

:setnet
:page8
set page=:page8
if %language%==1 (
	rem Texts in Spanish
	set watext=Configuraci¢n de red:~~~~~~~Establezca la direcci¢n IP del giga:
) else (
	rem Texts in English
	set watext=Network settings:~~~~~~~Set IP address of giga:
)
set waoutput=192.168.1.2
set waoutnum=
%wizapp% EB
if errorlevel 2 goto :cancel
if errorlevel 1 goto :page7
rem Retrieve the user's choice
call "%wabat%"
rem and store it
set ip=%waoutput%
bin\sed.exe -i -e "s/^ip=.*/ip=%ip:.=\.%/" temp\%fw_version%\%rfs%\etc\rc.local.conf

:page9
set page=:page9
if %language%==1 (
	rem Texts in Spanish
	set watext=Configuraci¢n de red:~~~~~~~Establezca la m scara de red:
) else (
	rem Texts in English
	set watext=Network settings:~~~~~~~Set netmask:
)
set waoutput=255.255.255.0
set waoutnum=
%wizapp% EB
if errorlevel 2 goto :cancel
if errorlevel 1 goto :page8
rem Retrieve the user's choice
call "%wabat%"
rem and store it
set sub=%waoutput%
bin\sed.exe -i -e "s/^netmask=.*/netmask=%sub:.=\.%/" temp\%fw_version%\%rfs%\etc\rc.local.conf

:skipnet
:page10
set page=:page10
if %language%==1 (
	rem Texts in Spanish
	set watext=Configuraci¢n de red:~~~~~~~Establezca el nombre del equipo:
) else (
	rem Texts in English
	set watext=Network settings:~~~~~~~Set computer name:
)
set waoutput=%type%
set waoutnum=
%wizapp% EB
if errorlevel 2 goto :cancel
if errorlevel 1 goto :page7
rem Retrieve the user's choice
call "%wabat%"
rem and store it
set host=%waoutput%
if %host%.==. (
	bin\sed.exe -i "s/.*/%type%/" temp\%fw_version%\%rfs%\etc\hostname
) else (
	bin\sed.exe -i "s/.*/%host%/" temp\%fw_version%\%rfs%\etc\hostname
)

:setvideo
:page11
set page=:page11
if %language%==1 (
	rem Texts in Spanish
	set watext=Establezca el directorio de grabaciones:~~El directorio de grabaciones debe ser~/media/etiqueta_del_disco_duro~~En caso de no haber definido una etiqueta al disco duro en el proceso de formateo, deje el valor por defecto:~/media/disk-volume-0
) else (
	rem Texts in English
	set watext=Set recording directory:~~The recording directory must be~/media/disk_label~~If you have not established a label to hard disk in formatting process, leave default:~/media/disk-volume-0
)
set waoutput=/media/disk-volume-0
set waoutnum=
%wizapp% EB
if errorlevel 2 goto :cancel
if errorlevel 1 if %version%==lite ( goto :page6 ) else ( goto :page10 )
rem Retrieve the user's choice
call "%wabat%"
rem and store it
set videodir=%waoutput%
if not "%videodir%."=="." bin\sed.exe -i -e "s/^vdr_videodirs=.*/vdr_videodirs=\"%videodir:/=\/%\"/" temp\%fw_version%\%rfs%\etc\rc.local.conf

if %type2%==M740AV (
	echo.
	echo Setting system type in firmware: M740AV
	bin\sed.exe -i -e "s/^export SYSTEMTYPE=.*/export SYSTEMTYPE=m740/" temp\%fw_version%\%rfs%\etc\rc.mini
	bin\sed.exe -i -e "s/^.*/m740/" temp\%fw_version%\%rfs%\etc\systemtype
	set rootfs_max_imgsize=6291456
)
if %type2%==M750T_EPG (
	echo.
   	echo Setting system type in firmware: M750T EPG
	bin\sed.exe -i -e "s/^export SYSTEMTYPE=.*/export SYSTEMTYPE=m750t/" temp\%fw_version%\%rfs%\etc\rc.mini
	bin\sed.exe -i -e "s/^.*/m750t/" temp\%fw_version%\%rfs%\etc\systemtype
	set rootfs_max_imgsize=6291456
)
if %type2%==M750S (
	echo.
	echo Setting system type in firmware: M750S
	bin\sed.exe -i -e "s/^export SYSTEMTYPE=.*/export SYSTEMTYPE=m750s/" temp\%fw_version%\%rfs%\etc\rc.mini
	bin\sed.exe -i -e "s/^.*/m750s/" temp\%fw_version%\%rfs%\etc\systemtype
	set rootfs_max_imgsize=8388608
)

echo.
echo Setting language in firmware
if %language%==0 (
	echo English in setup.conf
	bin\sed.exe -i -e "s/^AudioLanguages.*/AudioLanguages = eng/" temp\%fw_version%\%rfs%\etc\vdr\setup.conf
	bin\sed.exe -i -e "s/^EPGLanguages.*/EPGLanguages = eng/" temp\%fw_version%\%rfs%\etc\vdr\setup.conf
	bin\sed.exe -i -e "s/^OSDLanguage.*/OSDLanguage = 0/" temp\%fw_version%\%rfs%\etc\vdr\setup.conf

	echo English in LC_ALL
	bin\sed.exe -i -e "s/^export LC_ALL.*/export LC_ALL=en_US/" temp\%fw_version%\%rfs%\etc\vdr\runvdr

	if %version%==pro (
		echo English in webif.conf
		bin\sed.exe -i -e "s/^default language.*/default language = 0/" temp\%fw_version%\%rfs%\etc\webif\webif.conf
	)
)

if %language%==1 (
	echo Spanish in setup.conf
	bin\sed.exe -i -e "s/^AudioLanguages.*/AudioLanguages = esl/" temp\%fw_version%\%rfs%\etc\vdr\setup.conf
	bin\sed.exe -i -e "s/^EPGLanguages.*/EPGLanguages = esl/" temp\%fw_version%\%rfs%\etc\vdr\setup.conf
	bin\sed.exe -i -e "s/^OSDLanguage.*/OSDLanguage = 10/" temp\%fw_version%\%rfs%\etc\vdr\setup.conf

	echo Spanish in LC_ALL
	bin\sed.exe -i -e "s/^export LC_ALL.*/export LC_ALL=es_ES/" temp\%fw_version%\%rfs%\etc\vdr\runvdr

	if %version%==pro (
		echo Spanish in webif.conf
		bin\sed.exe -i -e "s/^default language.*/default language = 2/" temp\%fw_version%\%rfs%\etc\webif\webif.conf
	)
)

if %language%==2 (
	echo German in setup.conf
	bin\sed.exe -i -e "s/^AudioLanguages.*/AudioLanguages = deu/" temp\%fw_version%\%rfs%\etc\vdr\setup.conf
	bin\sed.exe -i -e "s/^EPGLanguages.*/EPGLanguages = deu/" temp\%fw_version%\%rfs%\etc\vdr\setup.conf
	bin\sed.exe -i -e "s/^OSDLanguage.*/OSDLanguage = 1/" temp\%fw_version%\%rfs%\etc\vdr\setup.conf

	echo German in LC_ALL
	bin\sed.exe -i -e "s/^export LC_ALL.*/export LC_ALL=de_DE/" temp\%fw_version%\%rfs%\etc\vdr\runvdr

	if %version%==pro (
		echo German in webif.conf
		bin\sed.exe -i -e "s/^default language.*/default language = 1/" temp\%fw_version%\%rfs%\etc\webif\webif.conf
	)
)

if %language%==3 (
	echo French in setup.conf
	bin\sed.exe -i -e "s/^AudioLanguages.*/AudioLanguages = fra/" temp\%fw_version%\%rfs%\etc\vdr\setup.conf
	bin\sed.exe -i -e "s/^EPGLanguages.*/EPGLanguages = fra/" temp\%fw_version%\%rfs%\etc\vdr\setup.conf
	bin\sed.exe -i -e "s/^OSDLanguage.*/OSDLanguage = 6/" temp\%fw_version%\%rfs%\etc\vdr\setup.conf

	echo French in LC_ALL
	bin\sed.exe -i -e "s/^export LC_ALL.*/export LC_ALL=fr_FR/" temp\%fw_version%\%rfs%\etc\vdr\runvdr

	if %version%==pro (
		echo French in webif.conf
		bin\sed.exe -i -e "s/^default language.*/default language = 3/" temp\%fw_version%\%rfs%\etc\webif\webif.conf
	)
)

:page12
set page=:page12
if %language%==1 (
	rem Texts in Spanish
	set watext=¨Quiere usar nombres de grabaciones compatibles con FAT32?~~Conteste "S¡" si el disco duro usado para las grabaciones est  formateado en FAT32 o quiere ver los directorios de las grabaciones por SAMBA. Aunque aqu¡ conteste "No" podr  seguir usando discos duros FAT32 porque solamente se refiere a las grabaciones hechas con VDR.
	rem Set the radiobutton options
	set wainput=S¡;No
) else (
	rem Texts in Spanish
	set watext=Do you want use recordings names compatible with FAT32?~~Answer "Yes" if the hard drive used for recordings is formatted in FAT32 or want to see the directories of these recordings by SAMBA. Although here you answer "No" can still use FAT32 hard drives because it is referred only to recordings made with VDR.
	rem Set the radiobutton options
	set wainput=Yes;No
)

rem Set the initially checked choice
set waoutnum=%waoutnum_fat32%
set waoutput=
%wizapp% RB
if errorlevel 2 goto :cancel
if errorlevel 1 goto :page11
rem Retrieve the user's choice
call "%wabat%"
rem and store it
set waoutnum_fat32=%waoutnum%
if %waoutnum%==0 bin\sed.exe -i -e "s/^vdr_options=\"\(.*\)\"/vdr_options=\"\1 --vfat\"/" temp\%fw_version%\%rfs%\etc\rc.local.conf

:page13
set page=:page13
if %language%==1 (
	rem Texts in Spanish
	set watext=Configuraci¢n b sica completada.~~Ahora, puede modificar archivos de configuraci¢n en:~temp\%fw_version%\%rfs%\etc~~Utilice un editor de texto compatible con Linux.~­El Bloc de notas no sirve!~~Tambi‚n puede a¤adir plugins en:~temp\%fw_version%\%rfs%\usr\lib\vdr~~Pulse [Continuar] para finalizar.
) else (
	rem Texts in English
	set watext=Basic configuration completed.~~Now, you can modify configuration files in:~temp\%fw_version%\%rfs%\etc~~Use a text editor compatible with Linux.~Notepad does not work!~~You can also add plugins in:~temp\%fw_version%\%rfs%\usr\lib\vdr~~Press [Next] to finish.
)

%wizapp% TB
if errorlevel 2 goto :cancel
if errorlevel 1 goto :page12

if %language%==1 (
	rem Texts in Spanish
	set watext=~~~~~Generando el firmware ...~~Espere unos segundos m s para terminar el proceso.
) else (
	rem Texts in English
	set watext=~~~~~Generating firmware ...~~Wait a few seconds to complete the process.
)

%wizapp% PB OPEN

%wizapp% PB UPDATE 45
echo.
echo Generating SquashFS image
if exist temp\VDR-NG-EM-%fw_version%-%version%-firmware.be.img del /f /q temp\VDR-NG-EM-%fw_version%-%version%-firmware.be.img
bin\mksquashfs.exe temp\%fw_version%\%rfs% temp\VDR-NG-EM-%fw_version%-%version%-firmware.be.img -all-root -be -noappend -noI -no-fragments -d downloads\%fw_version%\file_tab.%rfs%.lst

%wizapp% PB UPDATE 80
echo.
echo Checking max RootFS size (%rootfs_max_imgsize%)
set rootfs_size_error=0
for %%A in (temp\VDR-NG-EM-%fw_version%-%version%-firmware.be.img) do if %%~zA GTR %rootfs_max_imgsize% set rootfs_size_error=1
if %rootfs_size_error%==1 (
	echo.
	echo Error: SquashFS image too big!
	%wizapp% PB CLOSE
	set watext=Error: SquashFS image too big!
	%wizapp% MB INFORMATION
	goto :exit
)

%wizapp% PB UPDATE 85
echo.
echo Build final vdr firmware as %fw_type% for %type%
if %fw_type%==ofi bin\gen_ofi.exe -o VDR-NG-EM-%fw_version%-%version%-firmware.ofi -f downloads\%fw_version%\o7o_flasher --%type% -r temp\VDR-NG-EM-%fw_version%-%version%-firmware.be.img -k downloads\%fw_version%\siemens-linux-kernel-%type%_%version%.img -s bin\sha1sum.exe -j downloads\%fw_version%\jffs2-%type%_%version%.img

%wizapp% PB UPDATE 95
if %fw_type%==wsw bin\gen_wsw.exe -o VDR-NG-EM-%fw_version%-%version%-firmware.wsw -n VDR-NG-EM-%fw_version%-%version% --%type% -r temp\VDR-NG-EM-%fw_version%-%version%-firmware.be.img -k downloads\%fw_version%\siemens-linux-kernel-%type%_%version%.img -1 bin\rsaprivate1.key -2 bin\rsaprivate2.key -m bin\md5sum.exe -c bin\rsaencode.exe -d downloads\%fw_version%\jffs2-%type%_%version%.img

echo.
echo Done
%wizapp% PB CLOSE

if %language%==1 (
	rem Texts in Spanish
	if %fw_type%==wsw set watext=­Generaci¢n completada!~~Ya puede instalar el firm "VDR-NG-EM-%fw_version%-%version%-firmware.wsw" en su equipo mediante el men£ de los 4 apagados.
	if %fw_type%==ofi set watext=­Generaci¢n completada!~~Ya puede instalar el firm "VDR-NG-EM-%fw_version%-%version%-firmware.ofi" en su equipo mediante el plugin Filebrowser o telnet.
) else (
	rem Texts in English
	if %fw_type%==wsw set watext=Generation completed!~~You can install the firmware "VDR-NG-EM-%fw_version%-%version%-firmware.wsw" on your M7x0 with the 4x unplug menu.
	if %fw_type%==ofi set watext=Generation completed!~~You can install the firmware "VDR-NG-EM-%fw_version%-%version%-firmware.ofi" on your M7x0 with Filebrowser plugin or telnet.
)
%wizapp% MB INFORMATION

goto :exit

:cancel
if %language%==1 (
	rem Texts in Spanish
        set watext=¨Cancelar la generaci¢n del firmware VDR-NG-EM %fw_version%?
) else (
	rem Texts in English
	set watext=Cancel generation of VDR-NG-EM %fw_version% firmware?
)

%wizapp% MB QUES
if errorlevel 2 goto %page%
rem The user wants to cancel installation,
rem so we must uninstall what's been done. Above, we set
rem the undo variable to keep track of what to undo. If undo is not set,
rem just exit
if "%undo%"=="" goto :exit
rem if undo is set, there is a progress bar on the screen. Close it.
%wizapp% PB CLOSE
rem And undo the installation steps.
goto %undo%
rem Here we undo the installation in the reverse order. Any checkpoint
rem we didn't get to will not be undone.

:exit
set wizapp=
if exist "%wabat%" del /f /q "%wabat%"
if exist %TEMP%\wafile del /f /q %TEMP%\wafile
set undo=
set page=
set i0=
set i1=

set waprefix=
set watitle=
set watext=
set wainput=
set waeol=
set walistsep=
set wafile=
set wabmp=
set walabels=
set wasig=
set wasound=
set waico=
set wabat=
set waoutput=
set waoutnum=

set language=
set type=
set fw_type=
set version=
set waoutnum_type=
set waoutnum_fwtype=
set waoutnum_fat32=
set waoutnum_version=
set waoutnum_dhcp=
set wabmp=
:end
