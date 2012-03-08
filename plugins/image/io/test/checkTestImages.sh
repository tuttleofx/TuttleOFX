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


echo "***********************************************"
echo "****************      DIFF      ***************"
echo "***********************************************"

function diff {
	sam-diff --reader tuttle.$1reader --reader tuttle.$2reader --inputs $3 --inputs $4 >> diff.txt
	if [ "$?" -eq "0" ]; then
		echo -e "$BLUE""$1\t| $2 \t is OK ($3, $4)""$NC"
	else
		echo -e "$RED""$1\t| $2 \t not similar ($3, $4)  "`grep "diff =" diff.txt | tail -1`"$NC"
	fi
}

diff png         png         rgb_8bits.001.png          rgb_8bits.003.png
diff png         png         rgb_16bits.001.png         rgb_16bits.003.png

diff jpeg        jpeg        rgb.001.jpg                rgb.003.jpg
diff dpx         dpx         rgb_8bits.001.dpx          rgb_8bits.003.dpx
diff png         dpx         rgb_8bits.001.png          rgb_8bits.003.dpx
diff dpx         dpx         rgb_16bits.001.dpx         rgb_16bits.003.dpx
diff png         dpx         rgb_16bits.001.png         rgb_16bits.003.dpx
diff dpx         dpx         rgba_8bits.001.dpx         rgba_8bits.003.dpx
diff png         dpx         rgba_8bits.001.png         rgba_8bits.003.dpx
diff dpx         dpx         rgba_16bits.001.dpx        rgba_16bits.003.dpx
diff png         dpx         rgba_16bits.001.png        abgr_16bits.003.dpx
diff dpx         dpx         abgr_8bits.001.dpx         abgr_8bits.003.dpx
diff png         dpx         rgba_8bits.001.png         abgr_8bits.003.dpx
diff dpx         dpx         abgr_16bits.001.dpx        abgr_16bits.003.dpx
diff jpeg2000    jpeg2000    rgb_8bits.001.j2k          rgb_8bits.003.j2k
diff jpeg2000    jpeg2000    rgb_12bits.001.j2k         rgb_12bits.003.j2k
diff jpeg2000    jpeg2000    rgb_16bits.001.j2k         rgb_16bits.003.j2k
diff png         jpeg2000    rgb_8bits.001.png          rgb_8bits.001.j2k
diff png         jpeg2000    rgb_16bits.001.png         rgb_16bits.003.j2k


diff oiio        oiio        oiio_rgb_8bits.001.png     oiio_rgb_8bits.003.png
diff png         png         oiio_rgb_8bits.001.png     oiio_rgb_8bits.003.png
diff png         png         rgb_8bits.001.png          oiio_rgb_8bits.001.png
diff png         oiio        oiio_rgb_8bits.001.png     oiio_rgb_8bits.001.png
diff png         oiio        rgb_8bits.001.png          oiio_rgb_8bits.001.png
diff png         oiio        oiio_rgb_8bits.001.png     rgb_8bits.001.png
diff oiio        oiio        oiio_rgb_16bits.001.png    oiio_rgb_16bits.003.png
diff oiio        oiio        oiio_rgb.001.jpg           oiio_rgb.003.jpg
diff oiio        oiio        oiio_rgb_8bits.001.dpx     oiio_rgb_8bits.003.dpx
diff oiio        oiio        oiio_rgb_8bits.001.png     oiio_rgb_8bits.003.dpx
diff oiio        oiio        oiio_rgb_16bits.001.dpx    oiio_rgb_16bits.003.dpx
diff oiio        oiio        oiio_rgb_16bits.001.png    oiio_rgb_16bits.003.dpx
diff png         oiio        oiio_rgb_16bits.001.png    oiio_rgb_16bits.003.dpx
diff png         dpx         oiio_rgb_16bits.001.png    oiio_rgb_16bits.003.dpx
diff oiio        dpx         oiio_rgb_16bits.001.png    oiio_rgb_16bits.003.dpx
diff oiio        oiio        oiio_rgba_8bits.001.dpx    oiio_rgba_8bits.003.dpx
diff oiio        oiio        oiio_rgba_8bits.001.png    oiio_rgba_8bits.003.dpx
diff oiio        oiio        rgba_8bits.001.png         rgba_8bits.003.dpx
diff png         oiio        oiio_rgba_8bits.001.png    oiio_rgba_8bits.003.dpx
diff oiio        dpx         oiio_rgba_8bits.001.png    oiio_rgba_8bits.003.dpx

diff oiio        oiio        oiio_rgba_16bits.001.dpx   oiio_rgba_16bits.003.dpx
diff oiio        oiio        oiio_rgba_16bits.001.png   oiio_rgba_16bits.003.dpx
diff png         oiio        oiio_rgba_16bits.001.png   oiio_rgba_16bits.003.dpx



#diff oiio        oiio        oiio_rgb_8bits.001.j2k    oiio_rgb_8bits.003.j2k
#diff oiio        oiio        oiio_rgb_16bits.001.j2k   oiio_rgb_16bits.003.j2k
#diff oiio        oiio        oiio_rgb_8bits.001.png    oiio_rgb_8bits.001.j2k
#diff oiio        oiio        oiio_rgb_16bits.001.png   oiio_rgb_16bits.003.j2k
#diff oiio        jpeg2000    oiio_rgb_8bits.001.j2k    oiio_rgb_8bits.003.j2k
#diff jpeg2000    oiio        oiio_rgb_8bits.001.j2k    oiio_rgb_8bits.003.j2k
#diff jpeg2000    jpeg2000    oiio_rgb_8bits.001.j2k    oiio_rgb_8bits.003.j2k
#diff oiio        oiio        oiio_rgb_16bits.001.j2k   oiio_rgb_16bits.003.j2k
#diff oiio        oiio        oiio_rgb_8bits.001.png    oiio_rgb_8bits.001.j2k
#diff oiio        oiio        oiio_rgb_16bits.001.png   oiio_rgb_16bits.003.j2k


diff oiio        oiio        oiio_gray_8bits.001.png    oiio_gray_8bits.001.sgi
diff oiio        oiio        oiio_gray_16bits.001.png   oiio_gray_16bits.001.sgi

diff oiio        oiio        oiio_rgb_8bits.001.png     oiio_rgb_8bits.001.sgi
diff oiio        oiio        oiio_rgb_16bits.001.png    oiio_rgb_16bits.001.sgi

diff oiio        oiio        oiio_rgba_8bits.001.png    oiio_rgba_8bits.001.sgi
diff oiio        oiio        oiio_rgba_16bits.001.png   oiio_rgba_16bits.001.sgi

diff oiio        oiio        gray_16float.001.exr       oiio_gray_16float.001.exr
diff oiio        oiio        gray_32bits.001.exr        oiio_gray_32bits.001.exr
diff oiio        oiio        gray_32float.001.exr       oiio_gray_32float.001.exr

diff oiio        oiio        rgb_16float.001.exr        oiio_rgb_16float.001.exr
diff oiio        oiio        rgb_32bits.001.exr         oiio_rgb_32bits.001.exr
diff oiio        oiio        rgb_32float.001.exr        oiio_rgb_32float.001.exr

diff png         oiio        oiio_rgb_32bits.001.png    oiio_rgb_32bits.001.exr
diff png         oiio        oiio_rgb_32float.001.png   oiio_rgb_32float.001.exr

diff oiio        oiio        rgba_16float.001.exr       oiio_rgba_16float.001.exr
diff oiio        oiio        rgba_32bits.001.exr        oiio_rgba_32bits.001.exr
diff oiio        oiio        rgba_32float.001.exr       oiio_rgba_32float.001.exr

diff oiio        oiio        oiio_rgb_8bits.001.hdr     oiio_rgb_32float.001.exr
diff oiio        oiio        oiio_rgb_32float.001.hdr   oiio_rgb_32float.001.exr

diff oiio        oiio        oiio_rgb_32float.001.hdr   oiio_rgb_32float.003.png
diff oiio        png         oiio_rgb_32float.001.hdr   oiio_rgb_32float.003.png

