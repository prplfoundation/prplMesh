# prplMesh manifest
Deploy all prplMesh repos in a single tree using google repo tool

## Requirements
Download & install google repo - https://source.android.com/setup/build/downloading

## Initialize & sync
Optional - to avoid ssh keys and password reprompt, add the following to ~/.gitconfig:
```
[url "https://github.com/"]
        insteadOf = git@github.com:

[url "https://github.com/"]
        insteadOf = ssh://git@github.com/

```
Then run the following commands to fetch the code:
```
repo init -u https://github.com/prplfoundation/intel_multiap_manifest.git
repo sync
repo forall -p -c 'git checkout $REPO_RREV'
```
> **_NOTE:_**  DWPAL is currently private (will be public in a week's time), which will cause repo init to fail on fetch. To workaround that, `no-dwpal.xml` is provided - so use the following repo init command instead: `repo init -u https://github.com/prplfoundation/intel_multiap_manifest.git -m no-dwpal.xml`. Note that without DWPAL, only DUMMY mode is supported.

## Build Instructions
Each component can be built with CMAKE, or use the [tools/maptools.py](tools/README.md) build command.
