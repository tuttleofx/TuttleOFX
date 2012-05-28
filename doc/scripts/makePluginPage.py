#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import errno
import sys
import subprocess
import json

processFilename = 'process.txt'

if(os.path.exists( processFilename )):
  os.remove( processFilename )

class Node(object):
	def __init__(self, data):
		self.data = data
		self.childNames = []
		self.children = []

	def addChild(self, name, obj):
		self.children.append(obj)
		self.childNames.append(name)
	
	def asChild(self, obj):
		if obj in self.childNames:
			return True
		else:
			return False
	def getChild(self, obj):
		for subNode in self.children :
			if subNode.data == obj :
				return subNode

osname          = os.name.lower()
sysplatform     = sys.platform.lower()
windows         = osname == "nt" and sysplatform.startswith("win")
macos           = sysplatform.startswith("darwin")
linux           = not windows and not macos
unix            = not windows
is_64bits       = sys.maxsize > 2**32

currentDir      = os.getcwd()

TUTTLE_ROOT     = os.environ['TUTTLE_ROOT']

os.environ['TUTTLE_HOME']=TUTTLE_ROOT+'/.tuttleofxTest'
os.environ['OFX_PLUGIN_PATH']= TUTTLE_ROOT+'/plugin'
os.environ['LD_LIBRARY_PATH']= TUTTLE_ROOT+'/lib'
os.environ['PATH']= TUTTLE_ROOT+'/bin'
os.environ['SAM_PATH']=TUTTLE_ROOT+'/bin'

process = subprocess.Popen("sam-plugins", shell=True, stdout=subprocess.PIPE)

list = process.communicate()[0]

splited = list.split('\n')

root = Node("Root")

jsonTree = {}
jsonTree["group"]= [ ]
jsonTree["plugin"]= [ ]

class Group:
	def __init__(self, name):
		self.name = name
		self.subgroups = []
		self.plugin = []

class Plugin:
	def __init__(self, name):
		self.name = name

def TuttleOFX_serialisator(obj):
	# if it's a Plugin
	if isinstance(obj, Plugin):
		return {"id": "plugin",
				"name": obj.name}
	
	# if it's a Group
	if isinstance(obj, Group):
		return {"id": "group",
				"name": obj.name,
				"subgroup": obj.subgroups,
				"plugin": obj.plugin}

	# Sinon le type de l'objet est inconnu, on lance une exception.
	raise TypeError(repr(obj) + " is not serialisable !")


for plugin in splited:
	if( plugin == "") : break;
	processPlugin = subprocess.Popen("sam-do "+plugin+" --param-group" , shell=True, stdout=subprocess.PIPE)
	group = processPlugin.communicate()[0]
	group = group.replace("\n","")
	splitedGroup = group.split('/')

	parent=root
	#print plugin
	for subgroup in splitedGroup:
		node = Node(subgroup)
		if ( parent.asChild(subgroup) == True ) :
			n = parent.getChild(subgroup)
			parent = n
		else :
			parent.addChild(subgroup,node)
			parent=node

	node = Node(plugin)
	parent.addChild(plugin, node)

def convertTreeToHtml( pluginFile, root, plugingroup="", level=0 ) :
	for subNode in root.children :
		sublevel = level+1
		if(subNode.children == [] ) :
			#print plugingroup + "\t" + subNode.data
			pluginFile.write( '<li class="plugin" ><a href="#" class="blue" group="'+plugingroup+'">'+subNode.data+'</a></li>\n' )
		else :
			newGroup = Group(subNode.data)
			pluginFile.write( '<li class="menu">' )
			pluginFile.write( '	<ul>' )
			pluginFile.write( '		<li class="button"><a href="#" class="green">'+subNode.data+'<span></span></a></li>' )
			pluginFile.write( '		<li class="dropdown">' )
			pluginFile.write( '			<ul>' )
			
			subplugingroup=plugingroup+subNode.data+"/"
			convertTreeToHtml( pluginFile, subNode, subplugingroup, sublevel )
			
			pluginFile.write( '			</ul>' )
			pluginFile.write( '		</li>' )
			pluginFile.write( '	</ul>' )
			pluginFile.write( '</li>' )


pluginFile = open( "openfxPlugins.html", 'w')

pluginFile.write( '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">' )
pluginFile.write( '<html xmlns="http://www.w3.org/1999/xhtml">' )
pluginFile.write( '<head>' )
pluginFile.write( '<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />' )
pluginFile.write( '<title>TuttleOFX | OpenFX plugins</title>' )

pluginFile.write( '<link rel="stylesheet" type="text/css" href="style.css" />' )

pluginFile.write( '<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.3.2/jquery.min.js"></script>' )
pluginFile.write( '<script type="text/javascript" src="jquery.easing.1.3.js"></script>' )

pluginFile.write( '<script type="text/javascript" src="script.js"></script>' )

pluginFile.write( '</head>\n' )

pluginFile.write( '<body>\n' )
pluginFile.write( '<br/>\n' )

pluginFile.write( '<div id="main">' )
pluginFile.write( '	<div id="menu">' )
pluginFile.write( '		<ul class="container">\n' )

convertTreeToHtml( pluginFile, root)

pluginFile.write( '		</ul>' )
pluginFile.write( '	</div>' )
pluginFile.write( '	<div id="help">' )
pluginFile.write( '		<br/><h1>Plugin</h1><br/>Select a plugin on the left menu.' )
pluginFile.write( '	</div>' )
pluginFile.write( '</div>' )

pluginFile.write( '<script type="text/javascript" src="loadHelp.js"></script>' )

pluginFile.write( '</body>' )
pluginFile.write( '</html>' )
