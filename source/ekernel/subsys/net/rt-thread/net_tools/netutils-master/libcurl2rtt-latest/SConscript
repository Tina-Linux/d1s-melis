#-*- encoding: utf-8 -*-
#---------------------------------------------------------------------------------
# @File:   Sconscript for Package 
# @Author: Copyright (c) 2018-2019, liu2guang 1004383796@qq.com
#---------------------------------------------------------------------------------
import os
from building import *

objs = []
cwd  = GetCurrentDir()
list = os.listdir(cwd)

for item in list:
    if os.path.isfile(os.path.join(cwd, item, 'SConscript')):
        objs = objs + SConscript(os.path.join(item, 'SConscript'))

Return('objs')
#---------------------------------------------------------------------------------
# End
#---------------------------------------------------------------------------------
