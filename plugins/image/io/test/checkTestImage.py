#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import errno
import sys
import subprocess
import datetime
now = datetime.datetime.now()

processFilename = 'check.txt'

if(os.path.exists( processFilename )):
  os.remove( processFilename )

osname		= os.name.lower()
sysplatform	= sys.platform.lower()
windows		= osname == "nt" and sysplatform.startswith("win")
macos		= sysplatform.startswith("darwin")
linux		= not windows and not macos
unix		= not windows
is_64bits	= sys.maxsize > 2**32

currentDir	= os.getcwd()

TUTTLE_ROOT	= currentDir+'/../../../../dist/linux/release'
if( not os.path.exists( TUTTLE_ROOT+'/.tuttleofxTest' )):
  os.mkdir( TUTTLE_ROOT+'/.tuttleofxTest' )

os.environ['TUTTLE_ROOT']= TUTTLE_ROOT 
os.environ['TUTTLE_HOME']=TUTTLE_ROOT+'/.tuttleofxTest'
os.environ['OFX_PLUGIN_PATH']= TUTTLE_ROOT+'/plugin'
os.environ['LD_LIBRARY_PATH']= TUTTLE_ROOT+'/lib'
os.environ['PATH']= TUTTLE_ROOT+'/bin'
os.environ['SAM_PATH']=TUTTLE_ROOT+'/bin'

TT_EXE='sam-diff'
if( windows ):
	TT_EXE='sam-diff.exe'

TT_OUT_FILE=' >> generate.txt'

HTML_FILE='result.html'
CSS_FILE='style.css'

htmlF = open(HTML_FILE, 'w')
cssF  = open(CSS_FILE , 'w')

RED_COLOR  = '\033[1;31m'
BLUE_COLOR = '\033[1;34m'
CYAN_COLOR = '\033[1;36m'
NC_COLOR   = '\033[0m'  # No Color

GENERATOR="colorgradient"
GENERATOR_ARGS="type=2d nbPoints=5 point0=0,0 color0=0,0,0,0 point1=1,1 color1=1,1,1,1 point2=0,1 color2=0,1,0,1 point3=1,0.1 color3=0,0,1,1 point4=0.5,0.5 color4=1,0,0,1 width=512"


def writeHtmlHeader() :
	htmlF.write('<html>\n')
	#HEAD
	htmlF.write(' <head>\n')
	htmlF.write('\t<link rel="stylesheet" type="text/css" href="'+CSS_FILE+'">\n')
	htmlF.write('\t<link href="http://ajax.googleapis.com/ajax/libs/jqueryui/1.8/themes/base/jquery-ui.css" rel="stylesheet" type="text/css"/>\n')
	htmlF.write('\t<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.5/jquery.min.js"></script>\n')
	htmlF.write('\t<script src="http://ajax.googleapis.com/ajax/libs/jqueryui/1.8/jquery-ui.min.js"></script>\n')
	htmlF.write('\t<script>\n')
	htmlF.write('\t$(document).ready(function() {\n')
	htmlF.write('\t\t$("#accordion").accordion({ collapsible:true, active:false, autoHeight:false });\n')
	htmlF.write('\t});\n')

	htmlF.write('\t</script>\n')
	htmlF.write(' </head>\n')
	# BODY
	htmlF.write(' <body>\n')
	htmlF.write('\t<a style="color:#fff">update : ' + now.strftime("%Y-%m-%d %H:%M") + '</a><br/>\n')
	htmlF.write('\t<div id="accordion">\n')

	cssF.write('body {')
	cssF.write('background-color:black;')
	cssF.write('}')

def startFormat ( format ) :
	formatIsOK=1
	htmlF.write('\t<h3 style="background:none;" id="' + format + '"><a href="#">' +  format + '</a></h3>\n')
	htmlF.write('\t<div>\n')
	htmlF.write('\t\t<table border=0 cellpadding=5 >\n')
	htmlF.write('\t\t<tr>\n')
	htmlF.write('\t\t\t<td bgcolor=#E4E4E4>Format</td>\n')
	htmlF.write('\t\t\t<td bgcolor=#E4E4E4>Channels</td>\n')
	htmlF.write('\t\t\t<td bgcolor=#E4E4E4>Bit Depth</td>\n')
	htmlF.write('\t\t\t<td bgcolor=#E4E4E4>Plugin</td>\n')
	htmlF.write('\t\t\t<td bgcolor=#E4E4E4>Read</td>\n')
	htmlF.write('\t\t\t<td bgcolor=#E4E4E4>Write</td>\n')
	htmlF.write('\t\t<tr>\n')

def writeHtmlInsertLigne ( col1, col2, col3, col4, detailCol5, col5, detailCol6, col6) :
	htmlF.write('\t\t<tr>\n')
	htmlF.write('\t\t\t<td>' + col1 + '</td>\n')
	htmlF.write('\t\t\t<td>' + col2 + '</td>\n')
	htmlF.write('\t\t\t<td>' + col3 + '</td>\n')
	htmlF.write('\t\t\t<td>' + col4 + '</td>\n')
	htmlF.write('\t\t\t<td ' + detailCol5 + ' >' + col5 + '</td>\n')
	htmlF.write('\t\t\t<td ' + detailCol6 + ' >' + col6 + '</td>\n')
	htmlF.write('\t\t</tr>\n')


def endFormat ( format, formatIsValid ) :
	htmlF.write('\t\t</table>\n')
	htmlF.write('\t</div>\n')

	cssF.write('#' + format + ' { ')
	
	if ( formatIsValid == True ):
		cssF.write('background-color:#94D64D !important;')
	else :
		cssF.write('background-color:#D67E55 !important;')
	cssF.write('}')


def writeHtmlFooter ():
	htmlF.write(' </body>\n')
	htmlF.write('</html>\n')

COLOR_OK="#94D64D"
COLOR_BAD="#D67E55"
COLOR_UNDEF="#E4E4E4"

def writeRGoodWGood ( format, channel, bitDepth, plugin ) :
	writeHtmlInsertLigne( format, channel, bitDepth, plugin, "bgcolor=" + COLOR_OK, "OK", "bgcolor=" + COLOR_OK, "OK")

def writeRGoodWBad ( format, channel, bitDepth, plugin ) :
	writeHtmlInsertLigne( format, channel, bitDepth, plugin, "bgcolor=" + COLOR_OK, "OK", "bgcolor=" + COLOR_BAD, "BAD")

def writeRBadWGood ( format, channel, bitDepth, plugin ) :
	writeHtmlInsertLigne( format, channel, bitDepth, plugin, "bgcolor=" + COLOR_BAD, "BAD", "bgcolor=" + COLOR_OK, "OK")

def writeRBadWBad ( format, channel, bitDepth, plugin ) :
	writeHtmlInsertLigne( format, channel, bitDepth, plugin, "bgcolor=" + COLOR_BAD, "BAD", "bgcolor=" + COLOR_BAD, "BAD")

def writeRUndefWGood ( format, channel, bitDepth, plugin ) :
	writeHtmlInsertLigne( format, channel, bitDepth, plugin, "bgcolor=" + COLOR_UNDEF, "", "bgcolor=" + COLOR_OK, "OK")

def writeRUndefWBad ( format, channel, bitDepth, plugin ) :
	writeHtmlInsertLigne( format, channel, bitDepth, plugin, "bgcolor=" + COLOR_UNDEF, "", "bgcolor=" + COLOR_BAD, "BAD")

def writeRGoodWUndef ( format, channel, bitDepth, plugin ) :
	writeHtmlInsertLigne( format, channel, bitDepth, plugin, "bgcolor=" + COLOR_OK, "OK", "bgcolor=" + COLOR_UNDEF, "")

def writeRBadWUndef ( format, channel, bitDepth, plugin ) :
	writeHtmlInsertLigne( format, channel, bitDepth, plugin, "bgcolor=" + COLOR_BAD, "BAD", "bgcolor" + COLOR_UNDEF, "")


# function to call diff node from TuttleOFX process
# process the PSNR between 1 image and 1 generator
def diffReaderGenerator ( reader, input, generator, generatorArgs ) :
	ret = subprocess.call( TT_EXE + ' --reader tuttle.' + reader + 'reader --input ' + input + ' --reader tuttle.' + generator + ' --generator-args ' + generatorArgs + TT_OUT_FILE, shell=True )
	if( ret == 0 ) :
		print( BLUE_COLOR + 'OK : '  + reader + ' | ' + generator + ' (' + input + ', ' + generatorArgs + ')' + NC_COLOR )
		return 1
	else :
		print( RED_COLOR + 'not similar : ' + reader +'| ' + generator + '  (' + input + ', ' + generatorArgs + ')' + NC_COLOR )
		return 0

# function to call diff node from TuttleOFX process
# process the PSNR between 2 images
def diff ( reader1, reader2, input1, input2 ) :
	ret = subprocess.call( TT_EXE + ' --reader tuttle.' + reader1 + 'reader --reader tuttle.' + reader2 + 'reader --input ' + input1 + ' --input ' + input2 + TT_OUT_FILE, shell=True )
	if( ret == 0 ) :
		print( BLUE_COLOR + 'OK : '  + reader1 + ' | ' + reader2 + ' (' + input1 + ', ' + input2 + ')' + NC_COLOR )
		return 1
	else :
		print( RED_COLOR + 'not similar : ' + reader1 +'| ' + reader2 + '  (' + input1 + ', ' + input2 + ')' + NC_COLOR )
		return 0

def check1r1w ( reader1, writer1, components, bitDepth, extension ) :
	countReadOK = diff( reader1, reader1, writer1+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )
	
	if( countReadOK == 1 ) :
		if( reader1 == writer1 ) :
			writeRGoodWGood( extension, components, bitDepth, writer1 )
		else :
			writeRUndefWGood( extension, components, bitDepth, writer1 )
			writeRGoodWUndef( extension, components, bitDepth, reader1 )
		return 1
	else :
		if( reader1 == writer1 ) :
			writeRBadWBad( extension, components, bitDepth, writer1 )
		else :
			writeRUndefWBad( extension, components, bitDepth, writer1)
			writeRBadWUndef( extension, components, bitDepth, reader1)
		return 0

def check2r1w( reader1, reader2, writer1, components, bitDepth, extension ) :
	countReadOK = diff( reader1, reader1, writer1+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader2, reader2, writer1+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader1, reader2, writer1+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )
	
	if( countReadOK == 3 ) :
		if( reader1 == writer1 ) :
			writeRGoodWGood( extension, components, bitDepth, writer1 )
		else :
			writeRGoodWUndef( extension, components, bitDepth, reader1 )
		if( reader2 == writer1 ) :
			writeRGoodWGood( extension, components, bitDepth, writer1 )
		else :
			writeRGoodWUndef( extension, components, bitDepth, reader2 )
		if( writer1 != reader1 ) :
			if( writer1 != reader2 ) :
				writeRUndefWGood( extension, components, bitDepth, writer1 )
		return 1
	else :
		if( reader1 == writer1 ) :
			writeRBadWBad( extension, components, bitDepth, writer1 )
		else :
			writeRBadWUndef( extension, components, bitDepth, reader1 )
		if( reader2 == writer1 ) :
			writeRBadWBad( extension, components, bitDepth, writer1 )
		else :
			writeRBadWUndef( extension, components, bitDepth, reader2 )
		if( writer1 != reader1 ) :
			if( writer1 != reader2 ) :
				writeRUndefWBad( extension, components, bitDepth, writer1 )
		return 0


def check1r2w( reader1, writer1, writer2, components, bitDepth, extension ) :
	countReadOK = diff( reader1, reader1, writer1+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader1, reader1, writer2+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader1, reader1, writer2+'.'+components+'.'+bitDepth+'.001.'+extension, writer2+'.'+components+'.'+bitDepth+'.003.'+extension )
	
	if( countReadOK == 3 ) :
		if( reader1 == writer1 ) :
			writeRGoodWGood( extension, components, bitDepth, writer1 )
		else :
			if( reader1 == writer2 ) :
				writeRGoodWGood( extension, components, bitDepth, writer2 )
			else :
				writeRGoodWUndef( extension, components, bitDepth, reader1 )
		if( writer1 != reader1 ) :
			writeRUndefWGood( extension, components, bitDepth, writer1 )
		if( writer2 != reader1 ) :
			writeRUndefWGood( extension, components, bitDepth, writer1 )
		return 1
	else :
		if( reader1 == writer1 ) :
			writeRBadWBad( extension, components, bitDepth, writer1 )
		else :
			if( reader1 == writer2 ) :
				writeRBadWBad( extension, components, bitDepth, writer2 )
			else :
				writeRBadWUndef( extension, components, bitDepth, reader1 )
		if( writer1 != reader1 ) :
				writeRUndefWBad( extension, components, bitDepth, writer1 )
		if( writer2 != reader1 ) :
				writeRUndefWBad( extension, components, bitDepth, writer1 )
		return 0


def check2r2w( reader1, reader2, writer1, writer2, components, bitDepth, extension ) :

	countReadOK = diff( reader1, reader1, writer1+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader2, reader2, writer1+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader1, reader2, writer1+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )

	countReadOK += diff( reader1, reader1, writer2+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader2, reader2, writer2+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader1, reader2, writer2+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )

	countReadOK += diff( reader1, reader1, writer2+'.'+components+'.'+bitDepth+'.001.'+extension, writer2+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader2, reader2, writer2+'.'+components+'.'+bitDepth+'.001.'+extension, writer2+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader1, reader2, writer2+'.'+components+'.'+bitDepth+'.001.'+extension, writer2+'.'+components+'.'+bitDepth+'.003.'+extension )
	
	if( countReadOK == 9 ) :
		if( reader1 == writer1 ) :
			writeRGoodWGood( extension, components, bitDepth, writer1 )
		else :
			if( reader1 == writer2 ) :
				writeRGoodWGood( extension, components, bitDepth, writer2 )
			else :
				writeRGoodWUndef( extension, components, bitDepth, reader1 )
		if( reader2 == writer1 ) :
			writeRGoodWGood( extension, components, bitDepth, writer1 )
		else :
			if( reader2 == writer2 ) :
				writeRGoodWGood( extension, components, bitDepth, writer2 )
			else :
				writeRGoodWUndef( extension, components, bitDepth, reader2 )
		if( writer1 != reader1 ) :
			if( writer1 != reader2 ) :
				writeRUndefWGood( extension, components, bitDepth, writer1 )
		if( writer2 != reader1 ) :
			if( writer2 != reader2 ) :
				writeRUndefWGood( extension, components, bitDepth, writer1 )
		return 1
	else :
		if( reader1 == writer1 ) :
			writeRBadWBad( extension, components, bitDepth, writer1 )
		else :
			if( reader1 == writer2 ) :
				writeRBadWBad( extension, components, bitDepth, writer2 )
			else :
				writeRBadWUndef( extension, components, bitDepth, reader1 )
		if( reader2 == writer1 ) :
			writeRBadWBad( extension, components, bitDepth, writer1 )
		else :
			if( reader2 == writer2 ) :
				writeRBadWBad( extension, components, bitDepth, writer2 )
			else :
				writeRBadWUndef( extension, components, bitDepth, reader2 )
		if( writer1 != reader1 ) :
			if( writer1 != reader2 ) :
				writeRUndefWBad( extension, components, bitDepth, writer1 )
		if( writer2 != reader1 ) :
			if( writer2 != reader2 ) :
				writeRUndefWBad( extension, components, bitDepth, writer1 )
		return 0

def check3r2w( reader1, reader2, reader3, writer1, writer2, components, bitDepth, extension ) :
	diffReaderGenerator( reader2, writer2+'.'+components+'.'+bitDepth+'.001.'+extension, GENERATOR, GENERATOR_ARGS )

	countReadOK = diff( reader1, reader1, writer1+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader2, reader2, writer1+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader3, reader3, writer1+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader3, reader1, writer1+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader2, reader1, writer1+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader2, reader3, writer1+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )

	countReadOK += diff( reader1, reader1, writer2+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader2, reader2, writer2+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader3, reader3, writer2+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader3, reader1, writer2+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader2, reader1, writer2+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader2, reader3, writer2+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )
	
	countReadOK += diff( reader1, reader1, writer2+'.'+components+'.'+bitDepth+'.001.'+extension, writer2+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader2, reader2, writer2+'.'+components+'.'+bitDepth+'.001.'+extension, writer2+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader3, reader3, writer2+'.'+components+'.'+bitDepth+'.001.'+extension, writer2+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader3, reader1, writer2+'.'+components+'.'+bitDepth+'.001.'+extension, writer1+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader2, reader1, writer2+'.'+components+'.'+bitDepth+'.001.'+extension, writer2+'.'+components+'.'+bitDepth+'.003.'+extension )
	countReadOK += diff( reader2, reader3, writer2+'.'+components+'.'+bitDepth+'.001.'+extension, writer2+'.'+components+'.'+bitDepth+'.003.'+extension )

	if( countReadOK == 18 ) :
		if( reader1 == writer1 ) :
			writeRGoodWGood( extension, components, bitDepth, writer1 )
		else :
			if( reader1 == writer2 ) :
				writeRGoodWGood( extension, components, bitDepth, writer2 )
			else :
				writeRGoodWUndef( extension, components, bitDepth, reader1 )
		if( reader2 == writer1 ) :
			writeRGoodWGood( extension, components, bitDepth, writer1 )
		else :
			if( reader2 == writer2 ) :
				writeRGoodWGood( extension, components, bitDepth, writer2 )
			else :
				writeRGoodWUndef( extension, components, bitDepth, reader2 )
		if( reader3 == writer1 ) :
			writeRGoodWGood( extension, components, bitDepth, writer1 )
		else :
			if( reader3 == writer2 ) :
				writeRGoodWGood( extension, components, bitDepth, writer2 )
			else :
				writeRGoodWUndef( extension, components, bitDepth, reader3 )
		if( writer1 != reader1 ) :
			if( writer1 != reader2 ) :
				if( writer1 != reader3 ) :
					writeRUndefWGood( extension, components, bitDepth, writer1 )
		return 1
	else :
		if( reader1 == writer1 ) :
			writeRBadWBad( extension, components, bitDepth, writer1 )
		else :
			if( reader1 == writer2 ) :
				writeRBadWBad( extension, components, bitDepth, writer2 )
			else :
				writeRBadWUndef( extension, components, bitDepth, reader1 )
		if( reader2 == writer1 ) :
			writeRBadWBad( extension, components, bitDepth, writer1 )
		else :
			if( reader2 == writer2 ) :
				writeRBadWBad( extension, components, bitDepth, writer2 )
			else :
				writeRBadWUndef( extension, components, bitDepth, reader2 )
		if( reader3 == writer1 ) :
			writeRBadWBad( extension, components, bitDepth, writer1 )
		else :
			if( reader3 == writer2 ) :
				writeRBadWBad( extension, components, bitDepth, writer2 )
			else :
				writeRBadWUndef( extension, components, bitDepth, reader3 )
		if( writer1 != reader1 ) :
			if( writer1 != reader2 ) :
				if( writer1 != reader3 ) :
					writeRUndefWBad( extension, components, bitDepth, writer1 )
		return 0


print( "***********************************************" )
print( "***************       DIFF       **************" )
print( "***********************************************" )

writeHtmlHeader()

def checkPNG():
	print( "****************       PNG      ***************" )
	startFormat( "PNG" )
	formatIsValid=0
	formatIsValid += check3r2w( 'png', 'oiio', 'imagemagick', 'png', 'oiio', 'gray',  '8bits', 'png' )
	formatIsValid += check3r2w( 'png', 'oiio', 'imagemagick', 'png', 'oiio', 'gray', '16bits', 'png' )
	formatIsValid += check3r2w( 'png', 'oiio', 'imagemagick', 'png', 'oiio', 'rgb',   '8bits', 'png' )
	formatIsValid += check3r2w( 'png', 'oiio', 'imagemagick', 'png', 'oiio', 'rgb',  '16bits', 'png' )
	formatIsValid += check2r2w( 'png', 'oiio', 'png', 'oiio', 'rgba',  '8bits', 'png' )
	formatIsValid += check2r2w( 'png', 'oiio', 'png', 'oiio', 'rgba', '16bits', 'png' )
	endFormat( "PNG", formatIsValid == 6 )

def checkJPEG():
	print( "****************      JPEG      ***************" )
	startFormat( "JPEG" )
	formatIsValid=0
	formatIsValid += check3r2w( 'imagemagick', 'jpeg', 'oiio', 'jpeg', 'oiio', 'rgb',  '8bits', 'jpg' )
	endFormat( "Jpeg", formatIsValid == 1 )

def checkDPX():
	print( "****************       DPX      ***************" )
	startFormat( "DPX" )
	formatIsValid=0
	formatIsValid += check1r1w( 'imagemagick', 'dpx', 'gray',  '8bits', 'dpx' )
	formatIsValid += check1r1w( 'imagemagick', 'dpx', 'gray', '10bits', 'dpx' )
	formatIsValid += check1r1w( 'imagemagick', 'dpx', 'gray', '12bits', 'dpx' )
	formatIsValid += check1r1w( 'imagemagick', 'dpx', 'gray', '16bits', 'dpx' )
	
	formatIsValid += check2r2w( 'imagemagick', 'oiio', 'dpx', 'oiio', 'rgb',  '8bits', 'dpx' )
	formatIsValid += check2r2w( 'imagemagick', 'oiio', 'dpx', 'oiio', 'rgb', '10bits', 'dpx' )
	formatIsValid += check2r2w( 'imagemagick', 'oiio', 'dpx', 'oiio', 'rgb', '12bits', 'dpx' )
	formatIsValid += check2r2w( 'imagemagick', 'oiio', 'dpx', 'oiio', 'rgb', '16bits', 'dpx' )
	
	formatIsValid += check2r2w( 'imagemagick', 'oiio', 'dpx', 'oiio', 'rgba',  '8bits', 'dpx' )
	formatIsValid += check2r2w( 'imagemagick', 'oiio', 'dpx', 'oiio', 'rgba', '10bits', 'dpx' )
	formatIsValid += check2r2w( 'imagemagick', 'oiio', 'dpx', 'oiio', 'rgba', '12bits', 'dpx' )
	formatIsValid += check2r2w( 'imagemagick', 'oiio', 'dpx', 'oiio', 'rgba', '16bits', 'dpx' )
	
	formatIsValid += check1r1w( 'oiio', 'dpx', 'abgr',  '8bits', 'dpx' )
	formatIsValid += check1r1w( 'oiio', 'dpx', 'abgr', '10bits', 'dpx' )
	formatIsValid += check1r1w( 'oiio', 'dpx', 'abgr', '12bits', 'dpx' )
	formatIsValid += check1r1w( 'oiio', 'dpx', 'abgr', '16bits', 'dpx' )
	endFormat( "DPX", formatIsValid == 16 )

def checkEXR():
	print( "****************       EXR      ***************" )
	startFormat( "EXR" )
	formatIsValid=0
	formatIsValid += check2r2w( 'exr', 'oiio', 'exr', 'oiio', 'gray', '16float', 'exr' )
	formatIsValid += check2r2w( 'exr', 'oiio', 'exr', 'oiio', 'gray', '32bits',  'exr' )
	formatIsValid += check2r2w( 'exr', 'oiio', 'exr', 'oiio', 'gray', '32float', 'exr' )
	
	formatIsValid += check2r2w( 'exr', 'oiio', 'exr', 'oiio', 'rgb', '16float', 'exr' )
	formatIsValid += check2r2w( 'exr', 'oiio', 'exr', 'oiio', 'rgb', '32bits',  'exr' )
	formatIsValid += check2r2w( 'exr', 'oiio', 'exr', 'oiio', 'rgb', '32float', 'exr' )
	
	formatIsValid += check2r2w( 'exr', 'oiio', 'exr', 'oiio', 'rgba', '16float', 'exr' )
	formatIsValid += check2r2w( 'exr', 'oiio', 'exr', 'oiio', 'rgba', '32bits',  'exr' )
	formatIsValid += check2r2w( 'exr', 'oiio', 'exr', 'oiio', 'rgba', '32float', 'exr' )
	endFormat( "EXR", formatIsValid == 9 )

def checkJPEG2000():
	print( "****************   JPEG 2000    ***************" )
	startFormat( "JPEG2000" )
	formatIsValid=0
	formatIsValid += check1r1w( 'jpeg2000', 'jpeg2000', 'rgb',  '8bits', 'j2k' )
	formatIsValid += check1r1w( 'jpeg2000', 'jpeg2000', 'rgb', '12bits', 'j2k' )
	formatIsValid += check1r1w( 'jpeg2000', 'jpeg2000', 'rgb', '16bits', 'j2k' )
	endFormat( "JPEG2000", formatIsValid == 3 )

def checkTGA():
	print( "****************      TGA      ***************" )
	startFormat( "TGA" )
	formatIsValid=0
	formatIsValid += check2r1w( 'oiio', 'imagemagick', 'oiio', 'gray', '8bits', 'tga' )
	formatIsValid += check2r1w( 'oiio', 'imagemagick', 'oiio', 'rgb',  '8bits', 'tga' )
	formatIsValid += check2r1w( 'oiio', 'imagemagick', 'oiio', 'rgba', '8bits', 'tga' )
	endFormat( "TGA", formatIsValid == 3 )

def checkTIF():
	print( "****************      TIF      ***************" )
	startFormat( "TIF" )
	formatIsValid=0
	formatIsValid += check2r1w( 'oiio', 'imagemagick', 'oiio', 'gray', '8bits', 'tif' )
	formatIsValid += check2r1w( 'oiio', 'imagemagick', 'oiio', 'gray', '16bits', 'tif' )
	
	formatIsValid += check2r1w( 'oiio', 'imagemagick', 'oiio', 'rgb',  '8bits', 'tif' )
	formatIsValid += check2r1w( 'oiio', 'imagemagick', 'oiio', 'rgb',  '16bits', 'tif' )
	
	formatIsValid += check1r1w( 'oiio', 'oiio', 'rgba', '8bits', 'tif' )
	formatIsValid += check1r1w( 'oiio', 'oiio', 'rgba', '16bits', 'tif' )
	endFormat( "TIF", formatIsValid == 6 )

def checkPPM():
	print( "****************      PPM      ***************" )
	startFormat( "PPM" )
	formatIsValid=0
	formatIsValid += check2r1w( 'oiio', 'imagemagick', 'oiio', 'gray', '8bits', 'ppm' )
	formatIsValid += check2r1w( 'oiio', 'imagemagick', 'oiio', 'gray', '16bits', 'ppm' )
	
	formatIsValid += check2r1w( 'oiio', 'imagemagick', 'oiio', 'rgb',  '8bits', 'ppm' )
	formatIsValid += check2r1w( 'oiio', 'imagemagick', 'oiio', 'rgb',  '16bits', 'ppm' )
	
	formatIsValid += check2r1w( 'oiio', 'imagemagick', 'oiio', 'rgba', '8bits', 'ppm' )
	formatIsValid += check2r1w( 'oiio', 'imagemagick', 'oiio', 'rgba', '16bits', 'ppm' )
	endFormat( "PPM", formatIsValid == 6 )

def checkSGI():
	print( "****************      SGI      ***************" )
	startFormat( "SGI" )
	formatIsValid=0
	formatIsValid += check2r1w( 'imagemagick', 'oiio', 'oiio', 'gray', '8bits',  'sgi' )
	formatIsValid += check2r1w( 'imagemagick', 'oiio', 'oiio', 'gray', '16bits', 'sgi' )
	formatIsValid += check2r1w( 'imagemagick', 'oiio', 'oiio', 'rgb',  '8bits',  'sgi' )
	formatIsValid += check2r1w( 'imagemagick', 'oiio', 'oiio', 'rgb',  '16bits', 'sgi' )
	formatIsValid += check2r1w( 'imagemagick', 'oiio', 'oiio', 'rgba', '8bits',  'sgi' )
	formatIsValid += check2r1w( 'imagemagick', 'oiio', 'oiio', 'rgba', '16bits', 'sgi' )
	endFormat( "SGI", formatIsValid == 6 )

def checkHDR():
	print( "****************      HDR      ***************" )
	startFormat( "HDR" )
	formatIsValid=0
	formatIsValid += check2r1w( 'oiio', 'imagemagick', 'oiio', 'rgb',  '8bits',  'hdr' )
	formatIsValid += check2r1w( 'oiio', 'imagemagick', 'oiio', 'rgb', '16bits',  'hdr' )
	formatIsValid += check1r1w( 'oiio', 'oiio', 'rgb', '16float', 'hdr' )
	formatIsValid += check1r1w( 'oiio', 'oiio', 'rgb', '32bits',  'hdr' )
	formatIsValid += check1r1w( 'oiio', 'oiio', 'rgb', '32float', 'hdr' )
	endFormat( "HDR", formatIsValid == 5 )

def checkRGBE():
	print( "****************      RGBE     ***************" )
	startFormat( "RGBE" )
	formatIsValid=0
	formatIsValid += check2r1w( 'oiio', 'imagemagick', 'oiio', 'rgb',  '8bits', 'rgbe' )
	formatIsValid += check2r1w( 'oiio', 'imagemagick', 'oiio', 'rgb', '16bits', 'rgbe' )
	formatIsValid += check1r1w( 'oiio', 'oiio', 'rgb', '16float', 'rgbe' )
	formatIsValid += check1r1w( 'oiio', 'oiio', 'rgb', '32bits',  'rgbe' )
	formatIsValid += check1r1w( 'oiio', 'oiio', 'rgb', '32float', 'rgbe' )
	endFormat( "RGBE", formatIsValid == 5 )

if ( len(sys.argv) == 1 ):
	checkPNG()
	checkJPEG()
	checkDPX()
	checkEXR()
	checkJPEG2000()
	checkTGA()
	checkTIF()
	checkPPM()
	checkSGI()
	checkHDR()
	checkRGBE()
else:
	for arg in range( len(sys.argv)-1) :
		if( sys.argv[arg+1].lower() == "png" ) :
			checkPNG()
		if( sys.argv[arg+1].lower() == "jpg" ) :
			checkJPEG()
		if( sys.argv[arg+1].lower() == "jpeg" ) :
			checkJPEG()
		if( sys.argv[arg+1].lower() == "dpx" ) :
			checkDPX()
		if( sys.argv[arg+1].lower() == "exr" ) :
			checkEXR()
		if( sys.argv[arg+1].lower() == "jpeg2000" ) :
			checkJPEG2000()
		if( sys.argv[arg+1].lower() == "tga" ) :
			checkTGA()
		if( sys.argv[arg+1].lower() == "tif" ) :
			checkTIF()
		if( sys.argv[arg+1].lower() == "ppm" ) :
			checkPPM()
		if( sys.argv[arg+1].lower() == "sgi" ) :
			checkSGI()
		if( sys.argv[arg+1].lower() == "hdr" ) :
			checkHDR()
		if( sys.argv[arg+1].lower() == "rgbe" ) :
			checkRGBE()

writeHtmlFooter()
htmlF.close()
cssF.close()
