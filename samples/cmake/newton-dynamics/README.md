You must clone newton-dynamics first; also because of a bug somewhere, you must run CMake twice:

```
git clone https://github.com/MADEAPPS/newton-dynamics.git --branch v3.14c --depth=1
mkdir build
cd build
${NXDK_DIR}/usr/bin/nxdk-cmake ..
${NXDK_DIR}/usr/bin/nxdk-cmake ..
make
```
