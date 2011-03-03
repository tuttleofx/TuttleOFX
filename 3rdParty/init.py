#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys
import urllib
import subprocess

osname            = os.name.lower()
sysplatform       = sys.platform.lower()
windows           = osname == "nt" and sysplatform.startswith("win")
macos             = sysplatform.startswith("darwin")
linux             = not windows and not macos
unix              = not windows

global current_file # global variable used in dlProgress function
current_file = ''

def copytree(src, dst, symlinks=False, ignore=None):
    import os
    from shutil import copy2, copystat, Error
    names = os.listdir(src)
    if ignore is not None:
        ignored_names = ignore(src, names)
    else:
        ignored_names = set()

    if not os.path.exists(dst):
      try:
	  os.makedirs(dst)
      except OSError, exc:
	  # XXX - this is pretty ugly
	  if "file already exists" in exc[1]:  # Windows
	      pass
	  elif "File exists" in exc[1]:        # Linux
	      pass
	  else:
	      raise

    errors = []
    for name in names:
        if name in ignored_names:
            continue
        srcname = os.path.join(src, name)
        dstname = os.path.join(dst, name)
        try:
            if symlinks and os.path.islink(srcname):
                linkto = os.readlink(srcname)
                os.symlink(linkto, dstname)
            elif os.path.isdir(srcname):
                copytree(srcname, dstname, symlinks, ignore)
            else:
                copy2(srcname, dstname)
            # XXX What about devices, sockets etc.?
        except (IOError, os.error), why:
            errors.append((srcname, dstname, str(why)))
        # catch the Error from the recursive copytree so that we can
        # continue with other files
        except Error, err:
            errors.extend(err.args[0])
    try:
        copystat(src, dst)
    except WindowsError:
        # can't copy file access times on Windows
        pass
    except OSError, why:
        errors.extend((src, dst, str(why)))
    if errors:
        raise Error, errors 

def dlProgress(count, blockSize, totalSize):
	pcent = int( count * blockSize * 100/totalSize )
	sys.stdout.write( '\r' + current_file + '%d%% '% pcent)
	sys.stdout.flush()

global knowExtensions
knowExtensions = { 'tar': 'tar xf',
                   'tar.gz': 'tar xfz',
                   'tgz': 'tar xfz',
                   'tar.bz2': 'tar xfj',
                   'zip': 'unzip',
                   'exe': '' }

def getKnowExtensions( filename ):
	global knowExtensions
	return [f for f in knowExtensions.keys() if filename.endswith(f)]

def uncompress(filename, ext, inNewDirectory, libname):
	global knowExtensions
	binOptions = { 'tar' : {'directory':'--directory',},
	               'unzip' : {'directory':'-d',},
	             } 
	cmdFromExtension = knowExtensions[ext]
	if not cmdFromExtension:
		return
	cmd = cmdFromExtension.split()
	bin = cmd[0]
	if( inNewDirectory ):
		cmd.append( binOptions[bin]['directory'] )
		cmd.append( libname )
	cmd.append(filename)
	print '\n', ' '.join(cmd)
	if not os.path.exists( os.path.join( os.getcwd(), libname)):
		os.mkdir(os.path.join( os.getcwd() ,libname))
	p = subprocess.Popen(cmd).communicate()
	print 'uncompress and  copy', filename[:-len(ext)-1], libname, '\n'
	copytree(filename[:-len(ext)-1], libname)
	print 'end of uncompress\n'

def getAndUncompress( libraries ):
	
	for libname, url, inNewDirectory in libraries:
		print '_'*80
		print '--', libname
		parts = url.split('/')
		filename = [p for p in parts if len(getKnowExtensions(p))]
		#if len(filename) == 0:
		#	print '-'*40
		#	print 'No filename with a regognize extension in "'+libname+'" url="'+url+'"'
		#	print '-'*40
		#	continue
		filename = filename[0]
		print url, ' -> ', filename
		ext = getKnowExtensions(filename)[0]
		current_file = filename
		try:
			if os.path.isfile(filename): # if not already downloaded
				print 'Already downloaded: ', filename
			else:
				urllib.urlretrieve(url, filename, dlProgress)
			dirname = filename[:-len(ext)-1]
			#if os.path.isdir(libname) or os.path.islink(libname): # if not already uncompressed
			if os.path.isdir(filename[:-len(ext)-1]) :
				print 'Already uncompressed : ', dirname
			else:
				uncompress( filename, ext, inNewDirectory, libname )
		except Exception, e:
			print 'uncompress error (', str(e), ')'

allLibs = [
		('zlib','http://prdownloads.sourceforge.net/libpng/zlib-1.2.3.tar.gz', False) if not windows else ('zlib','http://prdownloads.sourceforge.net/gnuwin32/zlib-1.2.3.exe', False),
		('libpng', 'http://prdownloads.sourceforge.net/libpng/libpng-1.2.41.tar.gz', False) if not windows else ('png','http://prdownloads.sourceforge.net/gnuwin32/libpng-1.2.37-setup.exe', False),
		('ilmbase', 'http://download.savannah.nongnu.org/releases/openexr/ilmbase-1.0.1.tar.gz', False),
		('openexr', 'http://download.savannah.nongnu.org/releases/openexr/openexr-1.6.1.tar.gz', False),
		('boost', 'http://prdownloads.sourceforge.net/boost/boost_1_46_0.tar.bz2', False) if not windows else ('boost', 'http://www.boostpro.com/download/boost_1_46_setup.exe', False),
		('freetype','http://prdownloads.sourceforge.net/freetype/freetype-2.4.3.tar.bz2', False) if not windows else ('freetype','http://prdownloads.sourceforge.net/gnuwin32/freetype-2.3.5-1-setup.exe', False),
		('libraw','http://www.libraw.org/data/LibRaw-0.13.1.tar.gz', False)
	]

if len( sys.argv ) == 1:
	getAndUncompress( allLibs )
else:
	libDic = {}
	for lib in allLibs:
		libDic[lib[0]] = lib
	
	for libname in sys.argv[1:]:
		getAndUncompress( [libDic[libname]] )

#subprocess.Popen( ['patch', 'boost/libs/test/build/Jamfile.v2', 'boost_unit_testing_framework.patch'] )

#echo ">>> BUILDING Boost JAM"
#CURRENT_DIR=`pwd`
#cd $CURRENT_DIR/boost
#source `pwd`/bootstrap.sh
#cd $CURRENT_DIR
#
#echo ">>> PREPARING bjam.sh script"
#BJAM=../bjam.sh
#rm -f $BJAM
#echo "#!/bin/bash">>$BJAM
#echo "# This script is autogenerated from 3rdParty/init.sh">>$BJAM
#echo "export BOOST_BUILD_PATH=`pwd`/boost/tools/build/v2">>$BJAM
#echo "`pwd`/boost/bjam \$*">>$BJAM
#chmod u+x $BJAM

