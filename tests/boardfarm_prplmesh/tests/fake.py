from boardfarm.tests import rootfs_boot


class Fake(rootfs_boot.RootFSBootTest):
    '''lsmod shows loaded kernel modules.'''
    def runTest(self):
        print("Fake.runTest")

