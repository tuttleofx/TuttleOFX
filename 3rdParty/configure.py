#!/usr/bin/python
# -*- coding: utf-8 -*-

import os

os.chdir("./libcaca")
os.execlp( "./configure" )
os.chdir("..")
