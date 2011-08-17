#!/usr/bin/python
# -*- coding: utf-8 -*-

import os


os.chdir("./python26")
os.execlp( "./configure")
os.chdir("..")

os.chdir("./libcaca")
os.execlp( "./configure" )
os.chdir("..")
