""" Upload binaries bundles on Drive with a command-line interface.
    Based on pydrive.
    For auth, see http://pythonhosted.org/PyDrive/quickstart.html#authentication

"""
import argparse
import os
try:
    from pydrive.auth import GoogleAuth
    from pydrive.drive import GoogleDrive
except ImportError as import_err:
    MSG = str(import_err)
    if "No module named" in MSG:
        raise ImportError("{0}\nThis script is based on PyDrive module.\n"
                          "Try \"pip install PyDrive\".".format(MSG))
    else:
        raise


class BundleArbo(object):
    """ Arbo definition
        -- downloads
            |_ alt build
                |_ MacOSX 10.7
                    |_ clang
                        |_ Python 2.6
                            |_ develop
                                |_ debug
                                |_ (...)
                            |_ (...)
                        |_ (...)
                    |_ (...)
                |_ (...)
            |_ continuous integration
                |_ last versions
                    < same arbo as in alt build >
                |_ all versions
                     < same arbo as in alt build >

    """
    def __init__(self):
        self.root_directory = "downloads"
        # Bundle type
        self.alt_build = "alternative_builds"
        self.continuous_integration = "continuous_integration"
        # For continuous integration, last or all versions
        self.last_versions = "last_versions"
        self.all_versions = "all_versions"
        self.build_versions = [self.last_versions, self.all_versions]
        # Build modes
        self.production = 'production'
        self.release = 'release'
        self.debug = 'debug'
        self.modes = [self.production, self.release, self.debug]
        # Branch
        self.master = "master"
        self.develop = "develop"
        self.alt_branch = "alt_branch"
        self.branch_types = [[self.master, self.modes],
                        [self.develop, self.modes],
                        [self.alt_branch, self.modes]]
        # Python versions
        self.python26 = "python2.6"
        self.python27 = "python2.7"
        self.python33 = "python3.3"
        self.python_versions = [[self.python26, self.branch_types],
                           [self.python27, self.branch_types],
                           [self.python33, self.branch_types]]
        # Compilers
        self.gcc = 'gcc'
        self.clang = 'clang'
        self.msvc10 = 'msvc10'
        self.unix_compilers = [
                            [self.gcc, self.python_versions],
                            [self.clang, self.python_versions]
                          ]
        self.windows_compilers = [
                             [self.gcc, self.python_versions],
                             [self.clang, self.python_versions ],
                             [self.msvc10, self.python_versions ]
                             ]
        # OS
        self.opensuse12 = "OpenSuse12.1_64"
        self.win7 = "Windows7_64"
        self.winxp = "WindowsXP_32"
        self.mac107 = "MacOSX_10.7"
        self.mac108 = "MacOSX_10.8"
        self.os_types = [
                    [self.opensuse12, self.unix_compilers],
                    [self.win7, self.windows_compilers],
                    [self.winxp, self.windows_compilers],
                    [self.mac107, self.unix_compilers],
                    [self.mac108, self.unix_compilers]
                   ]
        # Final tree. To simplify process alt_build hierarchy is similar to
        # continuous_integration one
        self.tree = [self.root_directory,
                    [self.alt_build, self.os_types],
                    [self.continuous_integration,
                     [self.all_versions, self.os_types],
                     [self.last_versions, self.os_types]
                    ],
                ]
ARBO = BundleArbo()


class DriveHelper(object):
    """ Interface to get folder id, create folder and upload file

    """
    def __init__(self):
        self.gauth = GoogleAuth()
        self.gauth.LocalWebserverAuth()
        self.drive =  GoogleDrive(self.gauth)
        self.items_list = None
        self.list_dirty = False

    def get_items_list(self, cache=True):
        """ Get a list of Drive items created using the API (and not trashed)

        kwargs:
            cache (bool): getting list of items can be slow. Cache option allows
            to work with a cached instance of it.

        Returns:
            .list of drive items

        """
        if not cache or self.items_list is None or self.list_dirty:
            self.items_list = self.drive.ListFile({'q': "trashed=false"}
                                                   ).GetList()
            self.list_dirty = False
        return self.items_list

    def get_folder_by_name_and_parent(self, folder_name, parent_id):
        """ Get folder id from name and parent id
            Note that in the generated tree, each child has only one parent.

        Args:
            folder_name (str): ex "downloads"

            parent_id (str): ex 0B5IZz9Tq2HH0aTQxYzVldkN4UjQ

        Returns:
            .str id or None

        """
        for item in self.get_items_list():
            if (item['title'] == folder_name and
                (parent_id is None or parent_id in item['parents'][0]['id'])):
                return item['id']
        return None

    def get_folder_by_path(self, drive_path):
        """ Get folder id by browsing arbo from drive_path[0] (root) to
            drive_path[n]

        Args:
            drive_path ([str]): ex ['downloads', 'alternative_builds',
            'OpenSuse12.1_64', 'gcc', 'python2.6', 'develop', 'production']

        Returns:
            .str id

        """
        parent_id = None
        for directory in drive_path:
            res = self.get_folder_by_name_and_parent(directory, parent_id)
            if res is None:
                res = self.create_directory(directory, parent_id,
                                             check_exists=False)
            parent_id = res
        return parent_id

    def create_directory(self, folder_name, parent_id=None, check_exists=True):
        """ Create a folder under a parent and return its id

        Args:
            folder_name (str): ex "downloads"

        Kwargs:
            parent_id (str): ex 0B5IZz9Tq2HH0aTQxYzVldkN4UjQ

            check_exist (bool): if true, check if folder exists and if so
            returns directly the id.

        Returns:
            .str id

        """
        if check_exists:
            folder_id = self.get_folder_by_name_and_parent(folder_name,
                                                           parent_id)
            if folder_id:
                print ("{0} already exists and"
                       " won't be re-created").format(folder_name)
                return folder_id
        params = {'title': folder_name,
                  'mimeType': "application/vnd.google-apps.folder"
                 }

        if parent_id:
            params['parents'] = [{'id': parent_id}]
        folder_handle = self.drive.CreateFile(params)
        folder_handle.Upload()
        self.list_dirty = True
        return folder_handle['id']


    def upload_file(self, filepath, drive_path, title, description):
        """ Upload a file in a drive folder

        Args:
            filepath (str): file path

            drive_path ([str]): ex ['downloads', 'alternative_builds',
            'OpenSuse12.1_64', 'gcc', 'python2.6', 'develop', 'production']

            title (str): title that will be displayed in Drive

            description (str): description that will be displayed in Drive

        """
        parent_id = self.get_folder_by_path(drive_path)
        params = {
                  'title': title,
                  'description': description,
                  'parents': [{'id': parent_id}]
                 }
        file_handle = self.drive.CreateFile(params)
        file_handle.SetContentFile(filepath)
        # Upload options:
        # https://developers.google.com/drive/v2/reference/files/insert
        params = {
                  'convert': False
                 }
        file_handle.Upload(param=params)
        basename = os.path.basename(filepath)
        print "File {0} was uploaded in {1} (folder id: {2}).".format(basename,
                                                                     drive_path,
                                                                     parent_id)

    def create_arbo(self, tree, parent_id=None):
        """ Create arbo recursively in Drive
            Call : drive.create_arbo(ARBO.tree).
            Doesn't re-create folders when they exist.

            /!/ --- As Arbo as already been generated ---
            This function should'nt be used anymore.
            Exception : changes in arbo structure. But in that
            case be aware, it will affects in place folders and bundles.

        """
        build_parent = parent_id
        for node in tree:
            if not isinstance(node, (list, tuple)):
                build_parent = (self.create_directory(node, parent_id))
            else:
                self.create_arbo(node, build_parent)


def upload_file(args):
    """ Upload file

    Args:
        args : argparse options

    """
    # Get drive path as a folder list
    drive_path = [ARBO.root_directory]
    if args.alt_build:
        drive_path.append(ARBO.alt_build)
    else:
        drive_path.append(ARBO.continuous_integration)
        if args.ci_build_last:
            drive_path.append(ARBO.last_versions)
        else:
            drive_path.append(ARBO.all_versions)
    drive_path.append(args.os)
    drive_path.append(args.compiler)
    drive_path.append(args.py)
    drive_path.append(args.branch)
    drive_path.append(args.mode)

    # Get params
    title = args.title
    description = args.description
    if title is None:
        title = os.path.basename(args.filepath)
    if description is None:
        description = ("TuttleOFX bundle for {0}\n"
                       "Build infos : \n"
                       "--> Compiler : {1}\n"
                       "--> Python version : {2}\n"
                       "--> Branch : {3}\n"
                       "--> Mode : {4}").format(args.os, args.compiler,
                                             args.py, args.branch,
                                             args.mode)
    # Call upload function
    drive = DriveHelper()
    drive.upload_file(args.filepath, drive_path, title, description)

def __get_options():
    """Return option parser

    Returns:
        .argparse.ArgumentParser.args

    """
    ## Define parser
    description = 'Upload a file on google drive'
    parser = argparse.ArgumentParser(description=description)
    # File to upload
    parser.add_argument("filepath", help="file to upload", type=str)

    ## General params
    # OS
    parser.add_argument("os", help="OS type", type=str,
                        choices=[ARBO.opensuse12, ARBO.win7, ARBO.winxp,
                                 ARBO.mac107, ARBO.mac108])
    # Compiler
    parser.add_argument("compiler", help="compiler type", type=str,
                        choices=[ARBO.gcc, ARBO.clang, ARBO.msvc10])
    # Python version
    parser.add_argument("py", help="Python version", type=str,
                        choices=[ARBO.python26, ARBO.python27, ARBO.python33])
    # Branch
    parser.add_argument("branch", help="Branch",
                        type=str,
                        choices=[ARBO.master, ARBO.develop, ARBO.alt_branch])
    # Build mode
    parser.add_argument("mode", help="build mode", type=str,
                        choices=[ARBO.production, ARBO.release, ARBO.debug])
    # Build type
    excusive_group = parser.add_mutually_exclusive_group(required=True)
    excusive_group.add_argument("--alt-build",
                               help="alternative builds",
                               action='store_true')
    excusive_group.add_argument("--ci-build-last",
                               help="continuous integration, last builds",
                               action='store_true')
    excusive_group.add_argument("--ci-build-all",
                               help="continuous integration, all builds",
                               action='store_true')
    # Title
    parser.add_argument("--title", help="Title that'll be displayed in Drive",
                        type=str, default=None)
    # Description
    parser.add_argument("--description",
                        help="Description that'll be displayed in Drive.",
                        type=str, default=None)
    return parser.parse_args()

if __name__ == '__main__':
    upload_file(__get_options())
