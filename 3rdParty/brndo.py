#! /usr/bin/env python
'''
(brndo0.0)
Bernardo ! Come and fetch ! ... and build, and check and do stuffs.

SYNOPSIS
    ./brndo.py [-h]

    Without arguments run a cmd line interface (CLI) for helping compiling Tuttle 3rdparties.

    Options:
      -h  Print this help and exit.

    Feature:
        readline history (file save in .brndohistory)

CLI CMDS
  help ?
  quit q EOF
  list
  fetch (WIP - DryRun)
  shacheck

TODO
cmds:
  fetch - force download option
  uncompress
  configure
  build
  install
  clean
  distclean
'''
import cmd
import os
import readline
import sha
import sys
from pprint import pformat
from pydoc import pager

from init import all_libs


download_dir = "archive"
extracted_dir = "extracted"

lib_dict = {
    # lib_name: {'name': libname, 'sha': sha, 'url': url},
}

class Brndo(cmd.Cmd):
    def __init__(self):
        cmd.Cmd.__init__(self)
        self.ruler =' '

    def help_help(self):
        print "Print the help."

    def help_brndo(self):
        pager(__doc__)

    def do_quit(self, arg):
        '''
        Exit program
        '''
        sys.exit()

    def do_list(self, arg):
        '''
        list
        Print the lib list.
        '''
        if arg:
            print self.do_list.__doc__
            return
        pager(pformat(all_libs))

    def do_fetch(self, arg):
        '''
        fetch [-f] [lib [...]]
        Fetch lib.
          -f  force fetching
        '''
        libs = all_libs
        force_fetch = False
        if arg:
            args = arg.split()
            if '-f' in args:
                force_fetch = True
                args.remove('-f')
            if len(args):
                libs = []
                for name in args:
                    if name in lib_dict.keys():
                        libs.append(lib_dict[name])
                    else:
                        print "*Warning* - target lib '{}' not found.".format(name)

        for lib in libs:
            print "--\nFetching ", lib['name'], 'at', lib['url']
            print "force_fetch =", force_fetch
            print "(Dry run)"
            destination_filename = os.path.abspath(os.path.join(download_dir,
                                                                os.path.basename(lib['url'])))
            if os.path.exists(destination_filename):
                print("Destination {} alredy exist.".format(destination_filename))
                if not force_fetch:
                    print("Skipping download.")
                    continue
                print("Forced download !")
            ## HERE THE CODE FOR RETRIEVING ARCHIVE
            print("Ok - File downloaded.")
            
            
    def do_shacheck(self, arg):
        '''
        shacheck [lib [...]]
        Check lib archive integrity.
        '''
        libs = all_libs

        if arg:
            libs = []
            args = arg.split()
            for name in args:
                if name in lib_dict.keys():
                    libs.append(lib_dict[name])
                else:
                    print "*Warning* - target lib '{}' not found.".format(name)

        for lib in libs:
            print "--\nSHA check", lib['name']
            destination_filename = os.path.abspath(os.path.join(download_dir,
                                                                os.path.basename(lib['url'])))
            if os.path.exists(destination_filename):
                print("File {} exist.\n"
                      "Performing check ...".format(destination_filename))
                with open(destination_filename, 'rb') as f:
                    current_sha = sha.sha(f.read()).hexdigest()
                print "Sha Expected :", lib['sha']
                print "Sha Current  :", current_sha
                if current_sha != lib['sha']:
                    print "**ERROR** - Sha dont match !"
                else:
                    print "Ok - Sha match."
            else:
                "**ERROR** - File {} doesn't exist."

    def do_uncompress(arg):
        '''
        Uncompress lib.
        TODO
        '''
        print "Not implemented."

    def do_configure(arg):
        '''
        Configure lib.
        TODO
        '''
        print "Not implemented."

    def do_build(arg):
        '''
        Build lib.
        TODO
        '''
        print "Not implemented."

    def do_install(arg):
        '''
        Install lib.
        TODO
        '''
        print "Not implemented."

    def do_clean(arg):
        '''
        Clean lib build dir.
        TODO
        '''
        print "Not implemented."

    def do_distclean(arg):
        '''
        Delete all files (archive, build directory)
        TODO
        '''
        print "Not implemented."

    # shortcuts
    do_q        = do_quit
    do_EOF      = do_quit


## History stuffs
historyPath = ".brndohistory"

def save_history(historyPath=historyPath):
    readline.write_history_file(historyPath)

if __name__ == '__main__':
    if '-h' in sys.argv:
        print __doc__
        sys.exit()

    brndo = Brndo()

    ## Cmd history setup
    if os.path.exists(historyPath):
        readline.read_history_file(historyPath)
    import atexit
    atexit.register(save_history)

    # lib_dict fill
    for l in all_libs:
        lib_dict[l['name']] = l

    brndo.cmdloop(__doc__)
