cmake_minimum_required(VERSION 3.22.1)
project(snappy-tcl VERSION 1.0.6 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
find_package(TclStub 8.6 REQUIRED)
find_package(Snappy CONFIG REQUIRED HINTS ${SNAPPY_DIR})

message(STATUS "TCL_INCLUDE_PATH: ${TCL_INCLUDE_PATH}")
message(STATUS "TCL_STUB_LIBRARY: ${TCL_STUB_LIBRARY}")

add_library(${PROJECT_NAME} library.cc)
set_target_properties(${PROJECT_NAME}
        PROPERTIES PREFIX "lib"
        POSITION_INDEPENDENT_CODE ON
        INSTALL_RPATH_USE_LINK_PATH ON
)
target_include_directories(${PROJECT_NAME} PRIVATE ${TCL_INCLUDE_PATH})
target_compile_definitions(${PROJECT_NAME} PRIVATE TCL_THREADS PROJECT_VERSION=${PROJECT_VERSION})
target_link_libraries(${PROJECT_NAME} PRIVATE Snappy::snappy)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_TCL_STUBS)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${TCL_STUB_LIBRARY})
else()
    # The embedding application links Tcl and registers Snappy_Init.
    target_compile_definitions(${PROJECT_NAME} PRIVATE STATIC_BUILD)
endif()

install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib/${PROJECT_NAME}${PROJECT_VERSION}
        ARCHIVE DESTINATION lib/${PROJECT_NAME}${PROJECT_VERSION}
        RUNTIME DESTINATION lib/${PROJECT_NAME}${PROJECT_VERSION}
)

if(BUILD_SHARED_LIBS)
    configure_file(pkgIndex.tcl.in pkgIndex.tcl @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pkgIndex.tcl
            DESTINATION lib/${PROJECT_NAME}${PROJECT_VERSION})
endif()

install(DIRECTORY examples/ DESTINATION share/${PROJECT_NAME}/examples
        FILES_MATCHING PATTERN "*.tcl")

include(CTest)
if(BUILD_TESTING AND BUILD_SHARED_LIBS)
    if(NOT TCL_TCLSH)
        message(FATAL_ERROR "Tcl shell required for tests; set TCL_TCLSH or BUILD_TESTING=OFF")
    endif()
    add_test(NAME AllUnitTests COMMAND ${TCL_TCLSH}
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/all.tcl $<TARGET_FILE:${PROJECT_NAME}>)
endif()
