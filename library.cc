/**
 * Copyright Jerily LTD. All Rights Reserved.
 * SPDX-FileCopyrightText: 2023 Neofytos Dimitriou (neo@jerily.cy)
 * SPDX-License-Identifier: MIT.
 */
#include <snappy.h>
#include <climits>
#include <cstdint>
#include <limits>
#include <new>
#include <stdexcept>
#include <string>
#include "library.h"

#ifndef TCL_SIZE_MAX
typedef int Tcl_Size;
# define TCL_SIZE_MAX      INT_MAX
#endif

#if TCL_MAJOR_VERSION < 9
# define Tcl_CreateObjCommand2 Tcl_CreateObjCommand
#endif

#define XSTR(s) STR(s)
#define STR(s) #s

#define CheckArgs(min, max, n, msg) \
                 if ((objc < min) || (objc >max)) { \
                     Tcl_WrongNumArgs(interp, n, objv, msg); \
                     return TCL_ERROR; \
                 }

static int snappy_Error(Tcl_Interp *interp, const char *code, const char *message) {
    Tcl_SetObjResult(interp, Tcl_NewStringObj(message, -1));
    Tcl_SetErrorCode(interp, "SNAPPY", code, nullptr);
    return TCL_ERROR;
}

static int snappy_CompressCmd(ClientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    CheckArgs(2, 2, 1, "bytes");

    Tcl_Size length;
    unsigned char *bytes = Tcl_GetByteArrayFromObj(objv[1], &length);
    // The raw Snappy format encodes the input size as an unsigned 32-bit value.
    if (static_cast<uint64_t>(length) > std::numeric_limits<uint32_t>::max()) {
        return snappy_Error(interp, "SIZE", "Input exceeds the Snappy block size limit");
    }
    try {
        std::string compressed;
        snappy::Compress(reinterpret_cast<const char *>(bytes), static_cast<size_t>(length), &compressed);
        if (compressed.size() > static_cast<size_t>(TCL_SIZE_MAX)) {
            return snappy_Error(interp, "SIZE", "Compressed data exceeds the Tcl byte array size limit");
        }
        Tcl_SetObjResult(interp, Tcl_NewByteArrayObj(
            reinterpret_cast<const unsigned char *>(compressed.data()), static_cast<Tcl_Size>(compressed.size())));
    } catch (const std::bad_alloc &) {
        return snappy_Error(interp, "MEMORY", "Unable to allocate compression buffer");
    } catch (const std::length_error &) {
        return snappy_Error(interp, "SIZE", "Compression buffer is too large");
    }
    return TCL_OK;
}

static int snappy_UncompressCmd(ClientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    CheckArgs(2, 2, 1, "bytes");

    Tcl_Size length;
    unsigned char *bytes = Tcl_GetByteArrayFromObj(objv[1], &length);
    const char *compressed = reinterpret_cast<const char *>(bytes);
    size_t uncompressed_length;
    // Validate before allocating from an untrusted length header.
    if (!snappy::IsValidCompressedBuffer(compressed, static_cast<size_t>(length)) ||
        !snappy::GetUncompressedLength(compressed, static_cast<size_t>(length), &uncompressed_length)) {
        return snappy_Error(interp, "INVALID_DATA", "Invalid compressed data");
    }
    if (uncompressed_length > static_cast<size_t>(TCL_SIZE_MAX)) {
        return snappy_Error(interp, "SIZE", "Uncompressed data exceeds the Tcl byte array size limit");
    }
    try {
        std::string uncompressed;
        if (!snappy::Uncompress(compressed, static_cast<size_t>(length), &uncompressed)) {
            return snappy_Error(interp, "INVALID_DATA", "Invalid compressed data");
        }
        Tcl_SetObjResult(interp, Tcl_NewByteArrayObj(
            reinterpret_cast<const unsigned char *>(uncompressed.data()), static_cast<Tcl_Size>(uncompressed.size())));
    } catch (const std::bad_alloc &) {
        return snappy_Error(interp, "MEMORY", "Unable to allocate decompression buffer");
    } catch (const std::length_error &) {
        return snappy_Error(interp, "SIZE", "Decompression buffer is too large");
    }
    return TCL_OK;
}

static int snappy_IsValidCompressedCmd(ClientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    CheckArgs(2, 2, 1, "bytes");

    Tcl_Size length;
    unsigned char *bytes = Tcl_GetByteArrayFromObj(objv[1], &length);
    bool flag = snappy::IsValidCompressedBuffer((const char *) bytes, length);
    Tcl_SetObjResult(interp, Tcl_NewBooleanObj(flag));
    return TCL_OK;
}

static int snappy_MaxCompressedLengthCmd(ClientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    CheckArgs(2, 2, 1, "num_uncompressed_bytes");

    Tcl_WideInt num_uncompressed_bytes;
    if (Tcl_GetWideIntFromObj(interp, objv[1], &num_uncompressed_bytes) != TCL_OK) {
        return TCL_ERROR;
    }
    if (num_uncompressed_bytes < 0 ||
        static_cast<uint64_t>(num_uncompressed_bytes) > std::numeric_limits<uint32_t>::max()) {
        return snappy_Error(interp, "SIZE", "num_uncompressed_bytes must be between 0 and 4294967295");
    }
    size_t max_compressed_length = snappy::MaxCompressedLength(static_cast<size_t>(num_uncompressed_bytes));
    if (max_compressed_length == std::numeric_limits<size_t>::max()) {
        return snappy_Error(interp, "SIZE", "Compression buffer is too large");
    }
    Tcl_SetObjResult(interp, Tcl_NewWideIntObj(static_cast<Tcl_WideInt>(max_compressed_length)));
    return TCL_OK;
}


static int snappy_GetUncompressedLengthCmd(ClientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    CheckArgs(2, 2, 1, "bytes");

    Tcl_Size length;
    unsigned char *bytes = Tcl_GetByteArrayFromObj(objv[1], &length);
    size_t uncompressed_length;
    if (!snappy::GetUncompressedLength((const char *) bytes, length, &uncompressed_length)) {
        return snappy_Error(interp, "INVALID_DATA", "Invalid compressed data");
    }
    Tcl_SetObjResult(interp, Tcl_NewWideIntObj(static_cast<Tcl_WideInt>(uncompressed_length)));
    return TCL_OK;
}

#if TCL_MAJOR_VERSION > 8
#define MIN_VERSION "9.0"
#else
#define MIN_VERSION "8.6"
#endif

int Snappy_Init(Tcl_Interp *interp) {
    if (Tcl_InitStubs(interp, MIN_VERSION, 0) == nullptr) {
        return TCL_ERROR;
    }

    Tcl_CreateNamespace(interp, "::snappy", nullptr, nullptr);
    Tcl_CreateObjCommand2(interp, "::snappy::compress", snappy_CompressCmd, nullptr, nullptr);
    Tcl_CreateObjCommand2(interp, "::snappy::uncompress", snappy_UncompressCmd, nullptr, nullptr);
    Tcl_CreateObjCommand2(interp, "::snappy::is_valid_compressed", snappy_IsValidCompressedCmd, nullptr, nullptr);
    Tcl_CreateObjCommand2(interp, "::snappy::max_compressed_length", snappy_MaxCompressedLengthCmd, nullptr, nullptr);
    Tcl_CreateObjCommand2(interp, "::snappy::get_uncompressed_length", snappy_GetUncompressedLengthCmd, nullptr, nullptr);

    return Tcl_PkgProvide(interp, "snappy", XSTR(PROJECT_VERSION));
}

#ifdef USE_NAVISERVER
int Ns_ModuleInit(const char *server, const char *module) {
    Ns_TclRegisterTrace(server, (Ns_TclTraceProc *) Snappy_Init, server, NS_TCL_TRACE_CREATE);
    return NS_OK;
}
#endif
