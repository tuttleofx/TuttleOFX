#!/usr/bin/env python
# coding: utf-8 
# PYTHON_ARGCOMPLETE_OK

import argparse
import shutil

from clint.textui import colored

from sam_mv import Sam_mv


class Sam_cp(Sam_mv):
    """
    Class which represents the sam-cp operation.
    """

    def __init__(self):
        Sam_mv.__init__(self)
        self.command = 'cp'
        self.help = 'to copy sequences'
        self.description = str(colored.green('''
            Copy one or more sequences to a directory.
            '''))
        self._operation = shutil.copy2


if __name__ == '__main__':
    # Create the tool
    tool = Sam_cp()

    # Create command-line interface
    parser = argparse.ArgumentParser(
        prog='sam-'+tool.command,
        description=tool.description,
        formatter_class=argparse.RawTextHelpFormatter,
        )
    tool.fillParser(parser)

    # Activate completion
    samUtils.doCompletion(parser)

    # Run the command
    tool.run(parser)
