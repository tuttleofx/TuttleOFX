#!/bin/sh

red='\e[0;31m'
RED='\e[1;31m'
blue='\e[0;34m'
BLUE='\e[1;34m'
cyan='\e[0;36m'
CYAN='\e[1;36m'
NC='\e[0m'  # No Color


export TUTTLE_ROOT=`pwd`/../../../../dist/linux/release
export OFX_PLUGIN_PATH=$TUTTLE_ROOT/plugin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$TUTTLE_ROOT/lib
export PATH=$PATH:$TUTTLE_ROOT/bin
export SAM_PATH=$TUTTLE_ROOT/bin

if [ ! -d "$TUTTLE_ROOT/.tuttleofxTest" ]; then
   mkdir $TUTTLE_ROOT/.tuttleofxTest
fi
export TUTTLE_HOME=$TUTTLE_ROOT/.tuttleofxTest

resultFile=result.html
cssFile=style.css

rm $resultFile
rm $cssFile

date=`date +"%-d %B %Y"`

# write header in html file

function writeHtmlHeader {
	echo "<html>" >>  $resultFile
## HEAD
	echo '<head>' >>  $resultFile
	echo '<link rel="stylesheet" type="text/css" href="'$cssFile'">' >> $resultFile
	echo '<link href="http://ajax.googleapis.com/ajax/libs/jqueryui/1.8/themes/base/jquery-ui.css" rel="stylesheet" type="text/css"/>' >> $resultFile
	echo '<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.5/jquery.min.js"></script>' >> $resultFile
	echo '<script src="http://ajax.googleapis.com/ajax/libs/jqueryui/1.8/jquery-ui.min.js"></script>' >> $resultFile
	echo '<script>' >> $resultFile
	echo '$(document).ready(function() {' >> $resultFile
	echo '	$("#accordion").accordion({ collapsible:true, active:false, autoHeight:false });' >> $resultFile
	echo '});' >> $resultFile

	echo '</script>' >> $resultFile
	echo '</head>' >> $resultFile
## BODY
	echo '<body>' >> $resultFile
	echo '<a style="color:#fff">update : ' $date '</a><br/>' >> $resultFile
	echo '<div id="accordion">' >> $resultFile

	echo 'body {' >> $cssFile
	echo 'background-color:black;' >> $cssFile
	echo '}' >> $cssFile



}

function startFormat {
	formatIsOK=1
	echo '	<h3 style="background:none;" id="'$1'"><a href="#">'$1'</a></h3>' >> $resultFile
	echo '	<div>' >> $resultFile
	echo "		<table border='0' cellpadding=5 >" >> $resultFile
	echo "		<tr><td bgcolor=#E4E4E4>Format</td><td bgcolor=#E4E4E4>Channels</td><td bgcolor=#E4E4E4>Bit Depth</td><td bgcolor=#E4E4E4>Plugin</td><td bgcolor=#E4E4E4>Read</td><td bgcolor=#E4E4E4>Write</td><tr>" >>  $resultFile
}

function writeHtmlInsertLigne {
	echo "<tr>" >>  $resultFile
	echo "<td>" $1 >>  $resultFile
	echo "</td>" >>  $resultFile
	echo "<td>" $2 >>  $resultFile
	echo "</td>" >>  $resultFile
	echo "<td>" $3 >>  $resultFile
	echo "</td>" >>  $resultFile
	echo "<td>" $4 >>  $resultFile
	echo "</td>" >>  $resultFile
	echo "<td "$5" >" $6 >>  $resultFile
	echo "</td>" >>  $resultFile
	echo "<td "$7">" $8 >>  $resultFile
	echo "</td>" >>  $resultFile
	echo "</tr>" >>  $resultFile
}

function endFormat {
	echo '		</table>' >>  $resultFile
	echo '	</div>' >> $resultFile

	echo '#'$1' { ' >> $cssFile
	if [ $formatIsOK -eq "1" ]; then
		echo 'background-color:#94D64D !important;' >> $cssFile
	else
		echo 'background-color:#D67E55 !important;' >> $cssFile
	fi
	echo '}'>> $cssFile

}

function writeHtmlFooter {

	echo "</body>" >>  $resultFile
	echo "</html>" >>  $resultFile
}

function writeRGoodWGood {
	writeHtmlInsertLigne $1 $2 $3 $4 "bgcolor=#94D64D" "OK" "bgcolor=#94D64D" "OK"
}

function writeRGoodWBad {
	writeHtmlInsertLigne $1 $2 $3 $4 "bgcolor=#94D64D" "OK" "bgcolor=#D67E55" "BAD"
}

function writeRBadWGood {
	writeHtmlInsertLigne $1 $2 $3 $4 "bgcolor=#D67E55" "BAD" "bgcolor=#94D64D" "OK"
}

function writeRBadWBad {
	writeHtmlInsertLigne $1 $2 $3 $4 "bgcolor=#D67E55" "BAD" "bgcolor=#D67E55" "BAD"
}

function writeRUndefWGood {
	writeHtmlInsertLigne $1 $2 $3 $4 "bgcolor=#E4E4E4" "" "bgcolor=#94D64D" "OK"
}

function writeRUndefWBad {
	writeHtmlInsertLigne $1 $2 $3 $4 "bgcolor=#E4E4E4" "" "bgcolor=#D67E55" "BAD"
}

function writeRGoodWUndef {
	writeHtmlInsertLigne $1 $2 $3 $4 "bgcolor=#94D64D" "OK" "bgcolor=#E4E4E4" ""
}

function writeRBadWUndef {
	writeHtmlInsertLigne $1 $2 $3 $4 "bgcolor=#D67E55" "BAD" "bgcolor=#E4E4E4" ""
}

echo "***********************************************"
echo "****************      DIFF     ***************"
echo "***********************************************"



function clearReadValues {
	countReadOK=0
	countReadBAD=0
}

# function to call diff node from TuttleOFX process
# process the PSNR between 2 images
function diff {
	sam-diff --reader tuttle.$1reader --reader tuttle.$2reader --inputs $3 --inputs $4 >> diff.txt
	if [ "$?" -eq "0" ]; then
		echo -e "$BLUE""$1 | $2 is OK ($3, $4)""$NC" | column -t
		countReadOK=`expr $countReadOK + 1`
		return 1
	else
		echo -e "$RED""$1\t| $2 not similar ($3, $4)  "`grep "diff =" diff.txt | tail -1`"$NC" | column -t
		countReadBAD=`expr $countReadBAD + 1`
		formatIsOK=0
		return 0
	fi
}


writeHtmlHeader
clearReadValues

function check1r1w {
	reader1=$1
	writer1=$2
	components=$3
	bitDepth=$4
	extension=$5

	diff $reader1 $reader1 $writer1.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension
	
	if [ $countReadOK -eq 1 ]; then
		if [ $reader1 == $writer1 ]; then
			writeRGoodWGood "$extension" "$components" "$bitDepth" "$writer1"
		else
			writeRUndefWGood "$extension" "$components" "$bitDepth" "$writer1"
			writeRGoodWUndef "$extension" "$components" "$bitDepth" "$reader1"
		fi
	else
		if [ $reader1 == $writer1 ]; then
			writeRBadWBad "$extension" "$components" "$bitDepth" "$writer1"
		else
			writeRUndefWBad "$extension" "$components" "$bitDepth" "$writer1"
			writeRBadWUndef "$extension" "$components" "$bitDepth" "$reader1"
		fi
	fi
	clearReadValues
}

function check2r1w {
	reader1=$1
	reader2=$2
	writer1=$3
	components=$4
	bitDepth=$5
	extension=$6

	diff $reader1 $reader1 $writer1.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension
	diff $reader2 $reader2 $writer1.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension
	diff $reader1 $reader2 $writer1.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension
	
	if [ $countReadOK -eq 3 ]; then
		if [ $reader1 == $writer1 ]; then
			writeRGoodWGood "$extension" "$components" "$bitDepth" "$writer1"
		else
			writeRGoodWUndef "$extension" "$components" "$bitDepth" "$reader1"
		fi
		if [ $reader2 == $writer1 ]; then
			writeRGoodWGood "$extension" "$components" "$bitDepth" "$writer1"
		else
			writeRGoodWUndef "$extension" "$components" "$bitDepth" "$reader2"
		fi
		if [ $writer1 != $reader1 ]; then
			if [ $writer1 != $reader2 ]; then
				writeRUndefWGood "$extension" "$components" "$bitDepth" "$writer1"
			fi
		fi
	else
		if [ $reader1 == $writer1 ]; then
			writeRBadWBad "$extension" "$components" "$bitDepth" "$writer1"
		else
			writeRBadWUndef "$extension" "$components" "$bitDepth" "$reader1"
		fi
		if [ $reader2 == $writer1 ]; then
			writeRBadWBad "$extension" "$components" "$bitDepth" "$writer1"
		else
			writeRBadWUndef "$extension" "$components" "$bitDepth" "$reader2"
		fi
		if [ $writer1 != $reader1 ]; then
			if [ $writer1 != $reader2 ]; then
				writeRUndefWBad "$extension" "$components" "$bitDepth" "$writer1"
			fi
		fi
	fi
	clearReadValues
}

function check1r2w {
	reader1=$1
	writer1=$2
	writer2=$3
	components=$4
	bitDepth=$5
	extension=$6

	diff $reader1 $reader1 $writer1.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension
	diff $reader1 $reader1 $writer2.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension
	diff $reader1 $reader1 $writer2.$components.$bitDepth.001.$extension  $writer2.$components.$bitDepth.003.$extension
	
	if [ $countReadOK -eq 3 ]; then
		if [ $reader1 == $writer1 ]; then
			writeRGoodWGood "$extension" "$components" "$bitDepth" "$writer1"
		else
			if [ $reader1 == $writer2 ]; then
				writeRGoodWGood "$extension" "$components" "$bitDepth" "$writer2"
			else
				writeRGoodWUndef "$extension" "$components" "$bitDepth" "$reader1"
			fi
		fi
		if [ $writer1 != $reader1 ]; then
			writeRUndefWGood "$extension" "$components" "$bitDepth" "$writer1"
		fi
		if [ $writer2 != $reader1 ]; then
			writeRUndefWGood "$extension" "$components" "$bitDepth" "$writer1"
		fi
	else
		if [ $reader1 == $writer1 ]; then
			writeRBadWBad "$extension" "$components" "$bitDepth" "$writer1"
		else
			if [ $reader1 == $writer2 ]; then
				writeRBadWBad "$extension" "$components" "$bitDepth" "$writer2"
			else
				writeRBadWUndef "$extension" "$components" "$bitDepth" "$reader1"
			fi
		fi
		if [ $writer1 != $reader1 ]; then
				writeRUndefWBad "$extension" "$components" "$bitDepth" "$writer1"
		fi
		if [ $writer2 != $reader1 ]; then
				writeRUndefWBad "$extension" "$components" "$bitDepth" "$writer1"
		fi
	fi
	clearReadValues
}

function check2r2w {
	reader1=$1
	reader2=$2
	writer1=$3
	writer2=$4
	components=$5
	bitDepth=$6
	extension=$7

	diff $reader1 $reader1 $writer1.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension
	diff $reader2 $reader2 $writer1.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension
	diff $reader1 $reader2 $writer1.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension

	diff $reader1 $reader1 $writer2.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension
	diff $reader2 $reader2 $writer2.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension
	diff $reader1 $reader2 $writer2.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension

	diff $reader1 $reader1 $writer2.$components.$bitDepth.001.$extension  $writer2.$components.$bitDepth.003.$extension
	diff $reader2 $reader2 $writer2.$components.$bitDepth.001.$extension  $writer2.$components.$bitDepth.003.$extension
	diff $reader1 $reader2 $writer2.$components.$bitDepth.001.$extension  $writer2.$components.$bitDepth.003.$extension
	
	if [ $countReadOK -eq 9 ]; then
		if [ $reader1 == $writer1 ]; then
			writeRGoodWGood "$extension" "$components" "$bitDepth" "$writer1"
		else
			if [ $reader1 == $writer2 ]; then
				writeRGoodWGood "$extension" "$components" "$bitDepth" "$writer2"
			else
				writeRGoodWUndef "$extension" "$components" "$bitDepth" "$reader1"
			fi
		fi
		if [ $reader2 == $writer1 ]; then
			writeRGoodWGood "$extension" "$components" "$bitDepth" "$writer1"
		else
			if [ $reader2 == $writer2 ]; then
				writeRGoodWGood "$extension" "$components" "$bitDepth" "$writer2"
			else
				writeRGoodWUndef "$extension" "$components" "$bitDepth" "$reader2"
			fi
		fi
		if [ $writer1 != $reader1 ]; then
			if [ $writer1 != $reader2 ]; then
				writeRUndefWGood "$extension" "$components" "$bitDepth" "$writer1"
			fi
		fi
		if [ $writer2 != $reader1 ]; then
			if [ $writer2 != $reader2 ]; then
				writeRUndefWGood "$extension" "$components" "$bitDepth" "$writer1"
			fi
		fi
	else
		if [ $reader1 == $writer1 ]; then
			writeRBadWBad "$extension" "$components" "$bitDepth" "$writer1"
		else
			if [ $reader1 == $writer2 ]; then
				writeRBadWBad "$extension" "$components" "$bitDepth" "$writer2"
			else
				writeRBadWUndef "$extension" "$components" "$bitDepth" "$reader1"
			fi
		fi
		if [ $reader2 == $writer1 ]; then
			writeRBadWBad "$extension" "$components" "$bitDepth" "$writer1"
		else
			if [ $reader2 == $writer2 ]; then
				writeRBadWBad "$extension" "$components" "$bitDepth" "$writer2"
			else
				writeRBadWUndef "$extension" "$components" "$bitDepth" "$reader2"
			fi
		fi
		if [ $writer1 != $reader1 ]; then
			if [ $writer1 != $reader2 ]; then
				writeRUndefWBad "$extension" "$components" "$bitDepth" "$writer1"
			fi
		fi
		if [ $writer2 != $reader1 ]; then
			if [ $writer2 != $reader2 ]; then
				writeRUndefWBad "$extension" "$components" "$bitDepth" "$writer1"
			fi
		fi
	fi
	clearReadValues
}

function check3r2w {

	reader1=$1
	reader2=$2
	reader3=$3
	writer1=$4
	writer2=$5
	
	components=$6
	bitDepth=$7
	extension=$8

	diff $reader1 $reader1 $writer1.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension
	diff $reader2 $reader2 $writer1.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension
	diff $reader3 $reader3 $writer1.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension
	diff $reader3 $reader1 $writer1.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension
	diff $reader2 $reader1 $writer1.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension
	diff $reader2 $reader3 $writer1.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension

	diff $reader1 $reader1 $writer2.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension
	diff $reader2 $reader2 $writer2.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension
	diff $reader3 $reader3 $writer2.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension
	diff $reader3 $reader1 $writer2.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension
	diff $reader2 $reader1 $writer2.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension
	diff $reader2 $reader3 $writer2.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension
	
	diff $reader1 $reader1 $writer2.$components.$bitDepth.001.$extension  $writer2.$components.$bitDepth.003.$extension
	diff $reader2 $reader2 $writer2.$components.$bitDepth.001.$extension  $writer2.$components.$bitDepth.003.$extension
	diff $reader3 $reader3 $writer2.$components.$bitDepth.001.$extension  $writer2.$components.$bitDepth.003.$extension
	diff $reader3 $reader1 $writer2.$components.$bitDepth.001.$extension  $writer1.$components.$bitDepth.003.$extension
	diff $reader2 $reader1 $writer2.$components.$bitDepth.001.$extension  $writer2.$components.$bitDepth.003.$extension
	diff $reader2 $reader3 $writer2.$components.$bitDepth.001.$extension  $writer2.$components.$bitDepth.003.$extension
	
	if [ $countReadOK -eq 18 ]; then
		if [ $reader1 == $writer1 ]; then
			writeRGoodWGood "$extension" "$components" "$bitDepth" "$writer1"
		else
			if [ $reader1 == $writer2 ]; then
				writeRGoodWGood "$extension" "$components" "$bitDepth" "$writer2"
			else
				writeRGoodWUndef "$extension" "$components" "$bitDepth" "$reader1"
			fi
		fi
		if [ $reader2 == $writer1 ]; then
			writeRGoodWGood "$extension" "$components" "$bitDepth" "$writer1"
		else
			if [ $reader2 == $writer2 ]; then
				writeRGoodWGood "$extension" "$components" "$bitDepth" "$writer2"
			else
				writeRGoodWUndef "$extension" "$components" "$bitDepth" "$reader2"
			fi
		fi
		if [ $reader3 == $writer1 ]; then
			writeRGoodWGood "$extension" "$components" "$bitDepth" "$writer1"
		else
			if [ $reader3 == $writer2 ]; then
				writeRGoodWGood "$extension" "$components" "$bitDepth" "$writer2"
			else
				writeRGoodWUndef "$extension" "$components" "$bitDepth" "$reader3"
			fi
		fi
		if [ $writer1 != $reader1 ]; then
			if [ $writer1 != $reader2 ]; then
				if [ $writer1 != $reader3 ]; then
					writeRUndefWGood "$extension" "$components" "$bitDepth" "$writer1"
				fi
			fi
		fi
	else
	if [ $reader1 == $writer1 ]; then
		writeRBadWBad "$extension" "$components" "$bitDepth" "$writer1"
	else
		if [ $reader1 == $writer2 ]; then
			writeRBadWBad "$extension" "$components" "$bitDepth" "$writer2"
		else
			writeRBadWUndef "$extension" "$components" "$bitDepth" "$reader1"
		fi
	fi
	if [ $reader2 == $writer1 ]; then
		writeRBadWBad "$extension" "$components" "$bitDepth" "$writer1"
	else
		if [ $reader2 == $writer2 ]; then
			writeRBadWBad "$extension" "$components" "$bitDepth" "$writer2"
		else
			writeRBadWUndef "$extension" "$components" "$bitDepth" "$reader2"
		fi
	fi
	if [ $reader3 == $writer1 ]; then
		writeRBadWBad "$extension" "$components" "$bitDepth" "$writer1"
	else
		if [ $reader3 == $writer2 ]; then
			writeRBadWBad "$extension" "$components" "$bitDepth" "$writer2"
		else
			writeRBadWUndef "$extension" "$components" "$bitDepth" "$reader3"
		fi
	fi
	if [ $writer1 != $reader1 ]; then
		if [ $writer1 != $reader2 ]; then
			if [ $writer1 != $reader3 ]; then
				writeRUndefWBad "$extension" "$components" "$bitDepth" "$writer1"
			fi
		fi
	fi
	fi
	clearReadValues
}

echo "****************       PNG      ***************"
startFormat PNG
check3r2w png oiio imagemagick png oiio gray  8bits png
check3r2w png oiio imagemagick png oiio gray 16bits png
check3r2w png oiio imagemagick png oiio rgb   8bits png
check3r2w png oiio imagemagick png oiio rgb  16bits png
check3r2w png oiio imagemagick png oiio rgba  8bits png
check3r2w png oiio imagemagick png oiio rgba 16bits png
endFormat PNG

echo "****************      JPEG      ***************"
startFormat Jpeg
check3r2w imagemagick jpeg oiio jpeg oiio rgb  8bits jpg
endFormat Jpeg

echo "****************       DPX      ***************"
startFormat DPX
check1r1w imagemagick dpx gray  8bits dpx
check1r1w imagemagick dpx gray 10bits dpx
check1r1w imagemagick dpx gray 12bits dpx
check1r1w imagemagick dpx gray 16bits dpx

check2r2w imagemagick oiio dpx oiio rgb  8bits dpx
check2r2w imagemagick oiio dpx oiio rgb 10bits dpx
check2r2w imagemagick oiio dpx oiio rgb 12bits dpx
check2r2w imagemagick oiio dpx oiio rgb 16bits dpx

check2r2w imagemagick oiio dpx oiio rgba 8bits dpx
check2r2w imagemagick oiio dpx oiio rgba 10bits dpx
check2r2w imagemagick oiio dpx oiio rgba 12bits dpx
check2r2w imagemagick oiio dpx oiio rgba 16bits dpx

check2r2w imagemagick oiio dpx oiio abgr 8bits dpx
check2r2w imagemagick oiio dpx oiio abgr 10bits dpx
check2r2w imagemagick oiio dpx oiio abgr 12bits dpx
check2r2w imagemagick oiio dpx oiio abgr 16bits dpx
endFormat DPX

echo "****************       EXR      ***************"
startFormat EXR
check2r2w exr oiio exr oiio gray 16float exr
check2r2w exr oiio exr oiio gray 32bits  exr
check2r2w exr oiio exr oiio gray 32float exr

check2r2w exr oiio exr oiio rgb 16float exr
check2r2w exr oiio exr oiio rgb 32bits  exr
check2r2w exr oiio exr oiio rgb 32float exr

check2r2w exr oiio exr oiio rgba 16float exr
check2r2w exr oiio exr oiio rgba 32bits  exr
check2r2w exr oiio exr oiio rgba 32float exr
endFormat EXR

echo "****************   JPEG 2000    ***************"
startFormat JPEG2000
check1r1w jpeg2000 jpeg2000 rgb  8bits j2k
check1r1w jpeg2000 jpeg2000 rgb 12bits j2k
check1r1w jpeg2000 jpeg2000 rgb 16bits j2k
endFormat JPEG2000

echo "****************      TGA      ***************"
startFormat TGA
check2r1w oiio imagemagick oiio rgb  8bits tga
check2r1w oiio imagemagick oiio rgb 12bits tga
check2r1w oiio imagemagick oiio rgb 16bits tga
endFormat TGA

echo "****************      SGI      ***************"
startFormat SGI
check2r1w imagemagick oiio oiio gray 8bits  sgi
check2r1w imagemagick oiio oiio gray 16bits sgi
check2r1w imagemagick oiio oiio rgb  8bits  sgi
check2r1w imagemagick oiio oiio rgb  16bits sgi
check2r1w imagemagick oiio oiio rgba 8bits  sgi
check2r1w imagemagick oiio oiio rgba 16bits sgi
endFormat SGI

echo "****************      HDR      ***************"
startFormat HDR
check2r1w oiio imagemagick oiio rgb  8bits  hdr
check2r1w oiio imagemagick oiio rgb 16bits  hdr
check1r1w oiio oiio rgb 16float hdr
check1r1w oiio oiio rgb 32bits  hdr
check1r1w oiio oiio rgb 32float hdr
endFormat HDR
writeHtmlFooter
