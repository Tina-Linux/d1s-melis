#=====================================================================================
#
#       Filename:  maketheme.sh
#
#    Description:  build lanague file of GUI
#
#        Version:  Melis3.0 
#         Create:  2018-01-12 17:04:25
#       Revision:  none
#       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
#
#         Author:  zengzhijin@allwinnertech.com
#   Organization:  BU1-PSW
# Last Modified:  2019-05-29 15:07:39
#
#=====================================================================================

#!/bin/bash
if [ ! -n "$1" ];then
	NOW_PATH=.
	HEAD_PATH=$(MELIS_BASE)/${SOLUTION}/beetles/include/res
	BIN_PATH=$(MELIS_BASE)/projects/${TARGET_BOARD}/data/UDISK/apps/

else
	BASE_PATH=$1
	NOW_PATH=$BASE_PATH/${SOLUTION}/beetles/sun20iw1_app/res/lang
	HEAD_PATH=$BASE_PATH/${SOLUTION}/beetles/include/res
	BIN_PATH=$BASE_PATH/projects/${TARGET_BOARD}/data/UDISK/apps/
fi


pushd $NOW_PATH >/dev/null

echo "###########################make lang begin############################"

./langOSDBuild lang.xml > /dev/null

cp ./lang.h     $HEAD_PATH/lang.h
cp ./lang.bin   $BIN_PATH/lang.bin

echo "############################make lang end#############################"

popd > /dev/null
