@echo off
set FILES=files_to_uncrustify_delete_me.txt
dir applications\*.?pp libraries\*.?pp plugins\*.?pp /s /b  > %FILES%
uncrustify -c uncrustify.cfg --no-backup --mtime -l CPP -p uncrustify.log -F %FILES%
del %FILES%
