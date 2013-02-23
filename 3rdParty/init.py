#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import errno
import sys
import urllib
import subprocess
import sha
import tarfile
import zipfile

from shutil import move, copystat, Error, rmtree

osname       = os.name.lower()
sysplatform  = sys.platform.lower()
windows      = osname == "nt" and sysplatform.startswith("win")
macos        = sysplatform.startswith("darwin")
linux        = not windows and not macos
unix         = not windows

current_file = ''

download_dir = "archive"
extracted_dir = "extracted"

error_list = []

def dl_progress(count, block_size, total_size):
    percent = int( count * block_size * 100/total_size )
    progress_bar = '[--------------------]'
    fill = "#" * (count * block_size * (len(progress_bar)-2)/total_size)
    progress_bar = progress_bar.replace('-'*len(fill), fill, 1)
    sys.stdout.write( '\r{} {} {} %'.format(current_file, progress_bar,
                                            percent))
    if percent == 100:
        sys.stdout.write('\nDone.\n')
    sys.stdout.flush()

known_extensions = {
                   'tar':     'tar xf',
                   'tar.gz':  'tar xfz',
                   'tgz':     'tar xfz',
                   'tar.bz2': 'tar xfj',
                   'zip':     'unzip',
                   'exe':      '',
                   }

def get_known_extensions( filename ):
    global known_extensions
    return [f for f in known_extensions.keys() if filename.endswith(f)]

def uncompress(filename, ext, in_new_directory, libname, folder):
    if ext == 'tar.gz' :
        tar = tarfile.open( download_dir + "/" + filename, 'r:*')
        tar.extractall( folder )
        out_dir = tar.getnames()[0]
        
    if ext == 'tar.bz2' :
        tar = tarfile.open( download_dir + "/" + filename, 'r:*')
        tar.extractall( folder )
        out_dir = tar.getnames()[0]

    if ext == 'tgz' :
        tar = tarfile.open( download_dir + "/" + filename, 'r:*')
        tar.extractall( folder )
        out_dir = tar.getnames()[0]
        
    if ext == 'zip' :
        zip = zipfile.ZipFile( download_dir + "/" + filename, 'r' )
        if in_new_directory == True : folder += filename[:-len(ext)-1]
        zip.extractall( folder )
        out_dir = zip.namelist()[0]

    if ext == 'exe' :
        fileToRun = download_dir + "/" + filename + ".exe"
        os.startfile( fileToRun )

    if not os.path.exists( os.path.join( os.getcwd(), folder, libname)):
        os.mkdir(os.path.join( os.getcwd(), folder, libname))

    print ('\nuncompress : %s\n ' % libname )

    if not os.path.isdir(os.path.join(folder, out_dir)):
        out_dir = os.path.dirname(out_dir)

    if out_dir != libname:
        if os.path.exists(os.path.join(folder, libname)):
            rmtree(os.path.join(folder, libname))

        os.rename(os.path.join(folder, out_dir),
                  os.path.join(folder, libname))

    print ('end of uncompress\n')

def get_and_uncompress( libraries ):
    
    global current_file

    for lib in libraries:
        libname = lib['name']
        url = lib['url']
         
        print ('%s' % ('_'*80))
        print ('-- %s'% libname)

        parts = url.split('/')
        filename = [p for p in parts if len(get_known_extensions(p))][0]

        print ( '%s -> %s' % (url, filename) )
        
        ext = get_known_extensions(filename)[0]
        current_file = filename
        try:
            if not os.path.exists( download_dir ):
                os.makedirs( download_dir )

            already_download = False
            if os.path.isfile(download_dir + "/" + filename):
                print ('Already downloaded: %s' % filename )
                with open(download_dir + "/" + filename, 'rb') as f:
                    current_sha = sha.sha(f.read()).hexdigest()
                    
                print "Sha Expected :", lib['sha']
                print "Sha Current  :", current_sha
                if lib['sha'] == current_sha:
                    print "Sha match, skipping download."
                    already_download = True
                else:
                    print "Sha dont match, redownloading archive."
            
            if not already_download:
                urllib.urlretrieve(url, download_dir + "/" + filename, dl_progress)

            if os.path.isdir(os.path.join(extracted_dir, libname)) :
                print ('Already uncompressed : %s' % os.path.join(extracted_dir, libname))
            else:
                uncompress(filename, ext, False, libname, extracted_dir)
        except Exception as e:
            print ( 'uncompress error (%s)' % str(e) )

all_libs = [
            {'name': 'zlib',
             'sha': '60faeaaf250642db5c0ea36cd6dcc9f99c8f3902',
             'url': 'http://prdownloads.sourceforge.net/libpng/zlib-1.2.3.tar.gz'},

            {'name': 'libpng',
             'sha': 'c9e5ea884d8f5551de328210ccfc386c60624366',
             'url': 'http://prdownloads.sourceforge.net/libpng/libpng-1.2.41.tar.gz'},
            
            {'name': 'ilmbase',
             'sha': '143adc547be83c6df75831ae957eef4b2706c9c0',
             'url': 'http://download.savannah.nongnu.org/releases/openexr/ilmbase-1.0.1.tar.gz'},
            
            {'name': 'openexr',
             'sha': 'b3650e6542f0e09daadb2d467425530bc8eec333',
             'url': 'http://download.savannah.nongnu.org/releases/openexr/openexr-1.6.1.tar.gz'},

            {'name': 'ctl',
             'sha': '4e31de8e8da144bbc840d97014a8f45e20e398ac',
             'url': 'http://sourceforge.net/projects/ampasctl/files/ctl/ctl-1.4.1/ctl-1.4.1.tar.gz/download'},

            {'name': 'boost',
             'sha': 'cddd6b4526a09152ddc5db856463eaa1dc29c5d9',
             'url': 'http://prdownloads.sourceforge.net/boost/boost_1_52_0.tar.bz2'},

            {'name': 'boost_atomic',
             'sha': '9087c0dc338a3dfbe9fc38051ee66b008873e167',
             'url': 'http://www.chaoticmind.net/~hcb/projects/boost.atomic/boost.atomic.tar.gz'},
            
            {'name': 'freetype',
             'sha': 'bb74c4ad6749ead86f4d3c2973492bedccc397ec',
             'url': 'http://prdownloads.sourceforge.net/freetype/freetype-2.4.3.tar.gz'},
            
            {'name': 'libraw',
             'sha': 'e924527bed3d72ee4756da0c9383dc74c584799f',
             'url': 'http://www.libraw.org/data/LibRaw-0.14.7.tar.gz'},
            
            {'name': 'libcaca',
             'sha': '24cdb9fb7dc4a01be2787a9009304fad419f79db',
             'url': 'http://caca.zoy.org/files/libcaca/libcaca-0.99.beta17.tar.gz'},
            
            {'name': 'lcms2',
             'sha': '0ce856a004068ff6e22f9f2d7b94a11b9b469bf9',
             'url': 'http://downloads.sourceforge.net/project/lcms/lcms/2.1/lcms2-2.1.tar.gz'},
            
            {'name': 'bzlib',
             'sha': '3f89f861209ce81a6bab1fd1998c0ef311712002',
             'url': 'http://www.bzip.org/1.0.6/bzip2-1.0.6.tar.gz'},
            
            {'name': 'libtiff',
             'sha': 'f40aab20fb2f609b5cbc1171c40b66a1445e3773',
             'url': 'ftp://ftp.remotesensing.org/pub/libtiff/tiff-3.9.5.tar.gz'},
            
            {'name': 'libtool',
             'sha': '149e9d7a993b643d13149a94d07bbca1085e601c',
             'url': 'ftp://ftp.gnu.org/gnu/libtool/libtool-2.4.tar.gz'},
            
            {'name': 'libxml',
             'sha': '859dd535edbb851cc15b64740ee06551a7a17d40',
             'url': 'ftp://xmlsoft.org/libxml2/libxml2-2.7.8.tar.gz'},
            
            {'name': 'jasper',
             'sha': '9c5735f773922e580bf98c7c7dfda9bbed4c5191',
             'url': 'http://www.ece.uvic.ca/~mdadams/jasper/software/jasper-1.900.1.zip'},
            
            {'name': 'xz',
             'sha': '2f64e90df16a4cc143bf7ba5788953fac593f4f6',
             'url': 'http://tukaani.org/xz/xz-5.0.2.tar.gz'},
            
            {'name': 'gvc',
             'sha': '04503ac5a9eaa579859f0d017811fa245717edec',
             'url': 'http://www.graphviz.org/pub/graphviz/stable/SOURCES/graphviz-2.26.3.tar.gz'},
            
            {'name': 'jpeg',
             'sha': '7079f0d6c42fad0cfba382cf6ad322add1ace8f9',
             'url': 'http://freefr.dl.sourceforge.net/project/libjpeg/libjpeg/6b/jpegsrc.v6b.tar.gz'},
            
            {'name': 'turbojpeg',
             'sha': '4ab00afc9a8d54cd2e7a67aacb9c49e01a8bccac',
             'url': 'http://garr.dl.sourceforge.net/project/libjpeg-turbo/1.2.0/libjpeg-turbo-1.2.0.tar.gz'},
            
            {'name': 'openjpeg',
             'sha': '9ef6a08ecf5944962b4e2cd7569ac01a8eaa66d0',
             'url': 'http://openjpeg.googlecode.com/files/openjpeg_v1_4_sources_r697.tgz'},
            
            {'name': 'ffmpeg',
             'sha': '984f731aced1380840cd8e3576e8db0c2fd5537f',
             'url': 'http://ffmpeg.org/releases/ffmpeg-0.8.2.tar.bz2'},
            
            {'name': 'openimageio',
             'sha': '5e732c9e7b86629c1ec182fe1c21161bd0e252c0',
             'url': 'https://github.com/OpenImageIO/oiio/tarball/RB-1.0/OpenImageIO-oiio-Release-1.0.0-0-g5b37f1c.tar.gz'},
            
            {'name': 'opencolorio',
             'sha': '1d7a195b684fbd0687a661aa532c5cc447215420',
             'url': 'http://github.com/imageworks/OpenColorIO/tarball/v1.0.6/imageworks-OpenColorIO-v1.0.6-0-g0f89532.tar.gz'},

            {'name': 'imagemagick',
             'sha': 'c64dcac1fe5f315d07c3a506fa3969b14122867c',
             'url': 'http://www.imagemagick.org/download/ImageMagick.tar.gz'},
            
            {'name': 'webp',
             'sha': 'f46aff9f1aeb8620d7ca4b9afc54e6df6d4de313',
             'url': 'https://webp.googlecode.com/files/libwebp-0.1.3.tar.gz'},
            
            {'name': 'tinyxml',
             'sha': '2ff9c177a6c0bef10cbd27700b71826801c12987',
             'url': 'http://netcologne.dl.sourceforge.net/project/tinyxml/tinyxml/2.6.1/tinyxml_2_6_1.tar.gz'},
            
            {'name': 'yaml',
             'sha': '5141c5a3beecbe80e6772d3d9ece1d668940906f',
             'url': 'http://yaml-cpp.googlecode.com/files/yaml-cpp-0.2.6.tar.gz'},
            ]

def insert_in_file( filename, linesIndexes, textToAppend ):
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

def comment_line_in_file( filename, row ):
    '''
    Put a c++ comment (//) statement in filename at row.
    '''
    print ( '%s' % filename )
    if os.path.exists( filename ):
        f = open(filename , 'r')
        tmp_file = open('tmp.txt' , 'w')
        lines = f.readlines()

        nlist = list(lines)

        commented_line = nlist[row]
        commented_line = "//" + commented_line
        nlist[row]=commented_line
        tmp_file.writelines(nlist)

        f.close()
        tmp_file.close()
        os.remove(filename)
        os.rename('tmp.txt', filename)
    else:
        print('file {} is not present.'.format(filename))


def insert_patch_statement_in_file( filename, statement, row ):
    '''
    Insert statement in filename at row if not already present.
    '''
    print ( '%s' % filename )
    modification = 0
    if os.path.exists( filename ):
        for line in open(filename,'r'):
            if statement in line:
                modification = 1
                break
        if modification == 0 :
            insert_in_file(filename, (1, row) , ("", statement ) )
        else:
            print ( 'header is already include.' )
    else:
        print ( 'file is not present.' )

if __name__ == "__main__":

    if len(sys.argv) == 1:
        get_and_uncompress( all_libs )
    else:
        lib_dict = {}
        for l in all_libs:
            lib_dict[l['name']] = l
        for libname in sys.argv[1:]:
            get_and_uncompress([lib_dict[libname]])
            sys.exit()

    print ( '%s' % ('_'*80) )
    print ( '%s Files modifications  %s' % ( ('-'*29) , ('-'*29) ) )

    # Insertion patches
    print "-- Insertion patches"
    patch_list = [
                 #(filename, insert_line, at_line)
                  ("ctl/IlmCtl/CtlLex.cpp", "#include <cstdlib>", 60),
                  ("yaml/src/ptr_stack.h", "#include <cstdlib>", 11),
                  ("yaml/src/ptr_vector.h", "#include <cstdlib>", 11),
                  ("ctl/IlmCtlSimd/CtlSimdReg.h", "#include <cstring>", 55),
                  ]

    for filename, line, row in patch_list:
        insert_patch_statement_in_file(os.path.join(extracted_dir, filename), line, row)

    # Commenting patches
    print "-- Commenting patches"
    patch_list = [
                 #(filename, row),
                  ("yaml/src/emitter.cpp", 663)
                  ]
    for filename, row in patch_list:
        comment_line_in_file(os.path.join(extracted_dir, filename), row)
    
    print ( 'End of initialisation.' )
