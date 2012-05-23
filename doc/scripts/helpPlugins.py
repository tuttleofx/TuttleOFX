#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import errno
import sys
import subprocess

from plaintext2html import plaintext2html

osname          = os.name.lower()
sysplatform     = sys.platform.lower()
windows         = osname == "nt" and sysplatform.startswith("win")
macos           = sysplatform.startswith("darwin")
linux           = not windows and not macos
unix            = not windows
is_64bits       = sys.maxsize > 2**32

currentDir      = os.getcwd()

TUTTLE_ROOT=os.environ['TUTTLE_ROOT']
os.environ['TUTTLE_HOME']=TUTTLE_ROOT+'/.tuttleofxTest'
os.environ['OFX_PLUGIN_PATH']= TUTTLE_ROOT+'/plugin'
os.environ['LD_LIBRARY_PATH']= TUTTLE_ROOT+'/lib'
os.environ['PATH']= TUTTLE_ROOT+'/bin'
os.environ['SAM_PATH']=TUTTLE_ROOT+'/bin'

process = subprocess.Popen("sam-plugins", shell=True, stdout=subprocess.PIPE)

list = process.communicate()[0]
splited = list.split('\n')

for plugin in splited:
	if( plugin == "") : break;
	processPlugin = subprocess.Popen("sam-do "+plugin+" --param-group" , shell=True, stdout=subprocess.PIPE)
	group = processPlugin.communicate()[0]
	group = group.replace("\n","")
	processPlugin = subprocess.Popen("sam-do "+plugin+" -h // --color" , shell=True, stdout=subprocess.PIPE)
	help = processPlugin.communicate()[0]

	if not os.path.exists( "help/plugin/"+group ) :
		os.makedirs( "help/plugin/"+group );
	
	pluginFile = open( "help/plugin/"+group+"/"+plugin+".html", 'w')
	htmlHelp = plaintext2html( help );
	pluginFile.write( htmlHelp );
	pluginFile.close()

#### sam tools help ####

if not os.path.exists( "help/bin/" ) :
	os.makedirs( "help/bin/" );

def helpSamTool(tool) :
	process = subprocess.Popen(tool+" -h", shell=True, stdout=subprocess.PIPE)
	help = process.communicate()[0]

	helpFile = open( "help/bin/" + tool + ".html", 'w')
	htmlHelp = plaintext2html( help );
	helpFile.write( htmlHelp );
	helpFile.close();

helpSamTool("sam")
helpSamTool("sam-ls")
helpSamTool("sam-cp")
helpSamTool("sam-mv")
helpSamTool("sam-rm")
helpSamTool("sam-do")
helpSamTool("sam-info")
helpSamTool("sam-plugins")
