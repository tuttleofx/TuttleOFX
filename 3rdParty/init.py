#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import errno
import sys
import urllib
import subprocess
import tarfile
import zipfile

from shutil import move, copystat, Error, rmtree

osname		= os.name.lower()
sysplatform	= sys.platform.lower()
windows		= osname == "nt" and sysplatform.startswith("win")
macos		= sysplatform.startswith("darwin")
linux		= not windows and not macos
unix		= not windows

global current_file # global variable used in dlProgress function
current_file = ''
download_dir = "archive"

def movetree(src, dst, symlinks=False):
	names = os.listdir(src)
	if not os.path.exists(dst) :
		os.makedirs(dst)
	errors = []
	for name in names:
		srcname = os.path.join(src, name)
		dstname = os.path.join(dst, name)
		try:
			if symlinks and os.path.islink(srcname):
				linkto = os.readlink(srcname)
				os.symlink(linkto, dstname)
			elif os.path.isdir(srcname):
				movetree(srcname, dstname, symlinks)
				#copytree(srcname, dstname, symlinks)
			else:
				move(srcname, dstname)
				#copy2(srcname, dstname)
			# XXX What about devices, sockets etc.?
		except (IOError, os.error) as why:
			errors.append((srcname, dstname, str(why)))
		# catch the Error from the recursive copytree so that we can
		# continue with other files
		except Error as err:
			errors.extend(err.args[0])
	try:
		copystat(src, dst)
	except WindowsError:
		# can't copy file access times on Windows
		pass
	except OSError as why:
		errors.extend((src, dst, str(why)))
	if errors:
		raise Error(errors)

def dlProgress(count, blockSize, totalSize):
	pcent = int( count * blockSize * 100/totalSize )
	sys.stdout.write( '\r' + current_file + '%d%% '% pcent)
	sys.stdout.flush()

global knownExtensions
knownExtensions = {	'tar':		'tar xf',
			'tar.gz':	'tar xfz',
			'tgz':		'tar xfz',
			'tar.bz2':	'tar xfj',
			'zip':		'unzip',
			'exe':		'' }


def getKnownExtensions( filename ):
	global knownExtensions
	return [f for f in knownExtensions.keys() if filename.endswith(f)]

def uncompress(filename, ext, inNewDirectory, libname, folderExtracted):
	if ext == 'tar.gz' :
		tar = tarfile.open( download_dir + "/" + filename, 'r:*')
		folder = './'
		tar.extractall( folder )
		if not folderExtracted :
			folderExtracted = tar.getnames()[0]

	if ext == 'tar.bz2' :
		tar = tarfile.open( download_dir + "/" + filename, 'r:*')
		folder = './'
		tar.extractall( folder )
		if not folderExtracted :
			folderExtracted = tar.getnames()[0]

	if ext == 'tgz' :
		tar = tarfile.open( download_dir + "/" + filename, 'r:*')
		folder = './'
		tar.extractall( folder )
		if not folderExtracted :
			folderExtracted = tar.getnames()[0]

	if ext == 'zip' :
		zip = zipfile.ZipFile( download_dir + "/" + filename, 'r' )
		folder = './'
		if inNewDirectory == True : folder += filename[:-len(ext)-1]
		zip.extractall( folder )
		if not folderExtracted :
			folderExtracted = zip.namelist()[0]

	if ext == 'exe' :
		fileToRun = download_dir + "/" + filename + ".exe"
		os.startfile( fileToRun )

	if not os.path.exists( os.path.join( os.getcwd(), libname)):
		os.mkdir(os.path.join( os.getcwd() ,libname))

	print ('\nuncompress : %s -> %s\n ' % ( folderExtracted, libname ) )

	if os.path.exists( libname + '/Jamfile.v2' ) :
		print ( 'move %s => %s' % ( libname, folderExtracted ) )
		movetree( libname , folderExtracted )

	if folderExtracted != libname :
		if os.path.exists(libname) :
			rmtree( libname )
		os.rename( folderExtracted, libname )

	print ('end of uncompress\n')

def getAndUncompress( libraries ):
	for libname, url, folderExtracted, inNewDirectory in libraries:
		print ('%s' % ('_'*80))
		print ('-- %s'% libname)
		parts = url.split('/')
		filename = [p for p in parts if len(getKnownExtensions(p))]
		#if len(filename) == 0:
		#	print '-'*40
		#	print 'No filename with a regognize extension in "'+libname+'" url="'+url+'"'
		#	print '-'*40
		#	continue
		filename = filename[0]
		print ( '%s -> %s' % (url, filename) )
		ext = getKnownExtensions(filename)[0]
		current_file = filename
		try:
			if not os.path.exists( download_dir ):
				os.makedirs( download_dir )
			if os.path.isfile( download_dir + "/" + filename ): # if not already downloaded
				print ('Already downloaded: %s' % filename )
			else:
				urllib.urlretrieve(url, download_dir + "/" + filename, dlProgress)
			dirname = filename[:-len(ext)-1]
			#if os.path.isdir(libname) or os.path.islink(libname): # if not already uncompressed
			if os.path.isdir(filename[:-len(ext)-1]) :
				print ('Already uncompressed : %s' % dirname )
			else:
				#print ('%s | %s | %s | %s | %s' % ( filename, ext, inNewDirectory, libname, folderExtracted ) )
				uncompress( filename, ext, inNewDirectory, libname, folderExtracted )
		except Exception as e:
			print ( 'uncompress error (%s)' % str(e) )

allLibs = [
		('zlib',        'http://prdownloads.sourceforge.net/libpng/zlib-1.2.3.tar.gz'                                               , '', False), #if not windows else ('zlib'    , 'http://downloads.sourceforge.net/project/libpng/zlib/1.2.3/zlib123.zip'                      , '', True),
		('libpng',      'http://prdownloads.sourceforge.net/libpng/libpng-1.2.41.tar.gz'                                            , '', False), #if not windows else ('libpng'  , 'http://downloads.sourceforge.net/project/libpng/libpng12/older-releases/1.2.41/lpng1241.zip' , '', False),
		('ilmbase',     'http://download.savannah.nongnu.org/releases/openexr/ilmbase-1.0.1.tar.gz'                                 , '', False),
		('openexr',     'http://download.savannah.nongnu.org/releases/openexr/openexr-1.6.1.tar.gz'                                 , '', False),
		('ctl',         'http://sourceforge.net/projects/ampasctl/files/ctl/ctl-1.4.1/ctl-1.4.1.tar.gz/download'                    , '', False),
		('boost',       'http://prdownloads.sourceforge.net/boost/boost_1_50_0.tar.bz2'                                             , '', False), #if not windows else ('boost'   , 'http://sourceforge.net/projects/boost/files/boost/1.47.0/boost_1_50_0.zip'                   , '', False),
		('freetype',    'http://prdownloads.sourceforge.net/freetype/freetype-2.4.3.tar.gz'                                         , '', False), #if not windows else ('freetype', 'http://prdownloads.sourceforge.net/freetype/freetype-2.4.3.tar.gz'                           , '', False),
		('libraw',      'http://www.libraw.org/data/LibRaw-0.14.5.tar.gz'                                                           , '', False), #if not windows else ('libraw'  , 'http://www.libraw.org/data/LibRaw-0.13.5.zip'                                                , '', False),
		('libcaca',     'http://caca.zoy.org/files/libcaca/libcaca-0.99.beta17.tar.gz'                                              , '', False),
		('lcms2',       'http://downloads.sourceforge.net/project/lcms/lcms/2.1/lcms2-2.1.tar.gz'                                   , '', False), #if not windows else ('lcms'    , 'http://downloads.sourceforge.net/project/lcms/lcms/2.1/lcms2-2.1.zip'                        , '', False),
		('bzlib',       'http://www.bzip.org/1.0.6/bzip2-1.0.6.tar.gz'                                                              , 'bzip2-1.0.6', False),
		('libtiff',     'ftp://ftp.remotesensing.org/pub/libtiff/tiff-3.9.5.tar.gz'                                                 , '', False), #if not windows else ('libtiff' , 'ftp://ftp.remotesensing.org/pub/libtiff/tiff-3.9.5.zip'                                      , '', False),
		('libtool',     'ftp://ftp.gnu.org/gnu/libtool/libtool-2.4.tar.gz'                                                          , '', False),
		('libxml',      'ftp://xmlsoft.org/libxml2/libxml2-2.7.8.tar.gz'                                                            , '', False), #if not windows else ('libxml'  , 'ftp://xmlsoft.org/libxml2/win32/libxml2-2.7.7.win32.zip'                                     , '', False),
		('jasper',      'http://www.ece.uvic.ca/~mdadams/jasper/software/jasper-1.900.1.zip'                                        , '', False),
		('xz',          'http://tukaani.org/xz/xz-5.0.2.tar.gz'                                                                     , '', False),
		('gvc',         'http://www.graphviz.org/pub/graphviz/stable/SOURCES/graphviz-2.26.3.tar.gz'                                , '', False),
		('jpeg',        'http://freefr.dl.sourceforge.net/project/libjpeg/libjpeg/6b/jpegsrc.v6b.tar.gz'                            , '', False), #if not windows else ('jpeg'    , 'http://netcologne.dl.sourceforge.net/project/libjpeg/libjpeg/6b/jpegsr6.zip'                 , '', False),
		('turbojpeg',   'http://garr.dl.sourceforge.net/project/libjpeg-turbo/1.2.0/libjpeg-turbo-1.2.0.tar.gz'                     , '', False),
		('openjpeg',    'http://openjpeg.googlecode.com/files/openjpeg_v1_4_sources_r697.tgz'                                       , '', False),
		('ffmpeg',      'http://ffmpeg.org/releases/ffmpeg-0.8.2.tar.bz2'                                                           , '', False),
		('openimageio', 'https://github.com/OpenImageIO/oiio/tarball/RB-1.0/OpenImageIO-oiio-Release-1.0.0-0-g5b37f1c.tar.gz'       , '', False),
		('opencolorio', 'http://github.com/imageworks/OpenColorIO/tarball/v1.0.6/imageworks-OpenColorIO-v1.0.6-0-g0f89532.tar.gz'   , '' , False),
		('imagemagick', 'http://www.imagemagick.org/download/ImageMagick.tar.gz'                                                    , '', False),
		('webp',        'https://webp.googlecode.com/files/libwebp-0.1.3.tar.gz'                                                    , '', False),
		('tinyxml',     'http://netcologne.dl.sourceforge.net/project/tinyxml/tinyxml/2.6.1/tinyxml_2_6_1.tar.gz'                   , '', False),
		('yaml',        'http://yaml-cpp.googlecode.com/files/yaml-cpp-0.2.6.tar.gz'                                                , '', False),
	]


def insertInFile( filename, linesIndexes, textToAppend ):
	file = open( filename , 'r' )
	fileTmp = open( 'tmp.txt' , 'w' )
	lines = file.readlines()

	nlist = list( lines )

	index = 0;

	for lineIndex in linesIndexes:
		nlist.insert( lineIndex , textToAppend[index]+'\n' )
		index+=1
	fileTmp.writelines( nlist )

	file.close()
	fileTmp.close()
	os.remove(filename)
	os.rename('tmp.txt', filename)

def commentInFile( filename, atLine ):
	print ( '%s' % filename )
	if(os.path.exists( filename )):
		file = open( filename , 'r' )
		fileTmp = open( 'tmp.txt' , 'w' )
		lines = file.readlines()

		nlist = list( lines )

		commentedLine = nlist[atLine]
		commentedLine = "//" + commentedLine
		nlist[atLine]=commentedLine
		fileTmp.writelines( nlist )

		file.close()
		fileTmp.close()
		os.remove(filename)
		os.rename('tmp.txt', filename)
	else:
		print( 'file is not present.' )


def makeModificationIfNecessaryInFile( filename, headerToInsert, atLine ):
	print ( '%s' % filename )
	modification = 0
	if(os.path.exists( filename )):
		for line in open(filename,'r'):
			if headerToInsert in line:
				modification = 1
				break
		if modification == 0 :
			insertInFile(filename, (1, atLine) , ("", headerToInsert ) )
		else:
			print ( 'header is already include.' )
	else:
		print ( 'file is not present.' )

if len( sys.argv ) == 1:
	getAndUncompress( allLibs )
else:
	libDic = {}
	for lib in allLibs:
		libDic[lib[0]] = lib

	for libname in sys.argv[1:]:
		getAndUncompress( [libDic[libname]] )

print ( '%s' % ('_'*80) )
print ( '%s Files modifications  %s' % ( ('-'*29) , ('-'*29) ) )

filename = "ctl/IlmCtl/CtlLex.cpp"
headerToInsert = "#include <cstdlib>"
atLine = 60
makeModificationIfNecessaryInFile(filename, headerToInsert, atLine );

filename = "yaml/src/ptr_stack.h"
headerToInsert = "#include <cstdlib>"
atLine = 11
makeModificationIfNecessaryInFile(filename, headerToInsert, atLine );

filename = "yaml/src/ptr_vector.h"
headerToInsert = "#include <cstdlib>"
atLine = 11
makeModificationIfNecessaryInFile(filename, headerToInsert, atLine );

filename = "ctl/IlmCtlSimd/CtlSimdReg.h"
headerToInsert = "#include <cstring>"
atLine = 55
makeModificationIfNecessaryInFile(filename, headerToInsert, atLine );

#OpenColorIO third parties patch

#patch yaml
filename = "yaml/src/emitter.cpp"
atLine = 663
commentInFile(filename, atLine );

print ( 'End of initialisation.' )

