""" Helper script that prints glibc dependencies > to a given glibc version

"""
import argparse
import os
import mimetypes
import subprocess
from distutils.version import StrictVersion


def get_bin_path(in_path):
    """ Return binaries pathes
        TODO Improve with extension filtering (*.png, ...)

    """
    if os.path.isdir(in_path):
        bin_paths = []
        for res in os.walk(in_path):
            # check out files
            for afile in res[2]:
                mime = mimetypes.guess_type(afile)
                if "text" not in mime:
                    bin_paths.append(os.path.join(res[0], afile))
    else:
        bin_paths = [in_path]
    return bin_paths


def get_glibc_functions(bin_path, glibc_version, verbose, libc_path, libm_path):
    """ Analyse objdump informations, get glibc functions > glibc_version
    and print former glibc dependent versions of the very same functions
    found in libm and libc

    """
    bin_paths = get_bin_path(os.path.abspath(bin_path))
    functions = []
    # Looking for GLIBC dependencies with objdump -T
    for path in bin_paths:
        process = subprocess.Popen(["objdump", "-T", path],
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE)
        result = process.stdout.readlines()
        for function in result:
            index = function.find("GLIBC_")
            if index >= 0:
                subfunction = function[index:].rstrip()
                index = subfunction.find(" ")
                if index:
                    version = subfunction[6:index]
                    # Compare versions
                    if StrictVersion(version) >= StrictVersion(glibc_version):
                        if verbose:
                            print ("{0} glibc found in {1}:\n{2}"
                                   ).format(version,
                                            os.path.basename(path),
                                            function)
                        functions.append(subfunction[index+1:])
    if len(functions) == 0:
        "No dependencies > to {0} found.".format(glibc_version)
        return
    functions = list(set(functions))
    for function in functions:
        process = subprocess.Popen(["readelf", "-s", libc_path],
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE)
        process2 = subprocess.Popen(["readelf", "-s", libm_path],
                                    stdout=subprocess.PIPE,
                                    stderr=subprocess.PIPE)
        result = process.stdout.readlines() + process2.stdout.readlines()
        print "====== for {0}:".format(function)
        for entry in result:
            if function in entry:
                print entry


def __get_options():
    """Return option parser

    Returns:
        .argparse.ArgumentParser.args

    """
    # # Define parser
    description = 'Print glibc dependencies > to a given glibc version'
    parser = argparse.ArgumentParser(description=description)
    parser.add_argument("bin_path",
                        help=("path of a binary or if a directory"
                              " containing binaries"),
                        type=str)
    parser.add_argument("--glibc-version",
                        help="Minimum glibc version. Default: 2.12",
                        type=str,
                        default="2.12")
    parser.add_argument("--libc-path",
                        help="libc path. Default: /lib/libc.so.6",
                        type=str,
                        default="/lib/libc.so.6")
    parser.add_argument("--libm-path",
                        help=("libm path. Default: /lib/libm.so.6"),
                        type=str,
                        default="/lib/libm.so.6")
    parser.add_argument("-v", "--verbose",
                        action="store_true",
                        help="Print log")
    return parser.parse_args()


if __name__ == '__main__':
    ARGS = __get_options()
    get_glibc_functions(ARGS.bin_path, ARGS.glibc_version, ARGS.verbose,
                        ARGS.libc_path, ARGS.libm_path)

