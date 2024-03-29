/**
 * Copyright Jerily LTD. All Rights Reserved.
 * SPDX-FileCopyrightText: 2023 Neofytos Dimitriou (neo@jerily.cy)
 * SPDX-License-Identifier: MIT.
 */
#include <iostream>
#include <snappy.h>
#include <cstdio>
#include <fstream>
#include "library.h"

#ifndef TCL_SIZE_MAX
typedef int Tcl_Size;
# define Tcl_GetSizeIntFromObj Tcl_GetIntFromObj
# define Tcl_NewSizeIntObj Tcl_NewIntObj
# define TCL_SIZE_MAX      INT_MAX
# define TCL_SIZE_MODIFIER ""
#endif

#define XSTR(s) STR(s)
#define STR(s) #s

#ifdef DEBUG
# define DBG(x) x
#else
# define DBG(x)
#endif

#define CheckArgs(min, max, n, msg) \
                 if ((objc < min) || (objc >max)) { \
                     Tcl_WrongNumArgs(interp, n, objv, msg); \
                     return TCL_ERROR; \
                 }

static int snappy_ModuleInitialized;

static int snappy_CompressCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]) {
    DBG(fprintf(stderr, "CompressCmd\n"));
    CheckArgs(2, 2, 1, "bytes");

    Tcl_Size length;
    unsigned char *bytes = Tcl_GetByteArrayFromObj(objv[1], &length);
    std::string compressed;
    snappy::Compress((const char *) bytes, length, &compressed);
    Tcl_SetObjResult(interp, Tcl_NewByteArrayObj((const unsigned char *) compressed.data(), compressed.length()));
    return TCL_OK;
}

static int snappy_UncompressCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]) {
    DBG(fprintf(stderr, "UncompressCmd\n"));
    CheckArgs(2, 2, 1, "bytes");

    Tcl_Size length;
    unsigned char *bytes = Tcl_GetByteArrayFromObj(objv[1], &length);
    std::string uncompressed;
    snappy::Uncompress((const char *) bytes, length, &uncompressed);
    Tcl_SetObjResult(interp, Tcl_NewByteArrayObj((const unsigned char *) uncompressed.data(), uncompressed.length()));
    return TCL_OK;
}

static int snappy_IsValidCompressedCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]) {
    DBG(fprintf(stderr, "IsValidCompressedCmd\n"));
    CheckArgs(2, 2, 1, "bytes");

    Tcl_Size length;
    unsigned char *bytes = Tcl_GetByteArrayFromObj(objv[1], &length);
    std::string uncompressed;
    bool flag = snappy::IsValidCompressedBuffer((const char *) bytes, length);
    Tcl_SetObjResult(interp, Tcl_NewBooleanObj(flag));
    return TCL_OK;
}

static int snappy_MaxCompressedLengthCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]) {
    DBG(fprintf(stderr, "MaxCompressedLengthCmd\n"));
    CheckArgs(2, 2, 1, "num_uncompressed_bytes");

    long num_uncompressed_bytes;
    if (Tcl_GetLongFromObj(interp, objv[1], &num_uncompressed_bytes) != TCL_OK) {
        Tcl_SetObjResult(interp, Tcl_NewStringObj("num_uncompressed_bytes must be a long integer", -1));
        return TCL_ERROR;
    }
    size_t max_compressed_length = snappy::MaxCompressedLength(num_uncompressed_bytes);
    Tcl_SetObjResult(interp, Tcl_NewLongObj(max_compressed_length));
    return TCL_OK;
}


static int snappy_GetUncompressedLengthCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]) {
    DBG(fprintf(stderr, "GetUncompressedLengthCmd\n"));
    CheckArgs(2, 2, 1, "bytes");

    Tcl_Size length;
    unsigned char *bytes = Tcl_GetByteArrayFromObj(objv[1], &length);
    size_t uncompressed_length;
    if (!snappy::GetUncompressedLength((const char *) bytes, length, &uncompressed_length)) {
        Tcl_SetObjResult(interp, Tcl_NewStringObj("Invalid compressed data", -1));
        return TCL_ERROR;
    }
    Tcl_SetObjResult(interp, Tcl_NewLongObj(uncompressed_length));
    return TCL_OK;
}

static void snappy_ExitHandler(ClientData unused) {
}


void snappy_InitModule() {
    if (!snappy_ModuleInitialized) {
        Tcl_CreateThreadExitHandler(snappy_ExitHandler, nullptr);
        snappy_ModuleInitialized = 1;
    }
}

int Snappy_Init(Tcl_Interp *interp) {
    if (Tcl_InitStubs(interp, "8.6", 0) == nullptr) {
        return TCL_ERROR;
    }

    snappy_InitModule();

    Tcl_CreateNamespace(interp, "::snappy", nullptr, nullptr);
    Tcl_CreateObjCommand(interp, "::snappy::compress", snappy_CompressCmd, nullptr, nullptr);
    Tcl_CreateObjCommand(interp, "::snappy::uncompress", snappy_UncompressCmd, nullptr, nullptr);
    Tcl_CreateObjCommand(interp, "::snappy::is_valid_compressed", snappy_IsValidCompressedCmd, nullptr, nullptr);
    Tcl_CreateObjCommand(interp, "::snappy::max_compressed_length", snappy_MaxCompressedLengthCmd, nullptr, nullptr);
    Tcl_CreateObjCommand(interp, "::snappy::get_uncompressed_length", snappy_GetUncompressedLengthCmd, nullptr, nullptr);

    return Tcl_PkgProvide(interp, "snappy", XSTR(PROJECT_VERSION));
}

#ifdef USE_NAVISERVER
int Ns_ModuleInit(const char *server, const char *module) {
    Ns_TclRegisterTrace(server, (Ns_TclTraceProc *) Snappy_Init, server, NS_TCL_TRACE_CREATE);
    return NS_OK;
}
#endif
