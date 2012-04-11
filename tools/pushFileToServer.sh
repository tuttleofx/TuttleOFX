#!/bin/bash

HOST='ftpperso.free.fr'
USER='tuttleofx'
PASSWD='dutrucmi'

ftp -n -v $HOST << EOT
ascii
user $USER $PASSWD
prompt
put $1 $2
bye
EOT
