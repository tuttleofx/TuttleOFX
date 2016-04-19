# TuttleOFX

This bunch of tools includes:
* a set of command line tools called Sam,
* some plugins like classical Resize, Lut, Crop... or LensDistort and Histogram Keyer.

TuttleOFX plugins are based on OpenFX standard, making them usable with the Sam command line tool but also with other applications like Nuke (The Foundry), Scratch (Assimilate), Baselight (FilmLight), Mistika (SGO)...

## The OFX_PLUGIN_PATH
The first step to use any OpenFX plugin in any OpenFX host, is to set an environment variable to tell where are the plugins.  
The variable is OFX_PLUGIN_PATH. After that, each host has its specificities and its ways to manage OpenFX plugins.

```
export OFX_PLUGIN_PATH=/path/to/your/tuttle/OFX
```

Note: some host provide a way to set this path in their GUI. Checkout the "preference" menu.


## Override host name
Some OFX plugins define a specific behavior depending on the host name (shame on them!).
To handle these cases, you can override the TuttleOFX host name by defined the following environment variable:
```
export TUTTLE_OVERRIDE_HOST_OFXPROPNAME=Your new Host name
```

## Override path to plugin cache
TuttleOFX host handles a cache file (called __tuttlePluginCacheSerialize.xml__) to load OpenFX plugins more quickly.
By default the path to this file is the UNIX home (see HOME) or the application data directory (see APPDATA) on Windows.
You can override this path by defining the following variable in your environment:
```
export TUTTLE_HOME=/your/custom/path
```
If none of these variables are found, the cache file is written in the temporary system directory.


## The log level
TuttleOFX host handles a global level to log information.  
The list of possible log level are:
* fatal
* error
* warning
* info
* debug
* trace

The default log level is warning. It can be override by defining the following variable in your environment:
```
export TUTTLE_LOG_LEVEL=trace
```
Note: the sam command line has an other log level, separate from the level of the TuttleOFX host. See sam options to get more details.


## More information
http://www.tuttleofx.org/user-documentation
