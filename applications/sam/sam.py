#!/usr/bin/env python
# coding: utf-8 
# PYTHON_ARGCOMPLETE_OK

import sys
import os
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


def getSpecificSamTool(toolCommand):
    """
    Get the sam tool from the given name.
    """
    toolClass = 'Sam_' + toolCommand
    toolModule = __import__('sam_' + toolCommand, fromlist=[toolClass])
    toolClass = getattr(toolModule, toolClass)
    return {toolCommand: toolClass()}


def isCompletion():
    """
    If the script is executed to generate autocomplete choices.
    """
    return '_ARGCOMPLETE' and 'COMP_LINE' in os.environ


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

    if isCompletion():
        compLine = os.environ['COMP_LINE'].split()
        if len(compLine) > 1:
            toolCommand = compLine[1]
            tools = getSpecificSamTool(toolCommand)
        else:
            tools = getAllSamTools()
    elif len(sys.argv) < 2 or sys.argv[1].startswith('-'):
        tools = getAllSamTools()
    else:
        try:
            toolCommand = sys.argv[1]
            tools = getSpecificSamTool(toolCommand)
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
