from contextlib import contextmanager
import sys, os
from sys import platform
import pip

old_stderr = sys.stderr

class dummmy_stderr():
    def __init__(self):
        return
    def write(self,str):
        return
    def flush(self):
        return

required_packages_versions = [('matplotlib',''),('pyside',''),('numpy',''),('paramiko',''),('networkx','1.10')]

def PRINTF(fmt, *args):
    sys.stdout.write(fmt % args)
    sys.stdout.flush()

def install_required_packages():
    global required_packages_versions
    global old_stderr

    old_stderr = sys.stderr
    sys.stderr = dummmy_stderr()

    installed_packages = [pkg.key for pkg in pip.get_installed_distributions()]
    installed_packages_versions = {package.key: package.version for package in pip.get_installed_distributions()}

    for package_version in required_packages_versions:
        if ('pyside' in package_version[0]) and ('linux' in platform):
            try:
                from PySide import QtCore, QtGui
            except:
                pkg = package_version[0]
                try:
                    pip.main(['install', pkg])
                except:
                    PRINTF("install_required_packages()  --> could not install package: %s, please install it manually\n", pkg)
        elif package_version[0] not in installed_packages:
            pkg = package_version[0]
            if package_version[1]:#version specified
                pkg += "==" + package_version[1]
            try:
                pip.main(['install', pkg])
            except:
                PRINTF("install_required_packages()  --> could not install package: %s, please install it manually\n", pkg)
        elif (package_version[1]) and  not (package_version[1] in installed_packages_versions.get(package_version[0])):
            pkg = package_version[0] + "==" + package_version[1]
            try:
                pip.main(['install', pkg])
            except:
                PRINTF("install_required_packages()  --> could not install package: %s, please install it manually\n", pkg)
    sys.stderr = old_stderr