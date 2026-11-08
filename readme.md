# snappy-tcl

Tcl 8.6 and 9.x bindings for [Snappy](https://github.com/google/snappy),
a fast compressor/decompressor. The Tcl package is named `snappy`; the project
and ttrek package are named `snappy-tcl`.

Version 1.0.5 uses Tcl stubs for shared builds and supports static embedding.
Build separately against Tcl 8.6 or Tcl 9 headers and the matching stub archive.
The shared extension does not link the Tcl runtime library.

## Build Snappy

Requirements: CMake 3.22.1 or newer, a C++11 compiler, and Tcl development
headers and stubs. These commands build Snappy 1.3.1 as a static library with
position-independent code so it can be linked into the extension:

```sh
wget https://github.com/google/snappy/archive/refs/tags/1.3.1.tar.gz
tar -xzf 1.3.1.tar.gz
cmake -S snappy-1.3.1 -B snappy-build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$HOME/.local" \
  -DCMAKE_INSTALL_LIBDIR=lib \
  -DBUILD_SHARED_LIBS=OFF \
  -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
  -DSNAPPY_BUILD_TESTS=OFF \
  -DSNAPPY_BUILD_BENCHMARKS=OFF
cmake --build snappy-build --parallel
cmake --install snappy-build
```

An existing Snappy installation with `SnappyConfig.cmake` also works.
Set `SNAPPY_DIR` to its prefix, or use `CMAKE_PREFIX_PATH` / `Snappy_DIR`.

## Build the Tcl extension

From this repository:

```sh
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$HOME/.local" \
  -DSNAPPY_DIR="$HOME/.local" \
  -DTCL_INCLUDE_DIR=/usr/local/include \
  -DTCL_LIBRARY_DIR=/usr/local/lib
cmake --build build --parallel
ctest --test-dir build --output-on-failure
cmake --install build
```

Point the Tcl directories at the desired installation. For precise selection,
set `TCL_INCLUDE_PATH`, `TCL_STUB_LIBRARY`, and `TCL_TCLSH` directly; for example,
`/usr/include/tcl8.6`, `/usr/lib/x86_64-linux-gnu/libtclstub8.6.a`, and
`/usr/bin/tclsh8.6` on Debian. Tests require a matching Tcl shell with `tcltest`;
set `BUILD_TESTING=OFF` when building without a runnable interpreter.

The library and `pkgIndex.tcl` install under `lib/snappy-tcl1.0.6`. Examples
install under `share/snappy-tcl/examples`.

### Static embedding

Configure with `-DBUILD_SHARED_LIBS=OFF` to install `libsnappy-tcl.a` without a
`pkgIndex.tcl`. Link the application with that archive, Snappy, the C++ runtime,
and the matching Tcl runtime. In a C++ host, register the module for later
loading with a null interpreter:

```cpp
extern "C" int Snappy_Init(Tcl_Interp *);
#if TCL_MAJOR_VERSION >= 9
Tcl_StaticLibrary(nullptr, "Snappy", Snappy_Init, nullptr);
#else
Tcl_StaticPackage(nullptr, "Snappy", Snappy_Init, nullptr);
#endif
```

Tcl code can then use:

```tcl
load {} Snappy
package require snappy 1.0.5
```

### NaviServer

The separate NaviServer Makefile remains available. With Snappy installed on
the compiler's include and library paths, run `make NAVISERVER=/usr/local/ns`
and `make NAVISERVER=/usr/local/ns install` from this repository.

## Commands

| Command | Result |
| --- | --- |
| `snappy::compress bytes` | Compressed byte array. |
| `snappy::uncompress bytes` | Original byte array; raises a Tcl error for invalid or truncated data. |
| `snappy::is_valid_compressed bytes` | `1` for a complete valid raw block, otherwise `0`. |
| `snappy::max_compressed_length num_uncompressed_bytes` | Upper bound on compressed size. Accepts integers from 0 through 4294967295, subject to platform size limits. |
| `snappy::get_uncompressed_length bytes` | Size advertised by the block header. Raises an error for an invalid header; does not validate the payload. |

These commands use **raw Snappy blocks**, not the framed stream format or a
`.sz` container. The raw format limits each input to 2^32 − 1 bytes; Tcl byte
array limits also apply. Compression and decompression hold the whole block in
memory. Length results use Tcl wide integers.

For text, use `encoding convertto utf-8` before compression and
`encoding convertfrom utf-8` after decompression. For byte arrays, use
`string length` to count bytes, not `string bytelength`.

## Examples

Use the Tcl shell matching the build. With the installation above:

```sh
export TCLLIBPATH="$HOME/.local/lib"
tclsh examples/example.tcl   # Unicode text and compression statistics
tclsh examples/binary.tcl    # All byte values and corrupt-input handling
tclsh examples/file.tcl compress input.bin output.snappy
tclsh examples/file.tcl uncompress output.snappy restored.bin
cmp input.bin restored.bin
```

The file example uses binary channels and refuses to overwrite an existing
output file. It reads the entire file into memory.

With the ttrek registry recipe and shared package installed:

```sh
ttrek run -package snappy-tcl -script demo
ttrek run -package snappy-tcl -script demo-binary
ttrek run -package snappy-tcl -script demo-file
```
