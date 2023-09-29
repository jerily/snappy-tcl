# snappy-tcl

TCL bindings for [snappy](https://github.com/google/snappy), a fast compressor/decompressor.
Use them in TCL or NaviServer as loadable modules.

## Build Dependency

Download latest stable version on Linux:
```bash
wget https://github.com/google/snappy/archive/refs/tags/1.1.10.tar.gz
tar -xzf 1.1.10.tar.gz
cd snappy-1.1.10
```

Download latest version from git on MacOS:
```bash
git clone https://github.com/google/snappy.git
cd snappy
```

Build and install:
```bash
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
wget https://github.com/jerily/snappy-tcl/archive/refs/tags/v1.0.3.tar.gz
tar -xzf v1.0.3.tar.gz
export TCL_SNAPPY_DIR=$(pwd)/snappy-tcl-1.0.3
cd ${TCL_SNAPPY_DIR}
mkdir build
cd build
cmake .. \
  -DTCL_LIBRARY_DIR=/usr/local/lib \
  -DTCL_INCLUDE_DIR=/usr/local/include \
  -DSNAPPY_DIR=/usr/local
make
make install
```

## Build for NaviServer
```bash
cd ${TCL_SNAPPY_DIR}/
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
* **::snappy::max_compressed_length** *uncompressed_bytes*
    - returns the maximum length of a compressed byte array
* **::snappy::get_uncompressed_length** *compressed_bytes*
    - returns the length of the uncompressed byte array
