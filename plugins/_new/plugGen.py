#!/usr/bin/python
import sys, os, string, shutil, time



def walktree(top = ".", depthfirst = True):
    import os, stat, types
    names = os.listdir(top)
    if not depthfirst:
        yield top, names
    for name in names:
        try:
            st = os.lstat(os.path.join(top, name))
        except os.error:
            continue
        if stat.S_ISDIR(st.st_mode):
            for (newtop, children) in walktree (os.path.join(top, name), depthfirst):
                yield newtop, children
    if depthfirst:
        yield top, names

def adaptTemplate(filepath):
    # Schmlurp whole file
    fSource = open( filepath, 'r' )
    source = fSource.read( )
    fSource.close( )
    source = source.replace('%L_CLASSNAME%', className.lower())
    source = source.replace('%D_CLASSNAME%', className.upper())
    source = source.replace('%CLASSNAME%', className)
    source = source.replace('%NAMESPACE%', namespace)
    source = source.replace('%PLUGIN_GROUP%', pluginGroup)
    source = source.replace('%PLUGIN_LABEL%', pluginLabel)
    source = source.replace('%PLUGIN_LONG_LABEL%', pluginLongLabel)
    source = source.replace('%PROJECT_NAME%', projectName)
    source = source.replace('%COMPANY_NAME%', companyName)
    source = source.replace('%DATE%', time.strftime('%d/%m/%y %H:%M',time.localtime()))
    fDest = open( filepath, 'w' )
    fDest.write(source)
    fDest.close()

if __name__ == '__main__':
    pluginName = ""
    while(len(pluginName) == 0):
        pluginName = raw_input( "Plugin name (With or without spaces) ?: " )
    pluginLabel = raw_input( "Plugin label ?: " )
    pluginLongLabel = raw_input( "Plugin long label ?: " )
    pluginGroup = ""
    while(len(pluginGroup) == 0):
        pluginGroup = raw_input( "Plugin group ?: " )
    className = ""
    while(len(className) == 0 or className.find( " " ) != -1  or className[0].isupper( ) == False):
        className = raw_input( "Class name (Capitalized, Without spaces) ?: " )
    namespace = ""
    while(len(namespace) == 0 or namespace.find( " " ) != -1):
        namespace = raw_input( "Namespace ?: ")
    print
    print "Setting up the declaration string (eg. fr.hd3d.tuttle.denoiser):"
    companyName = ""
    while (len(companyName) == 0 or companyName.find( " " ) != -1):
        companyName = raw_input( "company ?: " )
    projectName = ""
    while(len(projectName) == 0 or projectName.find( " " ) != -1):
        projectName = raw_input( "project ?: " )
    try:
        print "Processing files..."
        # Adapting directories.
        currentDir = os.getcwd( ) + '/'
        pluginDir = currentDir + '../' + className + '/'
        pluginSrcDir = pluginDir + 'src/'

        #Remove old plugin dir
        if os.path.exists( pluginDir ):
            print
            if (raw_input( "Warning ! Destination %s will be removed from the disk !\nAre you sure (y/n) ?:" % pluginDir).lower() == "y"):
                shutil.rmtree( pluginDir );
            else:
                sys.exit(2);
            print
        #Copy to plugin directory
        shutil.copytree( currentDir + 'folder/', pluginDir );
        for top, names in walktree( pluginDir ):
            for name in names:
                # Rename templates files with a correct filename
                dName = name.replace( "L_ClassName_", className.lower() );
                dName = dName.replace( "_ClassName_", className );
                dName = dName.replace( "L_CompanyName_", companyName.lower() );
                dName = dName.replace( "_CompanyName_", companyName );
                dName = dName.replace( "L_ProjectName_", projectName.lower() );
                dName = dName.replace( "_ProjectName_", projectName );
                if (dName != name):
                    print "Processing: " + name + " to " + dName;
                    os.rename( top + '/' + name, top + '/' + dName );
                    # Check if we need to replace things on the file
                    if (dName.endswith(".cpp") or dName.endswith(".hpp") or dName.endswith(".tcc")):
                        adaptTemplate(top + '/' + dName)
        sys.exit(0);

    except ValueError, v:
        try:
            ( code, message ) = v
        except:
            code = 0
            message = v
        print "Sorry, " + str( message ) + " (" + str( code ) + ")"
        print
        sys.exit(1);
