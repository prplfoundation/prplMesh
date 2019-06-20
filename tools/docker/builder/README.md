# Docker builder image

This folder includes a Dockerfile which can be used to build prplMesh from a container by running maptools.py from the container.

It includes 2 helper scripts - one to generate the image `image-build.sh` and one to actually run `maptools.py build` from the container - `build.sh`.

## Usage

Build the initial image (one time):

```bash
sudo ./image-build.sh
```

Build prplMesh in container:

```bash
sudo ./build.sh <maptools.py build arguments>
```

---

Proxy settings - the recommended way to work with docker behind a proxy is by [configuring the docker client to pass proxy information to containers automatically](https://docs.docker.com/network/proxy/).

---
