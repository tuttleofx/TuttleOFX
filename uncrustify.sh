#!/bin/bash
FILES=files_to_uncrustify_delete_me.txt
find applications libraries plugins -type f -name "*.?pp" -o -name "*.tcc" -o -name "*.h">$FILES
uncrustify -c uncrustify.cfg --no-backup --mtime -l CPP -p uncrustify.log -F $FILES
rm -f $FILES
