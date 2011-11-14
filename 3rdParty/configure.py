#!/usr/bin/python
# -*- coding: utf-8 -*-

import os


os.chdir ( "./freetype" )
os.system( "./configure" )
os.chdir ( ".." )

os.chdir ( "./libcaca" )
os.system( "./configure" )
os.chdir ( ".." )

os.chdir ( "./jpeg" )
os.system( "./configure" )
os.chdir ( ".." )

os.chdir ( "./openjpeg" )
os.system( "./configure" )
os.chdir ( ".." )
