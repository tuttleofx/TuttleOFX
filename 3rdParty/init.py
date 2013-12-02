all_libs = [
            {'name'        :  'zlib',
             'sha'         :  '60faeaaf250642db5c0ea36cd6dcc9f99c8f3902',
             'url'         :  'http://prdownloads.sourceforge.net/libpng/zlib-1.2.3.tar.gz',
             'dependencies': [],
             'cmd'         : {'default':
                                  {'configure': './configure --prefix ../../local',
                                   'build'    : 'make',
                                   'install'  : 'make install',
                                   'clean'    : 'make clean',
                                   'distclean': 'make distclean',
                                   },
                              'darwin':
                                  {'configure': './configure --prefix ../../local',
                                   'build'    : 'make',
                                   'install'  : 'make install',
                                   'clean'    : 'make clean',
                                   'distclean': 'make distclean',
                                   },
                              'linux' :
                                  {'configure': '',
                                   'build'    : '',
                                   'install'  : '',
                                   'clean'    : '',
                                   'distclean': '',
                                   },
                              'win32' :
                                  {'configure': '',
                                   'build'    : '',
                                   'install'  : '',
                                   'clean'    : '',
                                   'distclean': '',
                                   }
                              },
             },   

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
             'url': 'http://prdownloads.sourceforge.net/freetype/freetype-2.4.3.tar.gz',
             'configure_cmd': './configure',
             },
            
            {'name': 'libraw',
             'sha': 'e924527bed3d72ee4756da0c9383dc74c584799f',
             'url': 'http://www.libraw.org/data/LibRaw-0.14.7.tar.gz'},
            
            {'name': 'libcaca',
             'sha': '24cdb9fb7dc4a01be2787a9009304fad419f79db',
             'url': 'http://caca.zoy.org/files/libcaca/libcaca-0.99.beta17.tar.gz',
             'configure_cmd': './configure'},
            
            {'name': 'lcms2',
             'sha': '0ce856a004068ff6e22f9f2d7b94a11b9b469bf9',
             'url': 'http://downloads.sourceforge.net/project/lcms/lcms/2.1/lcms2-2.1.tar.gz'},
            
            {'name': 'bzlib',
             'sha': '3f89f861209ce81a6bab1fd1998c0ef311712002',
             'url': 'http://www.bzip.org/1.0.6/bzip2-1.0.6.tar.gz'},
            
            {'name': 'libtiff',
             'sha': 'f40aab20fb2f609b5cbc1171c40b66a1445e3773',
             'url': 'ftp://ftp.remotesensing.org/pub/libtiff/tiff-3.9.5.tar.gz',
             'configure_cmd': './configure',
             },
            
            {'name': 'libtool',
             'sha': '149e9d7a993b643d13149a94d07bbca1085e601c',
             'url': 'ftp://ftp.gnu.org/gnu/libtool/libtool-2.4.tar.gz'},
            
            {'name': 'libxml',
             'sha': '859dd535edbb851cc15b64740ee06551a7a17d40',
             'url': 'ftp://xmlsoft.org/libxml2/libxml2-2.7.8.tar.gz',
             'configure_cmd': './configure',
             },
            
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
             'url': 'http://freefr.dl.sourceforge.net/project/libjpeg/libjpeg/6b/jpegsrc.v6b.tar.gz',
             'configure_cmd': './configure',
             },
            
            {'name': 'turbojpeg',
             'sha': '4ab00afc9a8d54cd2e7a67aacb9c49e01a8bccac',
             'url': 'http://garr.dl.sourceforge.net/project/libjpeg-turbo/1.2.0/libjpeg-turbo-1.2.0.tar.gz'},
            
            {'name': 'openjpeg',
             'sha': '9ef6a08ecf5944962b4e2cd7569ac01a8eaa66d0',
             'url': 'http://openjpeg.googlecode.com/files/openjpeg_v1_4_sources_r697.tgz',
             'configure_cmd': './configure',
             },
            
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
             'sha': 'cea4342b1d56ad12b8e94279a4228b6f7633568e',
             'url': 'http://www.imagemagick.org/download/ImageMagick-6.8.3-4.tar.gz',
             'configure_cmd': ('./configure '
                               '--disable-opencl --disable-openmp --with-gnu-ld=no '
                               '--enable-hdri=no --with-bzlib=no --with-autotrace=no '
                               '--with-djvu=no --with-dps=no --with-fftw=no '
                               '--with-fpx=no --with-fontconfig=no --with-freetype=no '
                               '--with-gslib=no --with-gvc=no --with-jbig=no '
                               '--with-jpeg=no --with-jp2=no --with-lcms=no '
                               '--with-lcms2=no --with-lqr=no --with-lzma=no '
                               '--with-magick-plus-plus=no --with-openexr=no '
                               '--with-perl=no --with-pango=no --with-png=no  '
                               '--with-rsvg=no  --with-tiff=no --with-webp=no '
                               '--with-wmf=no --with-x=no --with-xml=no '
                               '--with-zlib=no')},
            
            {'name': 'webp',
             'sha': 'f46aff9f1aeb8620d7ca4b9afc54e6df6d4de313',
             'url': 'https://webp.googlecode.com/files/libwebp-0.1.3.tar.gz',
             'configure_cmd': './configure'},
            
            {'name': 'tinyxml',
             'sha': '2ff9c177a6c0bef10cbd27700b71826801c12987',
             'url': 'http://netcologne.dl.sourceforge.net/project/tinyxml/tinyxml/2.6.1/tinyxml_2_6_1.tar.gz'},
            
            {'name': 'yaml',
             'sha': '5141c5a3beecbe80e6772d3d9ece1d668940906f',
             'url': 'http://yaml-cpp.googlecode.com/files/yaml-cpp-0.2.6.tar.gz'},
            ]
