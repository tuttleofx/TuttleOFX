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

function checkComponentsBitDepth1 {
	reader=$1
	writer=$2
	components=$3
	bitDepth=$4
	extension=$5
	diff $reader $reader $components.$bitDepth.001.$extension  $components.$bitDepth.003.$extension
	
	if [ $countReadOK -eq 1 ]; then
		writeRUndefWGood "$extension" "$components" "$bitDepth" "$writer"
		writeRGoodWUndef "$extension" "$components" "$bitDepth" "$reader"
	else
		writeRUndefWBad "$extension" "$components" "$bitDepth" "$writer"
		writeRBadWUndef "$extension" "$components" "$bitDepth" "$reader"
	fi
	clearReadValues
}

function checkComponentsBitDepth {
	diff oiio        oiio        $2.$3.001.$4          $2.$3.003.$4
	diff imagemagick imagemagick $2.$3.001.$4          $2.$3.003.$4
	diff oiio        imagemagick $2.$3.001.$4          $2.$3.001.$4

	diff oiio        oiio        oiio.$2.$3.001.$4     $2.$3.003.$4
	diff imagemagick imagemagick oiio.$2.$3.001.$4     $2.$3.003.$4
	diff oiio        imagemagick oiio.$2.$3.001.$4     $2.$3.001.$4

	diff oiio        oiio        oiio.$2.$3.001.$4     oiio.$2.$3.003.$4
	diff imagemagick imagemagick oiio.$2.$3.001.$4     oiio.$2.$3.003.$4
	diff oiio        imagemagick oiio.$2.$3.001.$4     oiio.$2.$3.001.$4
	
	if [ $countReadOK -eq 9 ]; then
		writeRUndefWGood "$4" "$2" "$3" "$1"
		writeRGoodWGood  "$4" "$2" "$3" "OIIO"
		writeRGoodWUndef "$4" "$2" "$3" "ImageMagick"
	else
		writeRUndefWBad "$4" "$2" "$3" "$1"
		writeRBadWBad   "$4" "$2" "$3" "OIIO"
		writeRBadWUndef "$4" "$2" "$3" "ImageMagick"
	fi
	clearReadValues
}

function checkPluginComponentsBitDepth {
	diff $1          $1          $2.$3.001.$4          $2.$3.003.$4
	diff oiio        oiio        $2.$3.001.$4          $2.$3.003.$4
	diff imagemagick imagemagick $2.$3.001.$4          $2.$3.003.$4
	diff imagemagick $1          $2.$3.001.$4          $2.$3.001.$4
	diff oiio        $1          $2.$3.001.$4          $2.$3.001.$4
	diff oiio        imagemagick $2.$3.001.$4          $2.$3.001.$4
	
	diff $1          $1          oiio.$2.$3.001.$4     $2.$3.003.$4
	diff oiio        oiio        oiio.$2.$3.001.$4     $2.$3.003.$4
	diff imagemagick imagemagick oiio.$2.$3.001.$4     $2.$3.003.$4
	diff imagemagick $1          oiio.$2.$3.001.$4     $2.$3.001.$4
	diff oiio        $1          oiio.$2.$3.001.$4     $2.$3.001.$4
	diff oiio        imagemagick oiio.$2.$3.001.$4     $2.$3.001.$4
	
	diff $1          $1          oiio.$2.$3.001.$4     oiio.$2.$3.003.$4
	diff oiio        oiio        oiio.$2.$3.001.$4     oiio.$2.$3.003.$4
	diff imagemagick imagemagick oiio.$2.$3.001.$4     oiio.$2.$3.003.$4
	diff imagemagick $1          oiio.$2.$3.001.$4     oiio.$2.$3.001.$4
	diff oiio        $1          oiio.$2.$3.001.$4     oiio.$2.$3.001.$4
	diff oiio        imagemagick oiio.$2.$3.001.$4     oiio.$2.$3.001.$4
	
	if [ $countReadOK -eq 18 ]; then
		writeRGoodWGood  "$4" "$2" "$3" "$1"
		writeRGoodWGood  "$4" "$2" "$3" "OIIO"
		writeRGoodWUndef "$4" "$2" "$3" "ImageMagick"
	else
		writeRBadWBad   "$4" "$2" "$3" "$1"
		writeRBadWBad   "$4" "$2" "$3" "OIIO"
		writeRBadWUndef "$4" "$2" "$3" "ImageMagick"
	fi
	clearReadValues
}

echo "****************       PNG      ***************"
startFormat PNG
checkPluginComponentsBitDepth png gray 8bits  png
checkPluginComponentsBitDepth png gray 16bits png
checkPluginComponentsBitDepth png rgb  8bits  png
checkPluginComponentsBitDepth png rgb  16bits png
checkPluginComponentsBitDepth png rgba 8bits  png
checkPluginComponentsBitDepth png rgba 16bits png
endFormat PNG

echo "****************      JPEG      ***************"
startFormat Jpeg
checkPluginComponentsBitDepth jpeg rgb  8bits jpg
endFormat Jpeg

echo "****************       DPX      ***************"
startFormat DPX
#                        reader      writer components bitDepth extension
checkComponentsBitDepth1 imagemagick dpx    gray       8bits    dpx
checkComponentsBitDepth1 imagemagick dpx    gray       10bits   dpx
checkComponentsBitDepth1 imagemagick dpx    gray       12bits   dpx
checkComponentsBitDepth1 imagemagick dpx    gray       16bits   dpx

checkComponentsBitDepth dpx rgb  8bits dpx
checkComponentsBitDepth dpx rgb  10bits dpx
checkComponentsBitDepth dpx rgb  12bits dpx
checkComponentsBitDepth dpx rgb  16bits dpx

checkComponentsBitDepth dpx rgba 8bits dpx
checkComponentsBitDepth dpx rgba 10bits dpx
checkComponentsBitDepth dpx rgba 12bits dpx
checkComponentsBitDepth dpx rgba 16bits dpx
endFormat DPX

#echo "****************   JPEG 2000    ***************"



writeHtmlFooter
