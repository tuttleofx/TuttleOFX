#!/bin/bash

if [[ ! -f uncrustify.cfg ]]; then
	echo "This program should be used from the (TuttleOFX)/tool directory"
	exit 1
fi

# building configuration file without header
cp uncrustify.cfg uncrustify_header.cfg
echo cmt_insert_file_header=\"header.txt\">>uncrustify_header.cfg

# building configuration file with header
cp uncrustify.cfg uncrustify_no_header.cfg
echo cmt_insert_file_header=>>uncrustify_no_header.cfg

#uncrustifying TuttleOFX source file adding a header if needed
uncrustify -c uncrustify_header.cfg --no-backup --mtime $(find ../applications ../libraries ../plugins -type f -name "*.?pp" -o -name "*.tcc" -o -name "*.h"|grep -v /openfxHack)
#uncrustifying original OpenFX source file without adding our header
uncrustify -c uncrustify_no_header.cfg --no-backup --mtime $(find ../applications ../libraries ../plugins -type f -name "*.?pp" -o -name "*.tcc" -o -name "*.h"|grep /openfxHack)

#cleaning temporary files
rm -f uncrustify_header.cfg uncrustify_no_header.cfg
