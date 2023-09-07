# snappy-tcl

TCL bindings for [snappy](https://github.com/google/snappy).
Use them in TCL or NaviServer as loadable modules.

## Build Dependency

```bash
wget https://github.com/google/snappy/archive/refs/tags/1.1.10.tar.gz
tar -xzf 1.1.10.tar.gz
cd snappy-1.1.10
mkdir build
cd build
cmake .. \
  -DBUILD_SHARED_LIBS=ON \
  -DSNAPPY_BUILD_TESTS=OFF \
  -DSNAPPY_BUILD_BENCHMARKS=OFF
make 
make install
```

## Build TCL module
```bash
git clone https://github.com/jerily/snappy-tcl.git
mkdir build
cd build
cmake ..
make
make install
```

## TCL Commands
* **::snappy::compress** *bytes*
    - returns a compressed byte array
* **::snappy::uncompress** *bytes*
    - returns an uncompressed byte array
* **::snappy::is_valid_compressed** *bytes*
    - returns 1 if the byte array is a valid compressed byte array, 0 otherwise