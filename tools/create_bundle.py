""" Command line script to create TuttleOFX bundle.
Copy plugins, bins, python binding.
Detect shared libs and copy them (Linux only for the moment)

"""
__version__ = "0.1"
import argparse
import subprocess
import sys
import platform
import os
import errno
import shutil
import ntpath


OS_NAME = os.name.lower()
SYS_PLATFORM = sys.platform.lower()
OS_ARCH = 64 if sys.maxsize > 2**32 else 32

SHARED_LIB_EXT = ['.dylib', '.so', '.dll']


def create_dir(directory, overwrite=False):
    """Create a directory with overwrite option

    Args:
        directory (str): path to a directory

    Kwargs:
        overwrite (bool): if True, overwrite when directory exists ; else an
        error is raised.

    """
    try:
        os.makedirs(directory)
    except OSError as os_err:
        if os_err.errno == errno.EEXIST and os.path.isdir(directory):
            if not overwrite:
                print ("Error : output directory already exists ({0})\n"
                       "Try --overwrite option.").format(directory)
            else:
                shutil.rmtree(directory)
                os.makedirs(directory)
        else:
            raise


def copytree_verbose_condition(path):
    """Verbose condition to help printing usefull informations only

    Args:
        path (str): a path

    """
    return (os.path.isfile(path) and not path.endswith(".png")
            and not path.endswith(".svg"))


def copytree(src, dst, symlinks=False, ignore=None, verbose=False):
    """This function is a copy of shutil.copytree in which a verbose option was
    added

    """
    names = os.listdir(src)
    if ignore is not None:
        ignored_names = ignore(src, names)
    else:
        ignored_names = set()

    os.makedirs(dst)
    errors = []
    for name in names:
        if name in ignored_names:
            continue
        srcname = os.path.join(src, name)
        dstname = os.path.join(dst, name)
        try:
            if symlinks and os.path.islink(srcname):
                linkto = os.readlink(srcname)
                os.symlink(linkto, dstname)
            elif os.path.isdir(srcname):
                copytree(srcname, dstname, symlinks, ignore, verbose)
            else:
                shutil.copy2(srcname, dstname)
                # Verbose option
                if verbose:
                    if copytree_verbose_condition(srcname):
                        print "Copy {0}".format(srcname)
        except (IOError, os.error), why:
            errors.append((srcname, dstname, str(why)))
        # catch the Error from the recursive copytree so that we can
        # continue with other files
        except shutil.Error, err:
            errors.extend(err.args[0])
    try:
        shutil.copystat(src, dst)
        # Verbose option
        if verbose:
            if copytree_verbose_condition(src):
                print "Copy {0}".format(src)
    except OSError, why:
        if (shutil.WindowsError is not None
           and isinstance(why, shutil.WindowsError)):
            # Copying file access times may fail on Windows
            pass
        else:
            errors.extend((src, dst, str(why)))
    if errors:
        raise shutil.Error(errors)


def ignored_ext(filenames, ext):
    """Functor for copy tree ignore.
    Return files that match extension.

    Args:
        filenames: list of filenames

        ext (str): extension to ignore

    """
    return [filename for filename in filenames if filename.endswith(ext)]


def ignored_pyc(directory, filenames):
    """Functor for copy tree ignore.
    Return files that match extension pyc.

    Args:
        directory: mandatory param, not used here

        filenames: list of filenames

    """
    return ignored_ext(filenames, ".pyc")


def copy_tree(source_dir, dest_dir, overwrite=False, ignore=None,
              verbose=False):
    """Copy a tree with overwrite, ignore files and verbose options

    Args:
        source_dir (str): path to source dir

        dest_dir (str): path to dest dir

    Kwargs:
        overwrite (bool): if True, overwrite when directory exists ; else an
        error is raised.

        ignore (fctor): functor that return a list of files/dirs to exclude

        verbose (bool): print verbose

    """
    if not overwrite:
        copytree(source_dir, dest_dir, verbose=verbose)
        return
    if os.path.exists(dest_dir):
        shutil.rmtree(dest_dir)
    copytree(source_dir, dest_dir, ignore=ignore, verbose=verbose)


def copy_bin(parent_source_dir, parent_dest_dir, child_dir, overwrite=False,
             ignore=None, verbose=False):
    """Copy a directory in another directory with overwrite, ignore files and
    verbose options

        Args:
        parent_source_dir (str): root path to source dir.
        Ex: dist/machine/gcc4.6/production

        parent_dest_dir (str): root path to dest dir
        Ex: /datas/tuttle_release/

        child_dir (str): child directory name. Ex: bin, plugin, lib, python.

    Kwargs:
        overwrite (bool): if True, overwrite when directory exists ; else an
        error is raised.

        ignore (fctor): functor that return a list of file/dir to exclude

        verbose (bool): print verbose

    """
    if verbose:
        print_with_deco("Copy {0}".format(child_dir))
    build_plugin_dir = os.path.join(parent_source_dir, child_dir)
    out_plugin_dir = os.path.join(parent_dest_dir, child_dir)
    copy_tree(build_plugin_dir, out_plugin_dir, overwrite, ignore, verbose)


def get_ofx_platform_name():
    """Get the standard openfx name of the current platform.
    One of the followings :
        * MacOS - for Apple Macintosh OS X (compiled 32 bit)
        * Win32 - for Microsoft Windows (compiled 32 bit)
        * IRIX - for SGI IRIX plug-ins (compiled 32 bit)
        * IRIX64 - for SGI IRIX plug-ins (compiled 64 bit)
        * Linux-x86 - for Linux on intel x86 CPUs (compiled 32 bit)
        * Linux-x86-64 - for Linux on intel x86 CPUs running AMD's 64 bit
        extensions

    Stolen from :
    https://github.com/tuttleofx/TuttleOFX/blob/master/SConstruct#L19
    """
    if OS_NAME == "posix":
        if SYS_PLATFORM.find("linux") >= 0:
            if OS_ARCH == 64:
                return 'Linux-x86-64'
            return 'Linux-x86'
        elif SYS_PLATFORM.find("cygwin") >= 0:
            if OS_ARCH == 64:
                return 'Linux-x86-64'
            return 'Linux-x86'
        elif SYS_PLATFORM.find("darwin") >= 0:
            return 'MacOS'
        elif SYS_PLATFORM.find("irix") >= 0:
            if OS_ARCH == 64:
                return 'IRIX64'
            return 'IRIX'
    elif OS_NAME == "nt":
        if SYS_PLATFORM.startswith("win"):
            return 'Win' + str(OS_ARCH)
        elif SYS_PLATFORM.find("cygwin") >= 0:
            return 'Win' + str(OS_ARCH)
    raise ValueError("Unknown Platform ({0},{1})".format(OS_NAME, SYS_PLATFORM))


def get_ofx_bin_path(plugins_dir):
    """Return ofx binary pathes

    Args:
        plugins_dir (str): a directory containing OFX bundle directories.

    """
    plugin_pathes = []
    for item in os.listdir(plugins_dir):
        path = os.path.join(plugins_dir, item)
        if not os.path.isdir(path):
            continue
        # construct the path to the .ofx
        path = os.path.join(path, "Contents", get_ofx_platform_name())
        # check if the .ofx exists and if so add it to the result
        for item in os.listdir(path):
            tmp_path = os.path.join(path, item)
            if os.path.isdir(tmp_path) or not tmp_path.endswith(".ofx"):
                continue
            # tmp_path contains .ofx path
            plugin_pathes.append(tmp_path)
            break
    return plugin_pathes


def get_exe_bin_path(bin_dir):
    """Return exe binary pathes

    Args:
        bin_dir (str): a directory containing exe files.

    """
    bin_pathes = []
    for item in os.listdir(bin_dir):
        path = os.path.join(bin_dir, item)
        if os.path.isfile(path):
            bin_pathes.append(path)
    return bin_pathes


def get_py_bin_path(py_dir):
    """Return python binding binary pathes

    Args:
        py_dir (str): a directory containing binding files.

    """
    bin_pathes = []
    for res in os.walk(py_dir):
        # check out files
        for afile in res[2]:
            if os.path.splitext(afile)[1] in SHARED_LIB_EXT:
                bin_pathes.append(afile)
    return bin_pathes


def get_shared_libs_path(output_dir, exclude_sys_lib=False):
    """Inspect binaries location to get shared dependencies

    Args:
        output_dir (str): a directory containing TuttleOFX build.

    Kwargs:
        exclude_sys_lib (boolean): exclude system libs.

    """
    if OS_NAME == "nt" and SYS_PLATFORM.startswith("win"):
        raise NotImplementedError("Auto-detection of shared libs isn't "
                                  "implemented for Windows yet.")
    bin_paths = get_ofx_bin_path(os.path.join(output_dir, "plugin"))
    bin_paths += get_exe_bin_path(os.path.join(output_dir, "bin"))
    bin_paths += get_py_bin_path(os.path.join(output_dir, "python"))
    # ldd on each plugin binary
    shared_libs = []
    for binary in bin_paths:
        process = subprocess.Popen(["ldd", binary],
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE)
        result = process.stdout.readlines()
        # analyse the result
        for lib in result:
            split_str = lib.split()
            if "not found" in lib:
                # quit if a lib is not found
                raise EnvironmentError(("{0} => {1} not found.\n"
                                        "Adding this lib to your lib path may "
                                        "be the solution."
                                        ).format(ntpath.basename(binary),
                                                 split_str[0]))
            elif "=>" in lib:
                if len(split_str) == 3:
                    # virtual library
                    pass
                else:
                    shared_libs.append(split_str[2])
            else:
                if len(split_str) == 2:
                    if os.path.isfile(split_str[0]):
                        shared_libs.append(split_str[0])
    # remove duplicates
    shared_libs = sorted(set(shared_libs))

    # filter if asked
    if not exclude_sys_lib:
        return shared_libs
    filtered_shared_libs = []
    for lib in shared_libs:
        if (lib.startswith('/usr/lib64') or lib.startswith('/lib64') or
            lib.startswith('/usr/lib')):
            continue
        filtered_shared_libs.append(lib)
    return filtered_shared_libs


def print_with_deco(msg):
    """Print with decoration

    Args:
        msg (str): message to print

    """
    size = len(msg) + 2
    print "{0}\n {1}\n{0}".format("-" * size, msg)


def write_file(output_dir, file_name, text):
    """Write text in file

    Args:
        output_dir (str): output directory

        filename (str): file name

        text (str): text to write

    """
    filepath = os.path.join(output_dir, file_name)
    file_handle = open(filepath, 'w')
    file_handle.write(text)
    file_handle.close()


def copy_sam_scripts(source_dir, output_dir):
    """Write sam.sh script in output_dir

    Args:
        source_dir (str)

        output_dir (str)

    """
    script_path = os.path.join(source_dir, "tools", "sam_scripts")
    if OS_NAME == "nt" and SYS_PLATFORM.startswith("win"):
        native_script = os.path.join(script_path, "sam.bat")
    else:
        native_script = os.path.join(script_path, "sam.sh")
    python_script = os.path.join(script_path, "sam.py")
    shutil.copy(native_script, os.path.join(output_dir, "sam"))
    shutil.copy(python_script, output_dir)
    os.chmod(native_script, 0755)


def write_readme(output_dir):
    """Write readme.md script in output_dir

    Args:
        output_dir (str)

    """
    readme = (
"""**[TuttleOFX.org](www.TuttleOFX.org)**  

To use sam command-line 
-----------------------
Add execution rights to sam.sh (if necessary) and TuttleOFX directory to your 
path.  
Then run sam, sam do -h, sam cp -h to see tools documentation.  

More examples of command-line [here](http://www.tuttleofx.org/user-documentation/command-line-examples).

To use plugins 
-----------------------
Plugins can be used in "sam do" or in any OpenFX compatible softwares.  

A quick way to load them in other softwares is to add Tuttle's plugin directory
to `OFX_PLUGIN_PATH` var env and Tuttle's lib directory to your env.

""")
    write_file(output_dir, "readme.md", readme)


def create_bundle(source_dir, build_dir, output_dir, overwrite=False,
                  exclude_sys_lib=False, verbose=False):
    """Create bundle by copying pertinent files in output_dir

    Args:
        source_dir (str): TuttleOFX source code directory

        build_dir (str): directory where binaries has been built

        output_dir (str): Bundle will be create in this directory

        overwrite (boolean): Overwrite output dir

    """
    # get system infos
    python_version = "{0}.{1}.{2}".format(sys.version_info[0],
                                          sys.version_info[1],
                                          sys.version_info[2])
    os_type = platform.system()
    if 'Linux' in os_type:
        os_type = platform.linux_distribution()[0].rstrip()
        os_version = platform.linux_distribution()[1]
    else:
        os_version = platform.release()
    os_full_name = "{0}{1}_{2}bits".format(os_type, os_version, OS_ARCH)
    # get Tuttle version
    process = subprocess.Popen(["git", "describe"],
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE)
    version = process.stdout.readline().rstrip()

    # create bundle directory
    bundle_name = "TuttleOFX_{0}_{1}_python{2}".format(version, os_full_name,
                                                       python_version)
    output_dir = os.path.join(output_dir, bundle_name)
    create_dir(output_dir, overwrite)

    print_with_deco("Bundle infos")
    print "OS: {0}\nTuttleOFX version: {1}\n".format(os_full_name, version)

    # copy plugins
    copy_bin(build_dir, output_dir, "plugin", overwrite, verbose=verbose)

    # copy bins
    copy_bin(build_dir, output_dir, "bin", overwrite, verbose=verbose)

    # copy python binding
    copy_bin(build_dir, output_dir, "python", overwrite, ignored_pyc,
             verbose=verbose)

    # get libav presets
    print_with_deco("Get libav presets")
    preset_repo = "https://github.com/MarcAntoine-Arnaud/libav-presets.git"
    libav_dir = os.path.join(output_dir, "libav-presets")
    process = subprocess.Popen(["git", "clone", preset_repo, libav_dir],
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE)
    if verbose:
        print process.stdout.readline()

    # write utils files
    print_with_deco("sam launcher")
    copy_sam_scripts(source_dir, output_dir)
    print_with_deco("readme.md")
    write_readme(output_dir)

    # fetch and copy shared libs
    # Warning : this considers that lib env var are setted correctly
    if verbose:
        print_with_deco("Copy shared libraries")
    shared_libs = get_shared_libs_path(output_dir,
                                       exclude_sys_lib)
    lib_dir = os.path.join(output_dir, "lib")
    create_dir(lib_dir, overwrite)
    for lib in shared_libs:
        shutil.copy(lib, lib_dir)
        if verbose:
            print "Copy {0}".format(lib)
    if verbose:
        print "{0} libs were copied.".format(len(shared_libs))

    print "\n=> Files were copied in {0}".format(output_dir)


def __get_options():
    """Return option parser

    Returns:
        .argparse.ArgumentParser.args

    """
    ## Define parser
    description = 'Create Tuttle bundle'
    parser = argparse.ArgumentParser(description=description)
    parser.add_argument("source_directory", help=("TuttleOFX source code"
                                                  "directory"), type=str)
    parser.add_argument("build_directory", help=("directory where binaries has"
                                                 " been built"), type=str)
    parser.add_argument("output_directory", help=("Bundle will be create in"
                                                  "this directory"), type=str)
    parser.add_argument("-o", "--overwrite", action="store_true",
                        help="Overwrite output directory")
    parser.add_argument("-e", "--exclude-sys-lib", action="store_true",
                        help="Do not bundle system libs")
    parser.add_argument("-v", "--verbose", action="store_true",
                        help="Print log")
    return parser.parse_args()

if __name__ == '__main__':
    ARGS = __get_options()
    create_bundle(ARGS.source_directory, ARGS.build_directory,
                  ARGS.output_directory, ARGS.overwrite,
                  exclude_sys_lib=ARGS.exclude_sys_lib, verbose=ARGS.verbose)
