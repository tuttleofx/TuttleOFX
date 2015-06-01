#!/usr/bin/env python

import argparse

# import sam tools
from sam_ls import SamLs
from sam_mv import SamMv
from sam_cp import SamCp
from sam_rm import SamRm
from sam_do import SamDo


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

    # Create dict of sam tools
    tools = [SamLs(), SamMv(), SamCp(), SamRm(), SamDo()]
    tools = {tool.command: tool for tool in tools}

    # Create a subparser per sam tool
    subparsers = parser.add_subparsers(dest='samSubCommand')
    for tool in tools.values():
        toolParser = subparsers.add_parser(tool.command, help=tool.help, add_help=(False if tool.command == 'do' else True))
        tool.fillParser(toolParser)

    # Parse command-line
    args, unknown = parser.parse_known_args()

    # launch sam tool
    tool = tools.get(args.samSubCommand)
    if tool:
        tool.run(parser)
