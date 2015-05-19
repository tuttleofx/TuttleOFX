#!/usr/bin/env python

import os
import argparse


if __name__ == '__main__':

    # Create command-line interface
    parser = argparse.ArgumentParser(
            prog='sam',
            description='''
            A set of command line tools.
            Specialized in sequence manipulations.
            ''',
            formatter_class=argparse.RawTextHelpFormatter
            )

    # Options
    parser.add_argument('command', choices=['ls', 'mv', 'cp', 'rm'], help='''The command to launch:
    ls, to list sequences (and other files)
    mv, to move sequences
    cp, to copy sequences
    rm, to remove sequences (and other files)
    '''
    )
    parser.add_argument('options', nargs='*', help='List of options given to the command.')

    # Parse command-line
    args, unknown = parser.parse_known_args()

    # launch sam command
    args.options.extend(unknown)
    os.system('sam-' + args.command + '.py ' + ' '.join(args.options))
