#!/usr/bin/env python
# PYTHON_ARGCOMPLETE_OK

import argparse
import shutil

from sam_mv import SamMv


class SamCp(SamMv):
    """
    Class which represents the sam-cp operation.
    """

    def __init__(self):
        self.command = 'cp'
        self.help = 'cp, to copy sequences'
        self._operation = shutil.copy2


if __name__ == '__main__':
    # Create command-line interface
    parser = argparse.ArgumentParser(
        prog='sam-cp',
        description='''
        copy sequence(s) in a directory.
        Copy sequence of image files, and could remove trees (folder, files and sequences).
        ''',
        )

    # Create and run the command
    command = SamCp()
    command.fillParser(parser)
    command.run(parser)
