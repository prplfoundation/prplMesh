# MultiAP Tools
This project contains maptools.py, a utility for building and deploying multiap SW components.
The directory hirerchy expected by maptools.py is as follows:

```
multiap
└───framework
└───common
└───controller
└───agent
└───tools
│   │   maptools.py
```

Building and installing create build and install directories:
```
multiap
└───build
|   └───install
└───tools
│   │   maptools.py
```


## Prerequisites
<a name="prereq"></a>
Install dependencies listed in [requirements.txt](requirements.txt):

```
pip install -r requirements.txt
```

None sudo users can add `--user` to the command 

## maptools build & install
By default, maptools build command uses ../build folder for building and installing.
Therefore in order to run multiap binaries/libs, need to add ```LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<...>/build/install/lib``` to the command.

get a list of all supported subcommands:
```
./maptools.py --help
```
subcommand help:
```
./maptools.py <subcommand> --help
```
### Examples
build map (default):
```
./maptools.py build
```

build only dep modules(nng, safeclib, dwpal):
```
./maptools.py build dep -n
```

Build map with nng messaging library (needs to have nng installed):
```
./maptools.py build map -f MSGLIB=nng
```

Clean and rebuild controller only:
```
./maptools.py build map -c clean make
```

Distclean (removes ../build folder):
```
./maptools.py build map -c clean
```

To build with ninja (need to install the ninja tool in your distro first):
```
./maptools.py build map -G Ninja
```
Other generators (eclipse, codeblocks, ...) are available as well. Use
`cmake -G` to list them. Note that the `--make-verbose` option only works with
the Unix Makefiles generator.

## Versioning
<a name="ver"></a>
1.4.0

## Authors
<a name="authors"></a>
* **Tomer Eliyahu**  tomer.b.eliyahu@intel.com

## License
<a name="license"></a>
This project is licensed under the BSD+Pantet License - see the [LICENSE](LICENSE) file for details
