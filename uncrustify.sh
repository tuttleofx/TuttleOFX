#!/bin/bash
uncrustify -c uncrustify.cfg --no-backup --mtime -p uncrustify.log $(find applications libraries plugins -type f -name "*.?pp" -o -name "*.tcc" -o -name "*.h")
