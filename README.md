[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](COPYING "Read License GPL v3")
[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-brightgreen.svg)](https://github.com/bittor7x0/VDR-M7x0/commits/master "View commits")
[![Build Status](https://img.shields.io/github/workflow/status/bittor7x0/VDR-M7x0/VDR-NG-EM%20Nightly%20Build?label=Build)](https://github.com/bittor7x0/VDR-M7x0/actions/workflows/nightly-build.yml "Download nightly builds")
[![Coverity Scan Status](https://img.shields.io/coverity/scan/22406.svg)](https://scan.coverity.com/projects/bittor7x0-vdr-m7x0 "View Coverity Scan Status")
[![GitHub release](https://img.shields.io/github/release/bittor7x0/VDR-M7x0.svg)](https://github.com/bittor7x0/VDR-M7x0/releases/latest "Download latest release")
[![Forum](https://img.shields.io/static/v1?label=&labelColor=gray&message=Forum&color=blue&style=flat&logo=homeadvisor&logoColor=ffffff)](https://vdr-m7x0.foroactivo.com.es/ "VDR-NG-EM Forum")


## Overview
**VDR-M7X0** is a [VDR](http://www.tvdr.de/software.htm "The Video Disk Recorder") [port](https://en.wikipedia.org/wiki/Porting "Software port") for Siemens Gigaset M740AV, M750T EPG and M750S created in 2006 by German developers from **open7x0**.org (now defunct).

**VDR-NG-EM** (*Video Disk Recorder - Next Generation - Edición Mejorada*) is a VDR-M7X0 [fork](https://en.wikipedia.org/wiki/Fork_(software_development) "Software fork") enhanced since 2007 by [Spanish developers](https://github.com/bittor7x0/VDR-M7x0/wiki/Colaboradores "VDR-NG-EM Contributors") from [vdr-m7x0.foroactivo.com.es](https://vdr-m7x0.foroactivo.com.es/ "VDR-NG-EM Forum") with many fixes, new features and better performance.

In 2021, the **VDR-NG-EM** project was migrated from Assembla to GitHub (Wiki, Tickets and Repositories included the [unstable](https://github.com/bittor7x0/VDR-M7x0-Assembla-Archived) branch for historical reasons).


## Main features
* Backports from VDR 2.4.x with a lot of fixes and improvements.
* Many VDR plugins available (e.g. epgsearch, epgfixer, scheduler, osdteletext, rssreader, vdrmanager, xmltv2vdr, parental control with PIN plugin, etc.). [See complete list](https://github.com/bittor7x0/VDR-M7x0/wiki/Plugins_para_el_VDR-NG-EM "VDR-NG-EM Plugins").
* Permanent TimeShift very configurable.
* Record HD channels using MPEG-TS files (including DVB and Teletext subtitles).
* Edit recordings ultrafast using hard links.
* USB hard disks to 2 times faster.
* Kernel with Real Time scheduler, Low Latency, and many fixes and speed ups.
* Using LTO (Link Time Optimization), Graphite loops optimizations and aggressive compiler flags when possible.
* FTP, SSH, Web, NFS and DLNA servers, a NTP client and a send-only ```sendmail``` emulator.
* Two versions of the firmware are provided: **Pro** is a full version with network support and **Lite** is a smaller version without network packages and minimal Kernel and BusyBox.
* You can compile extra programs (Bash shell, BitTorrent client, VPN, SAMBA server, dvbsnoop, htop, lynx, smartmontools, etc.) enabling them in ```.config``` file.

You can read the [complete list of features in Wiki](https://github.com/bittor7x0/VDR-M7x0/wiki/Mejoras_del_VDR-NG-EM "VDR-NG-EM ChangeLog") (only in Spanish).


## Download Nightly Builds

**GitHub Actions Artifacts ONLY can be downloaded by GitHub logged users.**

Nightly builds are built automatically by GitHub Actions (GitHub's integrated CI/CD tool) every time a new commit is pushed to the _master_ branch.\
The latest nightly build is always up-to-date with the latest version of the code, but is less stable and may contain bugs.\
\
To download the latest nightly build, go to the [GitHub Actions](https://github.com/bittor7x0/VDR-M7x0/actions/workflows/nightly-build.yml) tab, enter the last run of workflow **VDR-NG-EM Nightly Build**, and download the artifacts generated from the run.\
\
You can download WSW firmwares for each hardware (M740, M750T and M750s) and each version (Pro and Lite), a Windows Builder to customize the firmware and the SAMBA binaries.


## Generate firmware on Windows
Download the latest ```VDR-NG-EM-<version>-Firmware-builder.rar``` file from [releases](https://github.com/bittor7x0/VDR-M7x0/releases/latest), extract it, run ```firmware-builder.bat``` and follow the steps on the wizard.\
\
If the firmware isn't generated, check the ```firmware-builder.log``` file to detect the problem and if you can't fix it, [open a new issue](https://github.com/bittor7x0/VDR-M7x0/issues/new?assignees=&labels=bug&template=bug_report.md&title=) attaching the log file.\
\
This builder uses precompiled binaries and Kernel, so it generates the firmware very fast, but it doesn't include latest changes (like nightly builds).


## Compile firmware on Linux

#### 1. Install dependencies

You need to have installed gcc, binutils, patch, flex, make, automake, autoconf, autotools-dev, pkg-config, bison, gettext, genext2fs, bzip2, unzip, subversion and libz-dev.\
If you want network support (Pro version) you also need libtirpc-dev and the java binary (jre-openjdk).

![Arch Linux](https://img.shields.io/badge/-Arch%20Linux-333?style=for-the-badge&labelColor=1793D1&logo=arch%20linux&logoColor=white)
![Manjaro](https://img.shields.io/badge/-Manjaro-333?style=for-the-badge&labelColor=35bf5c&logo=manjaro&logoColor=white)
```
$ sudo pacman -Sy base-devel subversion unzip jre-openjdk
```
\
![Debian](https://img.shields.io/badge/-Debian-333?style=for-the-badge&labelColor=A81D33&logo=debian&logoColor=white)
![Ubuntu](https://img.shields.io/badge/-Ubuntu-333?style=for-the-badge&labelColor=dd4814&logo=ubuntu&logoColor=white)
![Linux Mint](https://img.shields.io/badge/-Linux%20Mint-333?style=for-the-badge&labelColor=87CF3E&logo=linux%20mint&logoColor=white)
```
$ sudo apt-get install -y build-essential pkg-config m4 automake libtirpc-dev libz-dev subversion default-jre
```
\
![Fedora](https://img.shields.io/badge/-Fedora-333?style=for-the-badge&labelColor=0b57a4&logo=fedora&logoColor=white)
![CentOS](https://img.shields.io/badge/-CentOS-333?style=for-the-badge&labelColor=212078&logo=centos&logoColor=white)
![Red Hat Enterprise Linux](https://img.shields.io/badge/-Red%20Hat%20Enterprise%20Linux-333?style=for-the-badge&labelColor=ee0000&logo=red%20hat&logoColor=white)
```
$ sudo dnf install -y make automake gcc gcc-c++ flex subversion bzip2 zlib-devel libtirpc-devel java-latest-openjdk
```
\
![OpenSUSE](https://img.shields.io/badge/-OpenSUSE-333?style=for-the-badge&labelColor=73ba25&logo=opensuse&logoColor=white)
![SUSE Linux Enterprise Server](https://img.shields.io/badge/-SUSE%20Linux%20Enterprise%20Server-333?style=for-the-badge&labelColor=73ba25&logo=opensuse&logoColor=white)
```
$ sudo zypper install -y make automake gcc gcc-c++ flex subversion patch wget bzip2 unzip zlib-devel libtirpc-devel java-11-openjdk
```

#### 2. Get Toolchain source code
```
$ umask 022
$ svn checkout https://github.com/bittor7x0/VDR-M7x0/trunk/toolchain vdr-ng-em
$ cd vdr-ng-em
```

#### 3. Configuration
If you want a version with network support (Pro version), run:
```
$ cp configs/default-config-pro .config
```
or if you **don't** use the network (Lite version), run:
```
$ cp configs/default-config-lite .config
```
Edit the ```.config``` file and change parameters if it's necessary (e.g. ```CONFIG_LANGUAGE```).

#### 4. Compilation
```
$ make
```

Finally, you'll get two firmware files with extension:
* ```.wsw```: Install from the 4x unplug menu. Recommended.
* ```.ofi```: It's an executable file that installs itself. **Only** for advanced users because it's run with ssh/telnet or using the Filebrowser plugin and problems can arise.

If you have any problems or doubts you can send them in English to our [international forum](https://vdr-m7x0.foroactivo.com.es/international-forum-f19/ "VDR-NG-EM International Forum") or [open an issue](https://github.com/bittor7x0/VDR-M7x0/issues/new/choose).


## License
**VDR-NG-EM** is licensed under the [GPL v3 license](COPYING "License: GPL v3").
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](COPYING "Read License GPL v3")\
This license covers all **VDR-NG-EM** specific parts like Makefiles, configuration files, scripts, patches and other files and it doesn't cover the packages **VDR-NG-EM** is based on or **VDR-NG-EM** is able to build, because they may, of course, be used on their own under their own terms.


## Warranties
The **VDR-NG-EM** firmware is distributed without warranty of any kind.\
You assume all responsibility for its use.


## Directory structure
| Directory        | Short description                          |
|:-----------------|:-------------------------------------------|
| dvb-kernel-m7x0  | Some USB DVB drivers for M7x0              |
| toolchain        | Toolchain for build the VDR-NG-EM firmware |
| vdr-m7x0         | VDR-NG-EM source code                      |
| vdr-m7x0-plugins | All plugins that work with VDR-NG-EM       |
| webif            | Web interface server for VDR-NG-EM         |
