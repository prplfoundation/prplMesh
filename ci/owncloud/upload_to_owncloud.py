from os import path
from getpass import getuser
import netrc
from webdav3.client import Client
import json
from opts import debug, message, opts

default_hostname = "https://ftp.essensium.com/owncloud/remote.php/dav/files/"


def get_credentials(options_path=None, force_netrc=True):
    """
    get_credentials(options_path=None, force_netrc=True)

    Gets the user's default credentials from either an external options file or ~/.netrc

    Parameters
    ----------
    options_path : str, optional
                path to the options file

    force_netrc : bool
                flag indicating whether to skip parsing the options
                file and skip straight to the contents of ~/.netrc
    Returns
    -------
    dictionary with 3 key/value pairs,
    keys are: {webdav_login", "webdav_password","webdav_hostname")
    """
    if not force_netrc and path.exists(options_path):
        with open(options_path) as options_file:
            options = json.load(options_file)
            return options

    netrc_file = netrc.netrc()
    credentials = netrc_file.authenticators("ftp.essensium.com")
    return {
        "webdav_login": credentials[0],
        "webdav_password": credentials[2],
        "webdav_hostname": default_hostname + credentials[0]
    }


def get_path_components(dirname):
    """
    get_path_components(dirname)

    Gets a list of all subpaths needed to build a directory, including itself.

    Parameters
    ----------
    dirname : str
        A given directory path
    Returns
    -------
    A python list of path strings
    For an input 'a/b/c' returns ['a/b/c', 'a/b', 'a']
    """
    path_list = [dirname, ]
    tmp = path.split(dirname)
    while tmp[0] != "" and not path.ismount(tmp[0]):
        path_list.append(tmp[0])
        tmp = path.split(tmp[0])
    path_list.reverse()
    return path_list


def mkdir_recursively(webdav_client, remote_path):
    """
    mkdir_recursively(remote_path)

    Recursively creates all directories needed to create the input path.

    Parameters
    ----------
    remote_path: str
    """
    pathlist = get_path_components(remote_path)
    for p in pathlist:
        try:
            webdav_client.mkdir(p)
        except BaseException as e:
            debug(e)


