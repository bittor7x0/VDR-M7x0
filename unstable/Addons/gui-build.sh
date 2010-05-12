#!/bin/bash
#inicio de script
#Script con licencia de distribución Dominio Público
#http://creativecommons.org/publicdomain/
#Este software se entrega sin garantía de ningún tipo
#Autor: naldoco (Reynaldo Cordero)
#Autor: zjuanma
#Autor: pjllaneras

#Defaults
SCRIPT_VERSION="1.0"
COMP_OPTS=""

BASE=`pwd`
COMP_LOG="$BASE/build.log"
TEMP=/tmp/answer$$
version_firm="lite"
model_m7xx="m740"
locale_firm="es"
install_mode="basic"
last_svn_firm=0
CONF_NET_FILE="$BASE/.config_net"
CONF_LOCAL_FILE="$BASE/.config_local"
RC_CONF="$BASE/VDR-NG-EM/VDR-NG-FW/buildin/base-$version_firm/common/etc/rc.conf"
RC_LOCAL_CONF="$BASE/VDR-NG-EM/VDR-NG-FW/buildin/base-$version_firm/common/etc/rc.local.conf"

TEXTDOMAIN="gui-build"
TEXTDOMAINDIR="./locale/"

export CC=/usr/bin/gcc-3.4
#end defaults



#---------------------------------------------------------------------------------------------------------------
# utility function
on_off() {
    if [ "$1" = "$2" ] ; then echo TRUE ; else echo FALSE ; fi
}

debug() {
    echo $1 | tee -a $COMP_LOG
}

info() {
    echo $1 | tee -a $COMP_LOG
}

check_zenity() {
    zenity 2>&1 > /dev/null
    if [ "$?" = "127" ] ; then
        echo $"You have to install 'zenity' package"
        exit 1
    fi
}

get_svn_firm() {
    return `svn log -l 1 -q VDR-NG-FW | grep -v '^\-' | cut -d'|' -f 1`
}

system_type() {
   return `uname -a | grep $1 | wc -l`
}

ubuntu_install() {
    system_type 'Ubuntu'
    if [ "$?" = "0" ] ; then
        info $"This system is not Ubuntu, the package installation must be done manually"
        zenity --info --text $"This system is not Ubuntu, the package installation must be done manually"
        return;
    fi

    info $"Verifying installed packages..."

    PACKAGES='gcc-3.4 build-essential subversion make automake bison binutils genext2fs texinfo libmpfr-dev patch zlib1g-dev libncurses5-dev gettext flex libgmp3-dev'
    for PACKAGE in $PACKAGES; do
        if [ `dpkg --get-selections $PACKAGE | grep -w install | wc -l` = "0" ] ; then
            info $"Installing '$PACKAGE'"
            gksudo "apt-get install -q -y $PACKAGE"
        else
            info $"The package '$PACKAGE' is already installed"
        fi;
    done

    DEBIANPOOL='http://ftp.fr.debian.org/debian/pool/main/g/gcc-3.4/'
    PACKAGEVERSION='_3.4.6-9'
    PACKAGES='gcc-3.4-base cpp-3.4 gcc-3.4'
    system_type 'x86_64'
    if [ "$?" = "0" ] ; then
        PCTYPE='_i386'
    else
        PCTYPE='_amd64'
    fi
    for PACKAGE in $PACKAGES; do
        if [ `dpkg --get-selections $PACKAGE | grep -w install | wc -l` = "0" ] ; then
            if [ ! -e $PACKAGE$PACKAGEVERSION$PCTYPE.deb ] ; then
                info $"Downloading $PACKAGE$PACKAGEVERSION$PCTYPE.deb"
                wget -q $DEBIANPOOL$PACKAGE$PACKAGEVERSION$PCTYPE.deb
            fi
            info $"Installing '$PACKAGE'"
            gksudo "dpkg -i $PACKAGE$PACKAGEVERSION$PCTYPE.deb"
        else
            info $"The package '$PACKAGE' is already installed"
        fi;
    done
}

#---------------------------------------------------------------------------------------------------------------
select_locale() {
    ans=$(zenity --list --title $"LOCALE" --text $"Select locale:" --radiolist \
        --column=""\
        --column=$"Locale" `on_off $locale_firm 'es'` "es" `on_off $locale_firm 'en'` "en" \
        `on_off $locale_firm 'de'` "de" `on_off $locale_firm 'fr'` "fr"
    );

    if [ -z "$ans" ] ; then return ; fi

    case $ans in
        "es") locale_firm='es';;
        "en") locale_firm='en';;
        "de") locale_firm='de';;
        "fr") locale_firm='fr';;
    esac
}
#---------------------------------------------------------------------------------------------------------------
select_install_mode() {
    ans=$(zenity --list --title $"Install Mode" --text $"Select option:" --radiolist --column="" --column=$"Mode" `on_off $install_mode 'basic'` "BASIC" `on_off $install_mode 'expert'` "EXPERT" );

    if [ -z "$ans" ] ; then return ; fi

    case $ans in
        "BASIC") install_mode='basic';;
        "EXPERT") install_mode='expert';;
    esac
}
#---------------------------------------------------------------------------------------------------------------
select_version() {
    ans=$(zenity --list --title $"Version" --text $"Select Version:" --radiolist --column="" --column=$"Version" `on_off $version_firm 'lite'` "LITE" `on_off $version_firm 'pro'` "PRO" );

    if [ -z "$ans" ] ; then return ; fi

    case $ans in
        "LITE") version_firm='lite';;
        "PRO") version_firm='pro';;
    esac
}
#---------------------------------------------------------------------------------------------------------------
select_model() {
    ans=$(zenity --list --title $"Siemens m7xx model" --text $"Select Siemens m7xx model" --radiolist --column="" --column=$"Model" `on_off $model_m7xx 'm740'` "M740" `on_off $model_m7xx 'm750t'` "M750T" );

    if [ -z "$ans" ] ; then return ; fi

    case $ans in
        "M740") model_m7xx='m740';;
        "M750T") model_m7xx='m750t';;
    esac
}

#---------------------------------------------------------------------------------------------------------------
inet_menu() {
# network config
    ans=$(zenity --list --title $"IP configuration" --text $"Select option" --radiolist --column="" --column=$"Model" `on_off $net 'dhcp'` "DHCP" `on_off $net 'inet'` $"Static IP" );

    if [ -z "$ans" ] ; then return ; fi

    case $ans in
        "DHCP") net=dhcp;;
        $"Static IP") net=inet;
            ip=$(zenity --title $"Network configuration" --entry --text $"IP address" --entry-text "$ip")
            netmask=$(zenity --title $"Network configuration" --entry --text $"Netmask" --entry-text "$netmask")
            gateway=$(zenity --title $"Network configuration" --entry --text $"Gateway" --entry-text "$gateway")
            if [ -z "$gateway" ] ; then gateway="NO" ; fi ;;
    esac
}

#---------------------------------------------------------------------------------------------------------------
netdate_menu(){
#netdate=NO
#netdate_server=europe.pool.ntp.org

    ans=$(zenity --list --title $"Netdate server" --text $"Select option" --checklist --column="" --column="" `on_off $netdate "YES"` $"Activate Netdate" );

    if [ -z "$ans" ] ; then return ; fi

    case $ans in
        $"Activate Netdate")
            netdate='YES';
            netdate_server=$(zenity --title $"Netdate configuration" --entry --text $"Netdate server" --entry-text "$netdate_server");;
        *) netdate='NO';;
    esac
}

#---------------------------------------------------------------------------------------------------------------
net_menu() {
    while true
    do
        ans=$(zenity --list --title=$"Network configuration" --column "" --column ""\
            1 $"Assign IP" \
            2 $"Activate network directories" \
            3 $"Activate Webif server" \
            4 $"Activate Inetd server (telnet, ftp, ...)" \
            5 $"Activate Netdate server"
            )

        if [ -z "$ans" ] ; then return ; fi

        case $ans in
            1) inet_menu;;
            2) do_net_mount=$(zenity --list --title $"Network configuration" --text $"Mount network directories" --radiolist --column="" --column=$"Option" `on_off $do_net_mount 'YES'` "YES" `on_off $do_net_mount 'NO'` "NO" );
               fstab_entry=$(zenity --title $"Network configuration" --entry --text $"Mounting Parameters" --entry-text "$fstab_entry");;
            3) webif_start=$(zenity --list --title $"Network configuration" --text $"Activate Webif server" --radiolist --column="" --column=$"Option" `on_off $webif_start 'YES'` "YES" `on_off $webif_start 'NO'` "NO" );;
            4) inetd=$(zenity --list --title $"Network configuration" --text $"Activate Inetd server" --radiolist --column="" --column=$"Option" `on_off $inetd 'YES'` "YES" `on_off $inetd 'NO'` "NO" );;
            5) netdate_menu;;
        esac
    done
}
#---------------------------------------------------------------------------------------------------------------
set_config() {
    set_general
    set_net
}
#---------------------------------------------------------------------------------------------------------------
set_net() {
    if [ "$version_firm" = "lite" ]; then
        return
    fi

    cat /dev/null > $CONF_NET_FILE
    echo "net=$net" > $CONF_NET_FILE
    echo "ip=$ip" >> $CONF_NET_FILE
    echo "netmask=$netmask" >> $CONF_NET_FILE
    echo "gateway=$gateway" >> $CONF_NET_FILE
    echo "netdate=$netdate" >> $CONF_NET_FILE
    echo "netdate_server=$netdate_server" >> $CONF_NET_FILE

    echo "do_net_mount=$do_net_mount" >> $CONF_LOCAL_FILE;
    echo "fstab_entry='$fstab_entry'" >> $CONF_LOCAL_FILE;
    echo "webif_start=$webif_start" >> $CONF_LOCAL_FILE;
    echo "inetd=$inetd" >> $CONF_LOCAL_FILE;

    if [ -f $RC_LOCAL_CONF ]; then
        sed -i "s/net=.*/net=$net/g" $RC_LOCAL_CONF
        sed -i "s/ip=.*/ip=$ip/g" $RC_LOCAL_CONF
        sed -i "s/netmask=.*/netmask=$netmask/g" $RC_LOCAL_CONF
        sed -i "s/gateway=.*/gateway=$gateway/g" $RC_LOCAL_CONF
        sed -i "s/netdate=.*/netdate=$netdate/g" $RC_LOCAL_CONF
        sed -i "s/netdate_server=.*/netdate_server=$netdate_server/g" $RC_LOCAL_CONF
        sed -i "s/do_net_mount=.*/do_net_mount=$do_net_mount/g" $RC_LOCAL_CONF
        sed -i "s/webif_start=.*/webif_start=$webif_start/g" $RC_LOCAL_CONF
        sed -i "s/inetd=.*/inetd=$inetd/g" $RC_LOCAL_CONF
    fi
    ##1 linea de fstab
    FSTAB_CONF="$BASE/VDR-NG-EM/VDR-NG-FW/buildin/base-$version_firm/common/etc/fstab"
    if [ -f $FSTAB_CONF ]; then
        echo "$fstab_entry" >> $FSTAB_CONF
    fi
    return
}

#---------------------------------------------------------------------------------------------------------------
set_general() {
    cat /dev/null > $CONF_LOCAL_FILE;
    echo "last_script_version=$last_script_version" >> $CONF_LOCAL_FILE;
    echo "last_svn_firm=$last_svn_firm" >> $CONF_LOCAL_FILE;
    echo "install_mode=$install_mode" >> $CONF_LOCAL_FILE;
    echo "model_m7xx=$model_m7xx" >> $CONF_LOCAL_FILE;
    echo "version_firm=$version_firm" >> $CONF_LOCAL_FILE;
    echo "locale_firm='$locale_firm'" >> $CONF_LOCAL_FILE;

    echo "vdr_videodirs='$vdr_videodirs'" >> $CONF_LOCAL_FILE;
    echo "vdr_options='$vdr_options'" >> $CONF_LOCAL_FILE;

    if [ -f $RC_LOCAL_CONF ]; then
        sed -i "s:vdr_videodirs=.*:vdr_videodirs=\"$vdr_videodirs\":g" $RC_LOCAL_CONF
        sed -i "s/vdr_options=.*/vdr_options=\"$vdr_options\"/g" $RC_LOCAL_CONF
    fi


###TODO
##rescue_keys=NO

    return
}
#---------------------------------------------------------------------------------------------------------------
general_menu() {
    while true
    do
        ans=$(zenity --list --title=$"General configuration" --column "" --column ""\
            1 $"Recordings Directory" \
            2 $"VDR options" \
         )
        if [ -z "$ans" ] ; then return ; fi

        case $ans in
            1) vdr_videodirs=$(zenity --title $"General configuration" --entry --text $"Recordings Directory" --entry-text "$vdr_videodirs");;
            2) vdr_options=$(zenity --title $"General configuration" --entry --text $"VDR options" --entry-text "$vdr_options");;
        esac
    done
}

#---------------------------------------------------------------------------------------------------------------
load_general(){

#Configurar parametros del rc.local.conf
    if [ -f $CONF_LOCAL_FILE ]; then
        source $CONF_LOCAL_FILE > /dev/null
    fi
    RC_CONF="$BASE/VDR-NG-EM/VDR-NG-FW/buildin/base-$version_firm/common/etc/rc.conf"
    RC_LOCAL_CONF="$BASE/VDR-NG-EM/VDR-NG-FW/buildin/base-$version_firm/common/etc/rc.local.conf"

    if [ -f $RC_LOCAL_CONF ]; then
        source $RC_LOCAL_CONF > /dev/null
    fi
    if [ -f $CONF_LOCAL_FILE ]; then
        source $CONF_LOCAL_FILE > /dev/null
    fi

#Configurar parametros de la red
    if [ "$version_firm" = "pro" ]; then
        if [ -f $RC_CONF ]; then
            source $RC_CONF > /dev/null
        fi
        if [ -f $CONF_NET_FILE ]; then
            source $CONF_NET_FILE > /dev/null
        fi
    fi
}

#---------------------------------------------------------------------------------------------------------------
update_vdr() {
    svn update $BASE/VDR-NG-EM/Addons 2>&1 | tee -a $COMP_LOG | zenity --progress --auto-close --auto-kill --pulsate --text $"Updating VDR source code"

    cd $BASE/VDR-NG-EM/VDR-NG-FW ;
    $LINUX32BIN make clean-vdr 2>&1 | tee -a $COMP_LOG;
    $LINUX32BIN make distclean-vdr 2>&1 | tee -a $COMP_LOG;
    $LINUX32BIN make 2>&1 | tee -a $COMP_LOG;
}


#---------------------------------------------------------------------------------------------------------------
update_plugins() {
    svn update $BASE/VDR-NG-EM/Addons 2>&1 | tee -a $COMP_LOG | zenity --progress --auto-close --auto-kill --pulsate --text $"Updating plugins source code"

    cd $BASE/VDR-NG-EM/VDR-NG-FW ;
    $LINUX32BIN make clean-vdr-plugins 2>&1 | tee -a $COMP_LOG;
    $LINUX32BIN make distclean-vdr-plugins 2>&1 | tee -a $COMP_LOG;
    $LINUX32BIN make 2>&1 | tee -a $COMP_LOG;
}

#---------------------------------------------------------------------------------------------------------------
update_webif() {
    svn update $BASE/VDR-NG-EM/Addons 2>&1 | tee -a $COMP_LOG | zenity --progress --auto-close --auto-kill --pulsate --text $"Updating webif source code"

    cd $BASE/VDR-NG-EM/VDR-NG-FW;

    $LINUX32BIN make clean-webif 2>&1 | tee -a $COMP_LOG;
    $LINUX32BIN make distclean-webif 2>&1 | tee -a $COMP_LOG;
    $LINUX32BIN make 2>&1 | tee -a $COMP_LOG;
}
#---------------------------------------------------------------------------------------------------------------

backup_downloads() {
    #Gestionando downloads
    if [ ! -e $BASE/downloads ]; then
        mkdir -p $BASE/downloads
    fi

    if [ -d "$BASE/VDR-NG-EM/VDR-NG-FW/downloads" ]; then
        ( 
        cp -u -v $BASE/VDR-NG-EM/VDR-NG-FW/downloads/* $BASE/downloads/
        rm -rf $BASE/VDR-NG-EM/VDR-NG-FW/downloads
        ) | tee -a >(zenity --progress --text $"'Downloads' backup" --auto-close --auto-kill --pulsate --auto-close)
    fi

    if [ -e $BASE/downloads ]; then
        #recuperando ficheros downloads
        mkdir -p $BASE/VDR-NG-EM/VDR-NG-FW
        rm -rf $BASE/VDR-NG-EM/VDR-NG-FW/downloads
        ln -s $BASE/downloads $BASE/VDR-NG-EM/VDR-NG-FW/downloads
    fi
}

#---------------------------------------------------------------------------------------------------------------
update_firmware() {

###TODO
#info "Haciendo limpieza de ficheros..."
###rm -rf VDR-NG-EM/VDR-NG-FW | tee >(zenity --progress --auto-close --pulsate) #>> $COMP_LOG
#info "Actualizando VDR-NG-EM..." 
    if [ -e $BASE/VDR-NG-EM ]; then
        svn update $BASE/VDR-NG-EM | tee -a $COMP_LOG | zenity --progress --auto-close --auto-kill --pulsate --text $"Updating VDR-NG-EM source code"
    fi

# Aquí se configura automáticamente si la versión es PRO o LITE.
    sed -i "s/vdr_version=\".*\"/vdr_version=\"$version_firm\"/g" $BASE/VDR-NG-EM/make-fw
    info $"Reminder: building with `grep ^vdr_version $BASE/VDR-NG-EM/make-fw` option"
    info $"Preparing the building..."

    cd $BASE/VDR-NG-EM

    $LINUX32BIN ./make-fw 2>&1 | tee -a $COMP_LOG #| zenity --progress --auto-close --auto-kill --pulsate --text "Generating firmware base"

    debug $"We are in `pwd` directory"

# Aquí se configura automáticamente el modelo del Siemens
    if [ "$version_firm" = "lite" ]; then
        echo $model_m7xx > ./VDR-NG-FW/buildin/base-lite/m740/etc/systemtype
    fi

# VDR-NG-EM/VDR-NG-FW/buildin/base-pro/m740/etc/systemtype
    if [ "$version_firm" = "pro" ]; then
        echo $model_m7xx > ./VDR-NG-FW/buildin/base-pro/m740/etc/systemtype
    fi


}
#---------------------------------------------------------------------------------------------------------------
set_locale() {
if [ -f  $BASE/VDR-NG-EM/make-lang ]; then
    info $"Set firmware language [$locale_firm]"
    cd $BASE/VDR-NG-EM;
    $LINUX32BIN make-lang $locale_firm 2>&1 | tee -a $COMP_LOG;
fi
}
#---------------------------------------------------------------------------------------------------------------
generate_locale() {
    if [ "$SCRIPT_VERSION" != "$last_script_version" ] || [ ! -e $BASE/locale ]; then
        debug SCRIPT_VERSION=$SCRIPT_VERSION
        debug last_script_version=$last_script_version
        language="${LANG:0:4}"
        generate_mo_file;
        language="${LANG:0:2}"
        generate_mo_file;
    fi
}
#---------------------------------------------------------------------------------------------------------------
generate_mo_file(){
    if [ -e $BASE/VDR-NG-EM/Addons/gui-build-locale/$language.po ]; then
        if [ ! -e $BASE/locale ]; then
            mkdir $BASE/locale
        fi        
        if [ ! -e $BASE/locale/$language ]; then
            mkdir $BASE/locale/$language
        fi
        if [ ! -e $BASE/locale/$language/LC_MESSAGES ]; then
            mkdir $BASE/locale/$language/LC_MESSAGES
        fi
        msgfmt -o $BASE/locale/$language/LC_MESSAGES/gui-build.mo $BASE/VDR-NG-EM/Addons/gui-build-locale/$language.po

        TEXTDOMAIN="gui-build"
        TEXTDOMAINDIR="./locale/"

        last_script_version=$SCRIPT_VERSION
        set_general
    fi
}
#---------------------------------------------------------------------------------------------------------------
create_firmware(){

    set_locale

    info $"Building firmware..."
    info $"The building incidences will be saved in '$COMP_LOG' file"
    rm -f $BASE/VDR-NG-EM/VDR-NG-FW/*.ofi
    rm -f $BASE/VDR-NG-EM/VDR-NG-FW/*.wsw

    load_general
    backup_downloads

    cd $BASE/VDR-NG-EM/VDR-NG-FW
    export PATH=$PATH:./

# make
    $LINUX32BIN make $COMP_OPTS 2>&1 | tee -a $COMP_LOG #| zenity --progress --auto-close --pulsate --text "Creando fichero de firmware"

    info $"End of building"
    info $"Date: `date`"

    exito="NO"
    version=`svnversion $BASE/VDR-NG-EM/Addons | cut -d ":" -f 2`
    mensaje=$"Firmware created: "
    if [ -f $BASE/VDR-NG-EM/VDR-NG-FW/open7x0.org-m740.ofi ]; then
        cp $BASE/VDR-NG-EM/VDR-NG-FW/open7x0.org-m740.ofi $BASE/open7x0.org-m740-$version.ofi
        mensaje=$mensaje"\n* $BASE/open7x0.org-m740-$version.ofi"
        exito="SI"
    fi
    if [ -f $BASE/VDR-NG-EM/VDR-NG-FW/open7x0.org-m740.wsw ]; then
        cp $BASE/VDR-NG-EM/VDR-NG-FW/open7x0.org-m740.wsw $BASE/open7x0.org-m740-$version.wsw
        mensaje=$mensaje"\n* $BASE/open7x0.org-m740-$version.wsw"
        exito="SI"
    fi
    if [ -f $BASE/VDR-NG-EM/VDR-NG-FW/open7x0.org-m750s.ofi ]; then
        cp $BASE/VDR-NG-EM/VDR-NG-FW/open7x0.org-m750s.ofi $BASE/open7x0.org-m750s-$version.ofi
        mensaje=$mensaje"\n* $BASE/open7x0.org-m750s-$version.ofi"
        exito="SI"
    fi
    if [ -f $BASE/VDR-NG-EM/VDR-NG-FW/open7x0.org-m750s.wsw ]; then
        cp $BASE/VDR-NG-EM/VDR-NG-FW/open7x0.org-m750s.wsw $BASE/open7x0.org-m750s-$version.wsw
        mensaje=$mensaje"\n* $BASE/open7x0.org-m750s-$version.wsw"
        exito="SI"
    fi

    if [ "$exito" = "NO" ]; then
        zenity --error --text $"There has been an error. Look at '$COMP_LOG' file to figure out the problem."
    else
        zenity --info --text $"The firmware files have been created.\n\n$mensaje\n\nYou can proceed with the firmware installation."
    fi
 
    debug `date`
}
#---------------------------------------------------------------------------------------------------------------
first_download() {
    #Descargando fuentes del firmware si no existen
    if [ ! -e "$BASE/VDR-NG-EM" ]; then 
      svn checkout http://svn.assembla.com/svn/VDR-M7x0/unstable $BASE/VDR-NG-EM 2>&1 | tee -a $COMP_LOG | zenity --progress --auto-close --pulsate --text $"Downloading source"
      update_firmware
    fi
}

#---------------------------------------------------------------------------------------------------------------
main_menu() {

while true
do
    ans=$(zenity --list --width 300 --height 275 --title $"VDR-EM firwmare creation" --text "" --column "" --column "" \
         1 $"Install needed packages in Ubuntu" \
         2 $"Firmware Update" \
         3 $"VDR Update" \
         4 $"Plugins Update" \
         5 $"Web Server Update" \
         6 $"Setting options" \
         7 $"Firmware creation" \
         8 $"Quit"
        )
  
    if [ -z $ans ] ; then return ; fi

    case $ans in
        1) ubuntu_install;;
        2) first_download;update_firmware;generate_locale;;
        3) update_vdr;;
        4) update_plugins;;
        5) update_webif;;
        6) net_menu;set_config;general_menu;set_config; zenity --info --text $"Now you can generate the firmware" ;;
        7) create_firmware;;
        8) exit 0;;
    esac
done
}

#---------------------------------------------------------------------------------------------------------------
wizard() {
    #Instalacion de packetes
    ubuntu_install;

    #Descarga/actualizacion
    first_download;
    update_firmware;
    generate_locale;

    #Compilacion y generacion ficheros firmware
    create_firmware;
}

#--------------- MAIN -----------------------------------------------------------------------------

#Verificamos si el sistema es 64bits para utilizar linux32 al ejecutar los make
system_type 'x86_64'
if [ "$?" = "0" ] ; then
    LINUX32BIN=''
else
    LINUX32BIN='linux32'
fi

check_zenity

#Inicializar el fichero de log general
info $"Begin of building"
info $"Date: `date`"

# Introducción e instrucciones generales
zenity --info --text $"- It's going to download and build the VDR firmware sources for the Gigaset M7x0.\n- Info (in spanish): http://www.assembla.com/wiki/show/VDR-M7x0/Instalación_del_VDR-NG-EM#instalacion_linux.\n"

#Cargando configuracion guardada por defecto
load_general

#Seleccionar lenguaje
select_locale
debug $"Locale: $locale_firm"

select_install_mode
debug $"Install mode: $install_mode"

# El usuario debe elegir entre firmware 'PRO' y firmware 'LITE'
select_version
debug $"Version: $version_firm"

# El usuario debe elegir el modelo de su Siemens entre m740 y m750t
select_model
debug $"Model: $model_m7xx"

set_general
load_general

if [ "$install_mode" = "expert" ]; then
    #expert mode
    main_menu
else
    #Basic mode
    wizard
fi

#fin de script

