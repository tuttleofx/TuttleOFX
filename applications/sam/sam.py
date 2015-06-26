#!/usr/bin/env python
# PYTHON_ARGCOMPLETE_OK

import sys
import argparse

import argcomplete

from clint.textui import puts, colored


def getAllSamTools():
    """
    Get all sam tools as a dict {name: intance, ...}.
    """
    from sam_ls import Sam_ls
    from sam_mv import Sam_mv
    from sam_cp import Sam_cp
    from sam_rm import Sam_rm
    from sam_do import Sam_do

    tools = [Sam_ls(), Sam_mv(), Sam_cp(), Sam_rm(), Sam_do()]
    return {tool.command: tool for tool in tools}


if __name__ == '__main__':
    # Create command-line interface
    parser = argparse.ArgumentParser(
        prog='sam',
        description=colored.green('''
            Sam is a set of command line tools to manage image processing.
            Specialized in sequence manipulations.
            '''),
        formatter_class=argparse.RawTextHelpFormatter
        )

    # dict of sam tools to use
    tools = []

    if len(sys.argv) < 2 or sys.argv[1].startswith('-'):
        # import all sam tools
        tools = getAllSamTools()
    else:
        # import a specific sam tool
        toolCommand = sys.argv[1]
        toolClass = 'Sam_' + toolCommand
        try:
            toolModule = __import__('sam_' + toolCommand, fromlist=[toolClass])
            toolClass = getattr(toolModule, toolClass)
            tools = {toolCommand: toolClass()}
        except ImportError:
            # sam tool unknown: import all
            puts(colored.red('Error: no sam tool is named "' + toolCommand + '".'))
            tools = getAllSamTools()

    # Create a subparser for sam commands
    subparsers = parser.add_subparsers(dest='samSubCommand')
    # Add a parser per sam command
    for tool in tools.values():
        toolParser = subparsers.add_parser(
            tool.command,
            help=tool.help,
            description=tool.description,
            epilog=(tool.epilog if tool.epilog else None),
            formatter_class=argparse.RawTextHelpFormatter,
            add_help=(False if tool.command == 'do' else True))
        tool.fillParser(toolParser)

    # Activate completion
    argcomplete.autocomplete(parser)

    # Parse command-line
    args, unknown = parser.parse_known_args()

    # launch sam tool
    tool = tools.get(args.samSubCommand)
    if tool:
        tool.run(parser)
