# swephelp CMakeLists.txt

cmake_minimum_required( VERSION 3.10 )

project( swephelp )

message( STATUS "-- Configuring swephelp..." )

# Configure swisseph
if ( NOT LIBSWE_INCLUDE_PATH )
    if ( WIN32 )
        set( LIBSWE_INCLUDE_PATH
            "C:\\swisseph"
            CACHE STRING "Path to swisseph header files" )
    else()
        set( LIBSWE_INCLUDE_PATH
            "/usr/local/include/swisseph"
            CACHE STRING "Path to swisseph header files" )
    endif()
endif()

# Find sqlite3
if ( NOT MSVC )
    find_package( PkgConfig )
    pkg_check_modules( SQLITE3 sqlite3 )
    if ( NOT SQLITE3_FOUND )
        message( STATUS "... Using internal sqlite3" )
        add_subdirectory( sqlite3 )
        include_directories( BEFORE sqlite3 )
    else()
        include_directories( BEFORE ${SQLITE3_INCLUDE_DIRS} )
    endif()
else()
    add_subdirectory( sqlite3 )
    include_directories( BEFORE sqlite3 )
endif()

# Options
option( SWH_DB_TRACE
    "Print debug info (db)"
    OFF )

if ( SWH_DB_TRACE )
    add_definitions( -DSWH_DB_TRACE )
endif()

set( SOURCES
    swhaspect.c
    swhatlas.c
    swhdatetime.c
    swhdb.c
    swhdbxx.cpp
    swhformat.c
    swhgeo.c
    swhmisc.c
    swhraman.c
    swhsearch.c
    swhtimezone.c
    swhxx.cpp )

set( HEADERS
    swephelp.h
    swhaspect.h
    swhatlas.h
    swhdatetime.h
    swhdb.h
    swhdbxx.h
    swhdbxx.hpp
    swhdef.h
    swhformat.h
    swhgeo.h
    swhmisc.h
    swhraman.h
    swhsearch.h
    swhtimezone.h
    swhwin.h
    swhxx.h
    swhxx.hpp )

include_directories( BEFORE . ${LIBSWE_INCLUDE_PATH} )

if ( MSVC )
    add_definitions( -D_CRT_SECURE_NO_WARNINGS )
else()
    add_definitions( -g -O3 -Wall )
    if ( NOT MINGW )
        add_definitions( -fPIC )
    endif()
endif()

add_library( swephelp STATIC ${SOURCES} )

install( TARGETS swephelp ARCHIVE DESTINATION lib )
install( FILES ${HEADERS} DESTINATION include/swephelp )

# vi: sw=4 ts=4 et
