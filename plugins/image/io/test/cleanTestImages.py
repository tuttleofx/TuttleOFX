#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import glob

for filename in glob.glob('*.dpx') :
	os.remove( filename )

for filename in glob.glob('*.jpg') :
	os.remove( filename )
	
for filename in glob.glob('*.jpeg') :
	os.remove( filename )
	
for filename in glob.glob('*.bmp') :
	os.remove( filename )
	
for filename in glob.glob('*.dds') :
	os.remove( filename )
	
for filename in glob.glob('*.ppm') :
	os.remove( filename )
	
for filename in glob.glob('*.j2k') :
	os.remove( filename )
	
for filename in glob.glob('*.exr') :
	os.remove( filename )
	
for filename in glob.glob('*.sgi') :
	os.remove( filename )
	
for filename in glob.glob('*.tga') :
	os.remove( filename )
	
for filename in glob.glob('*.png') :
	os.remove( filename )
	
for filename in glob.glob('*.hdr') :
	os.remove( filename )
	
for filename in glob.glob('*.tif') :
	os.remove( filename )
	
for filename in glob.glob('*.tiff') :
	os.remove( filename )

