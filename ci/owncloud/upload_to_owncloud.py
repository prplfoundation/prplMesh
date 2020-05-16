import argparse
from os import path
from getpass import getuser
import netrc
from webdav3.client import Client
from uuid import uuid4
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


def upload(webdav_client, is_direct, remote_path, local_path, unique_id=None):
    """
    upload(webdav_client, is_direct, remote_path, local_path, unique_id)

    Uploads the content of a directory to a directory in the remote webdav server

    The remote directory's tail is a unique directory name.

    Parameters
    ----------

    webdav_client : webdav3.client.Client
        the remote dav server's client object
    is_direct : bool
        flag dictating whether the directory is uploaded directly into the remote path
        or copied into a temporary folder first, and then into the remote path.

    remote_path : str
        path in the webdav server to be created and for the files to be copied to

    local_path : str
        local path to be copied from

    unique_id : str, optional
        a uuid to act as the identifier of the uploaded files in the remote directory
    """
    if not unique_id:
        unique_id = str(uuid4())
    message("""Uploading files from local directory {0} to remote directory {1},\n \
         username is {2}, remote directory id is: {3}""".format(
        local_path, remote_path, webdav_client.webdav.options["login"], webdav_client, unique_id))
    remote_full_path = path.join(remote_path, unique_id)
    mkdir_recursively(remote_full_path)
    if not path.isdir(local_path):
        raise FileNotFoundError("Local path location is not a directory")
    if is_direct:
        webdav_client.upload_sync(remote_full_path, local_path)
    else:
        remote_temp_path = path.join("temp_uploads", unique_id)
        mkdir_recursively(remote_temp_path)
        webdav_client.upload_sync(remote_temp_path, local_path)
        webdav_client.copy(remote_temp_path, remote_full_path)
    message("Success")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("--verbose", "-v", action='store_true', default=False,
                        help="Enable verbosity.")
    parser.add_argument("--direct", "-d", action='store_true', default=False,
                        help="Direct upload without first uploading to a temporary directory")
    parser.add_argument("--url", "-u", action='store', nargs=1,
                        default="https://ftp.essensium.com/owncloud/remote.php/dav/files/{0}"
                        .format(getuser()),
                        help="webDAV URL")
    parser.add_argument("local-path", nargs=1, type=str, action='store',
                        help="Local file or folder to upload")
    parser.add_argument("remote-path", type=str, nargs=1, action='store',
                        help="Path in the cloud, relative to the user home to upload")
    args = parser.parse_args()
    args.local_path = getattr(args, "local-path")[0]
    args.remote_path = getattr(args, "remote-path")[0]
    opts.verbose = args.verbose
    webdav_options = get_credentials()
    client = Client(webdav_options)
    upload(client, args.direct, args.remote_path, args.local_path)
