#!/usr/bin/env python
# PYTHON_ARGCOMPLETE_OK

import argparse
import shutil

from clint.textui import colored

from sam_mv import SamMv


class SamCp(SamMv):
    """
    Class which represents the sam-cp operation.
    """

    def __init__(self):
        SamMv.__init__(self)
        self.command = 'cp'
        self.help = 'to copy sequences'
        self.description = str(colored.green('''
            Copy sequences in a directory.
            '''))
        self._operation = shutil.copy2


if __name__ == '__main__':
    # Create the tool
    tool = SamCp()

    # Create command-line interface
    parser = argparse.ArgumentParser(
        prog='sam-'+tool.command,
        description=tool.description,
        formatter_class=argparse.RawTextHelpFormatter,
        )

    # Run the command
    tool.fillParser(parser)
    tool.run(parser)
