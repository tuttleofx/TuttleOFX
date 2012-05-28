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



def convertTreeToJson( root, parent, level=0 ) :
	for subNode in root.children :
		#print "  " * level + subNode.data
		sublevel = level+1
		if(subNode.children == [] ) :
			parent.plugin.append(subNode.data)
		else :
			newGroup = Group(subNode.data)
			parent.subgroups.append(newGroup)
			parent = newGroup
		convertTreeToJson(subNode, parent, sublevel)

OFXJSON = Group("OpenFXNodes")
convertTreeToJson(root, OFXJSON)

#print json.dumps(OFXJSON, indent=4, default=TuttleOFX_serialisator)

with open('openfx.json', 'w') as file:
	json.dump(OFXJSON, file, indent=4, default=TuttleOFX_serialisator)

