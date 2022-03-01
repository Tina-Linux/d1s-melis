#!/bin/bash
commitid=`git rev-parse HEAD`
LANG=C
echo \#define APP_SDK_VERSION \"V1.0-`date +%Y%m%d%H%M`\" >> .ver
mv -f .ver app_version.h
rm -f .ver
