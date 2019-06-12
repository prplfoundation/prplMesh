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
## Build Instructions
Each component can be built with CMAKE, or use the [tools/maptools.py](tools/maptools.py) build command.
