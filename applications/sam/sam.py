#!/usr/bin/env python

import argparse

# sam common functions
from common import samCmdLines

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

    subparsers = parser.add_subparsers(dest='subCommand')
    
    # parser for ls command
    parser_ls = subparsers.add_parser('ls', help='ls, to list sequences (and other files)')
    samCmdLines.addLsArgumentsToParser(parser_ls)
    # parser for mv command
    parser_mv = subparsers.add_parser('mv', help='mv, to move sequences')
    samCmdLines.addMvCpArgumentsToParser(parser_mv)
    # parser for cp command
    parser_cp = subparsers.add_parser('cp', help='mv, to copy sequences')
    samCmdLines.addMvCpArgumentsToParser(parser_cp)
    # parser for rm command
    parser_rm = subparsers.add_parser('rm', help='rm, to remove sequences (and other files)')
    samCmdLines.addRmArgumentsToParser(parser_rm)

    # Parse command-line
    args = parser.parse_args()

    # launch sam command
    module = __import__('sam-' + args.subCommand)
    module.main(args)
