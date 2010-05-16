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

def replaceInFile(filepath, replaceInput, replaceOutput):
	fSource = open( filepath, 'r' )
	source = fSource.read( )
	fSource.close( )
	source = source.replace(replaceInput, replaceOutput)
	fDest = open( filepath, 'w' )
	fDest.write(source)
	fDest.close()


if __name__ == '__main__':
	try:
		if len(sys.argv):
			print "usage ./"+sys.argv[0]+" ./directory/to/rename"
		
		directory = sys.argv[1]
		#while not directory:
		#	directory = raw_input( 'directory : ' )
		
		replaceInput = ''
		while not replaceInput:
			replaceInput = raw_input( 'to replace : ' )
		
		replaceOutput = ''
		while not replaceOutput:
			replaceOutput = raw_input( 'replace by : ' )
		
		print 'Processing files...'
		
		for top, names in walktree( directory ):
			for name in names:
				# rename filenames
				dName = name.replace( replaceInput, replaceOutput )
				inputFile = os.path.join(top, name)
				outputFile = os.path.join(top, dName)
				if dName != name:
					os.rename( inputFile, outputFile )
				# Check if we need to replace things on the file
				if os.path.isfile(outputFile):
					print 'Processing: ' + name + ' to ' + dName
					replaceInFile(outputFile, replaceInput, replaceOutput)

	except ValueError, v:
		try:
			( code, message ) = v
		except:
			code = 0
			message = v
		print 'Sorry, ' + str( message ) + ' (' + str( code ) + ')'
		print
		sys.exit(1)

