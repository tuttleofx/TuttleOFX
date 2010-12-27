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

def checkOpenClose(filepath, openBracket, closeBracket):
	fSource = open( filepath, 'r' )
	source = fSource.read( )
	fSource.close( )
	return ( source.count(openBracket), source.count(closeBracket) )

if __name__ == '__main__':
	try:
		if len(sys.argv) < 2:
			print "usage: ./"+sys.argv[0]+" ./directory/to/check"

		path = sys.argv[1]

#		print( sys.argv )

		brackets = '{}'
		if len(sys.argv) > 2:
			brackets = sys.argv[2]
			print 'Using brackets: "'+brackets+'"'
#		else:
#			userBrackets = raw_input( 'brackets (default="{}"): ' )
#			if userBrackets:
#				if len( userBrackets ) != 2:
#					raise ValueError( 'You need 2 caracters, like "()".' )
#				brackets = userBrackets

#		print 'Search brackets:"'+brackets+'" ...'

		if os.path.isfile(path):
			fullfilename = path
			nbOpen, nbClose = checkOpenClose(fullfilename, brackets[0], brackets[1])
			if nbOpen != nbClose:
				print fullfilename, ' | ', 'open:', nbOpen, ', close:', nbClose
		else:
			for top, names in walktree( path ):
				for name in names:
					fullfilename = os.path.join(top, name)
					if os.path.isfile(fullfilename):
						nbOpen, nbClose = checkOpenClose(fullfilename, brackets[0], brackets[1])
						if nbOpen != nbClose:
							print fullfilename, ' | ', 'open:', nbOpen, ', close:', nbClose
						
#		print 'Search end.'

	except ValueError, v:
		try:
			( code, message ) = v
		except:
			code = 0
			message = v
		print 'Sorry, ' + str( message ) + ' (' + str( code ) + ')'
		print
		sys.exit(1)

