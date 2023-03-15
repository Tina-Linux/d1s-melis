#=====================================================================================
#
#       Filename:  maketheme.sh
#
#    Description:  build theme file of GUI
#
#        Version:  Melis3.0 
#         Create:  2018-01-12 17:04:25
#       Revision:  none
#       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
#
#         Author:  zengzhijin@allwinnertech.com
#   Organization:  BU1-PSW
# Last Modified:  2019-05-29 15:09:09
#
#=====================================================================================

#!/bin/bash

progname=

if [ ! -n "$1" ];then
    NOW_PATH=.
    HEAD_PATH=$MELIS_BASE/${SOLUTION}/beetles/include/res
    BIN_PATH=$MELIS_BASE/projects/${TARGET_BOARD}/data/UDISK/apps/
else
    BASE_PATH=$1
    NOW_PATH=$BASE_PATH/${SOLUTION}/beetles/sun20iw1_app/res/theme
    HEAD_PATH=$BASE_PATH/${SOLUTION}/beetles/include/res
    BIN_PATH=$BASE_PATH/projects/${TARGET_BOARD}/data/UDISK/apps/
fi

pushd $NOW_PATH >/dev/null
mv config/touchthemehdr.script config/touchthemehdr
rm -fr */*.script
mv config/touchthemehdr config/touchthemehdr.script

echo "##########################make script begin###########################"

if [ $(getconf WORD_BIT) = '32' ] && [ $(getconf LONG_BIT) = '64' ] ; then
	progname=./MakeScript
else
	progname=./MakeScript_32
fi

$progname  ./init       1       1000 >/dev/null
$progname  ./home       1001    2000 >/dev/null
$progname  ./movie      2001    3000 >/dev/null
$progname  ./explorer   3001    4000 >/dev/null
$progname  ./music      4001    5000 >/dev/null
$progname  ./setting    5001    6000 >/dev/null
$progname  ./dialog     6001    7000 >/dev/null
$progname  ./loading    7001    8000 >/dev/null
$progname  ./alarm      8001    9000 >/dev/null
$progname  ./linein     9001    10000 >/dev/null
$progname  ./ebook      10001   11000 >/dev/null
$progname  ./fm         11001   12000 >/dev/null
$progname  ./photo      12001   13000 >/dev/null
$progname  ./record     13001   14000 >/dev/null
$progname  ./game       14001   15000 >/dev/null
$progname  ./dtv        15001   16000 >/dev/null

echo "###########################make script end############################"

rm -fr touchtheme.script
cat ./config/touchthemehdr.script   \
    ./init/init.script              \
    ./home/home.script              \
    ./movie/movie.script            \
    ./explorer/explorer.script      \
    ./music/music.script            \
    ./setting/setting.script        \
    ./dialog/dialog.script          \
    ./loading/loading.script        \
    ./alarm/alarm.script            \
    ./linein/linein.script          \
    ./ebook/ebook.script            \
    ./fm/fm.script                  \
    ./photo/photo.script            \
    ./record/record.script          \
    ./game/game.script              \
    ./dtv/dtv.script                \
    >> touchtheme.script
    
echo "###########################make theme begin###########################"

if [ $(getconf WORD_BIT) = '32' ] && [ $(getconf LONG_BIT) = '64' ] ; then
	progname=./facebuildercmd
else
	progname=./facebuildercmd
fi

$progname ./config/config.ini > /dev/null & 

while test $(pgrep -P $$ | wc -l) -ne 1
do
    sleep 1
    echo -n '.'
done
echo -ne "\n"

echo "############################make theme end############################"

cp  ./theme.h       $HEAD_PATH/theme.h
cp  ./theme.bin     $BIN_PATH/theme.bin

popd >/dev/null
