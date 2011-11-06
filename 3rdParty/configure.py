#!/usr/bin/python
# -*- coding: utf-8 -*-

import os


os.chdir ( "./freetype" )
os.execlp( "./configure" )
os.chdir ( ".." )

os.chdir ( "./libcaca" )
os.execlp( "./configure" )
os.chdir ( ".." )

os.chdir ( "./openjpeg" )
os.execlp( "./configure" )
os.chdir ( ".." )
