#!/bin/sh
ll -d /usr/OFX > /dev/null 2>&1
if [ $? = 0 ]
then
mkdir /usr/OFX
fi

ll -d /usr/OFX/Plugins > /dev/null 2>&1
if [ $? = 0 ]
then
mkdir /usr/OFX/Plugins
fi

cp -R ../dist/plugins/image/*/*/bundle/* /usr/OFX/Plugins
cp -R ../dist/plugins/image/*/*/*/bundle/* /usr/OFX/Plugins
cp -R ../dist/plugins/param/*/*/bundle/* /usr/OFX/Plugins
