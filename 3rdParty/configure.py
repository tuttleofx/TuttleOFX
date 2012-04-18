#!/usr/bin/python
# -*- coding: utf-8 -*-

import os


os.chdir ( "./freetype" )
os.system( "./configure" )
os.chdir ( ".." )

os.chdir ( "./libcaca" )
os.system( "./configure" )
os.chdir ( ".." )

os.chdir ( "./jpeg" )
os.system( "./configure" )
os.chdir ( ".." )

os.chdir ( "./openjpeg" )
os.system( "./configure" )
os.chdir ( ".." )

os.chdir ( "./libtiff" )
os.system( "./configure" )
os.chdir ( ".." )

os.chdir ( "./libxml" )
os.system( "./configure" )
os.chdir ( ".." )

os.chdir ( "./imagemagick" )
os.system( "./configure --disable-opencl --disable-openmp --with-gnu-ld=no --enable-hdri=no --with-bzlib=no --with-autotrace=no --with-djvu=no  --with-dps=no  --with-fftw=no  --with-fpx=no --with-fontconfig=no --with-freetype=no --with-gslib=no --with-gvc=no --with-jbig=no --with-jpeg=no --with-jp2=no --with-lcms=no --with-lcms2=no --with-lqr=no --with-lzma=no --with-magick-plus-plus=no --with-openexr=no --with-perl=no --with-pango=no --with-png=no  --with-rsvg=no  --with-tiff=no --with-webp=no --with-wmf=no --with-x=no --with-xml=no --with-zlib=no" )
os.chdir ( ".." )

os.chdir ( "./webp" )
os.system( "./configure" )
os.chdir ( ".." )
