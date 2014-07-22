Tuttle binding scripts samples
==============================

envTuttle.py
------------

> import envTuttle

Set pyTuttle environment.
Edit this script to set your TUTTLE_ROOT path.  

envTuttle **should be imported before using the binding**.

loadTuttleLibraries.py
----------------------

Imported in *envTuttle.py* to load shared libraries.  
This script **should be edited to match your own installation**.

listPlugins
-----------

List avalaible OpenFX plugins

getBestPlugin.py
----------------

> readIdentifier = getBestReader(ext)  
> writerIdentifier = getBestWriter(ext)

Functions to choose the best reader or writer according to an extension.
Required by seq-info.

seq-info
--------

Display properties of a file or sequence.  
**Requires argparse**.


viewer
-----

Compute the equivalent of : sam do r input // view
