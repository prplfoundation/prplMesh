#!/usr/bin/env python
import paramiko
import os

def put_dir(sftp, source, target):
    ''' Uploads the contents of the source directory to the target path. The
        target directory needs to exists. All subdirectories in source are 
        created under target.
    '''
    for item in os.listdir(source):
        if os.path.isfile(os.path.join(source, item)):
            sftp.put(os.path.join(source, item), '%s/%s' % (target, item))
        else:
            try:
                sftp.mkdir('%s/%s' % (target, item))
            except:
                pass                
            put_dir(sftp, os.path.join(source, item), '%s/%s' % (target, item))            

def copyToVM(ip, source, destination):
    username = "libit"
    password = "libit"
    port = 22
    t = paramiko.Transport((ip, port))
    t.connect(username=username, password=password)
    sftp = paramiko.SFTPClient.from_transport(t)
    put_dir(sftp, source, destination)

def copyToBoard(ip, source, destination):
    username = "libit"
    password = "libit"
    sshoptions = "-oUserKnownHostsFile=/dev/null -oStrictHostKeyChecking=no"
    command = 'sshpass -p "admin" scp -r {} {} admin@192.168.1.1:~/{}'.format(sshoptions, source, destination)
    print (command)
    port="22"

    client = paramiko.SSHClient()
    client.load_system_host_keys()
    client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    client.connect(ip, port=port, username=username, password=password)
    stdin, stdout, stderr = client.exec_command(command)
    print stdout.read()

def main():
    print("start")
    copyToVM("10.124.123.55", "chdlab", "/home/libit/chdlab")
    copyToBoard("10.124.123.55", "/home/libit/chdlab", "chdlab")
    print("done")

if __name__== "__main__":
  main()