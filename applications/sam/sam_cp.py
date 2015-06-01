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
        SamMv.__init__(self)
        self.command = 'cp'
        self.help = 'cp, to copy sequences'
        self.description = '''
            Copy sequence(s) in a directory.
            '''
        self._operation = shutil.copy2


if __name__ == '__main__':
    # Create the tool
    tool = SamCp()

    # Create command-line interface
    parser = argparse.ArgumentParser(
        prog='sam-'+tool.command,
        description=tool.description,
        )

    # Run the command
    tool.fillParser(parser)
    tool.run(parser)
