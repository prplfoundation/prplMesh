from os import path
from getpass import getuser
from webdav3.client import Client
from opts import debug, message, opts

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


