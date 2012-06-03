#!/usr/bin/python
import sys, os, string, shutil, time



def walktree(top = '.', depthfirst = True):
	import os, stat, types
	names = os.listdir(top)
	if not depthfirst:
		yield top, names
	for name in names:
		try:
			st = os.lstat(os.path.join(top, name))
		except os.error:
			continue
		if stat.S_ISDIR(st.st_mode):
			for (newtop, children) in walktree (os.path.join(top, name), depthfirst):
				yield newtop, children
	if depthfirst:
		yield top, names

def adaptTemplate(filepath):
	#print 'adaptTemplate: ', filepath
	# Schmlurp whole file
	fSource = open( filepath, 'r' )
	source = fSource.read( )
	fSource.close( )
	source = source.replace('%L_CLASSNAME%', className.lower())
	source = source.replace('%D_CLASSNAME%', className.upper())
	source = source.replace('%CLASSNAME%', className)
	source = source.replace('%NAMESPACE%', namespace)
	source = source.replace('%PLUGIN_GROUP%', pluginGroup)
	source = source.replace('%PLUGIN_LABEL%', pluginLabel)
	source = source.replace('%PLUGIN_LONG_LABEL%', pluginLongLabel)
	source = source.replace('%PLUGIN_UNIQUE_ID%', pluginUniqueId)
	source = source.replace('%DATE%', time.strftime('%d/%m/%y %H:%M',time.localtime()))
	fDest = open( filepath, 'w' )
	fDest.write(source)
	fDest.close()

if __name__ == '__main__':
	try:
		# we can set the output directory on the command line
		defaultDir = None
		if len(sys.argv) > 1:
			defaultDir = sys.argv[1]

		# Adapting directories.
		currentDir = os.getcwd()
		pluginBaseDir = raw_input( 'Plugin output directory (default is '+( ('"'+defaultDir+'"') if defaultDir else 'current directory')+') ?: ' )
		if not pluginBaseDir:
			if defaultDir:
				pluginBaseDir = defaultDir
			else:
				pluginBaseDir = currentDir
		if not os.path.isabs(pluginBaseDir):
			pluginBaseDir = os.path.join( currentDir, pluginBaseDir )

		api = 'ImageEffectApi'
		while True:
			api_input = raw_input( 'api (default: "'+api+'") ?: ' )
			if api_input.find( ' ' ) != -1  or not os.path.isdir(os.path.join(os.getcwd(),api_input)):
				print 'Not valid api name.'
			else:
				if api_input:
					api = api_input
				break

		className = ''
		while not className or className.find( ' ' ) != -1  or className[0].isupper( ) == False:
			className = raw_input( 'Plugin name (Capitalized, without spaces) ?: ' )

		pluginDir = os.path.join( pluginBaseDir, className )

		#Remove old plugin dir
		if os.path.exists( pluginDir ):
			print
			deleteDest = ''
			while deleteDest != 'y' and deleteDest != 'n':
				deleteDest = raw_input( 'Warning ! Destination %s will be removed from the disk !\nAre you sure (y/n) ?:' % pluginDir).lower()
			if deleteDest == 'y':
				shutil.rmtree( pluginDir )
			else:
				sys.exit(2)
			print

		pluginLabel = raw_input( 'Plugin label (default "'+className+'") ?: ' )
		if not pluginLabel:
			pluginLabel = className
		pluginLongLabel = raw_input( 'Plugin long label (default "'+pluginLabel+'") ?: ' )
		if not pluginLongLabel:
			pluginLongLabel = pluginLabel
		pluginGroup = raw_input( 'Plugin group ? (default "tuttle"): ' )
		if not pluginGroup:
			pluginGroup = 'tuttle'
		defaultNamespace = className[0].lower() + className[1:]
		namespace = raw_input( 'Namespace (default "'+defaultNamespace+'") ?: ')
		if not namespace:
			namespace = defaultNamespace
		print
		print 'Setting up the declaration string (eg. "com.PROJECT.COMPANY.'+className.lower()+'"):'
		id = []
		country = raw_input( 'country ? (eg. "com"): ' )
		if country:
			id.append( country )
		projectName = raw_input( 'project ? (default "tuttle"): ' )
		if not projectName:
			projectName = 'tuttle'
		id.append( projectName )
		companyName = raw_input( 'company ?: ' )
		if companyName:
			id.append( companyName )
		id.append( className.lower() )
		pluginUniqueId = '.'.join(id)

		print 'Processing files...'
		if not os.path.exists(pluginBaseDir):
			os.makedirs( pluginBaseDir )

		#Copy to plugin directory
		thisScriptDir = os.path.abspath(os.path.dirname(__file__))
		shutil.copytree( os.path.join(thisScriptDir, api), pluginDir )
		for top, names in walktree( pluginDir ):
			for name in names:
				# Rename templates files with a correct filename
				dName = name.replace( 'L_ClassName_', className.lower() )
				dName = dName.replace( '_ClassName_', className )
				dName = dName.replace( '_PLUGIN_UNIQUE_ID_', pluginUniqueId )
				dName = dName.replace( 'L_ProjectName_', projectName.lower() )
				dName = dName.replace( '_ProjectName_', projectName )
				if dName[-1] == '_':
					dName = dName[:-1]
				inputFile = os.path.join(top, name)
				outputFile = os.path.join(top, dName)
				if dName != name:
					os.rename( inputFile, outputFile )
				# Check if we need to replace things on the file
				if os.path.isfile(outputFile):
					print 'Processing: ' + name + ' to ' + dName
					if not any( dName.endswith(ext) for ext in ['.png', '.svg']):
						adaptTemplate( outputFile )

	except ValueError, v:
		try:
			( code, message ) = v
		except:
			code = 0
			message = v
		print 'Sorry, ' + str( message ) + ' (' + str( code ) + ')'
		print
		sys.exit(1)
