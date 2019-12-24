### Running Klocwork on UGW/RDKB environment, on Intel Network

- Request access on ags.intel.com for the role:
    > DevTools - Klocwork - JF - CHD-P - CHD-P Developer

- Make sure you already set the following environment variables:
   ``` bash
    export CMAKE_TOOLCHAIN_FILE=/local/$USER/dev1/prplMesh/tools/cmake/toolchain/rdkb.cmake
    export PRPLMESH_PLATFORM_TYPE=rdkb
    export PRPLMESH_PLATFORM_BASE_DIR=/local/$USER/dev1/rdkb/atom_rdkbos/
    export PRPLMESH_PLATFORM_BUILD_NAME=puma7-atom
    export PRPLMESH_PLATFORM_TOOLCHAIN_PREFIX=i686-rdk-linux
    export PRPLMESH_PLATFORM_TOOLCHAIN=x86_64-linux

   ```

- Open a new shell and make sure you are using the new path:
    ``` bash
    which kwauth
    ```
    Answer should be:
    ```bash
    /nfs/site/proj/chdsw_ci/common/kw/klocwork_12_3/user/bin/kwauth
    ```

- Authenticate user with Klockwork server once per environment/server:
    **UGW:**
    ``` bash
    kwauth --url https://klocwork-iind4.devtools.intel.com:8105
    ```

    **RDKB:**
    ```bash
    kwauth --url https://klocwork3-jf.devtools.intel.com:8140/
    ```

    And use User and Password:

    > Login: [WIN_USER_NAME]
    > Password: [WIN_PASSWORD]

### Running Klocwork Linux environment, on non Intel Network
Make sure that 'external_toolchain.cfg' is not exist on your environment.

# Running Klocwork:
```bash
cd tools/klocwork/
./kw.sh
```
-- OR --
Trigger Klocwork task on VSCODE


###### Reference and installation guide:
https://wiki.ith.intel.com/display/CHDSE/Klocwork+on+Linux+Build+Servers+How-To
