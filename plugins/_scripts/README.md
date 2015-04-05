Here you can use python scripts to easily create your OpenFX plugin.

#### How to create my plugin?
###### newPlugin.py
This script asks you information about your plugin and create a new plugin source tree in your current directory.
You should indicate as destination the ```plugins/private``` folder in the TuttleOFX project.
After that, you just need to care about your algorithm.

###### renameInDirectory.py
Use this script to rename your plugin (files and content), even after some development.

###### checkBrackets.py
Check the number of open/close brackets in files.

#### How to compile my plugin?
You need to warn CMake about your new plugin. To do that, edit ```plugins/CMakeLists.txt``` by adding:
```
add_subdirectory(private/yourPlugin)
```
CMake will take care of the rest when you will build the project.

#### How to implement my plugin?
Now it's your turn!

