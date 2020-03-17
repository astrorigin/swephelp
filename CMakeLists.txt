# swephelp CMakeLists.txt

cmake_minimum_required( VERSION 3.10 )
project( swephelp )

message( STATUS "-- Configuring swephelp..." )

set( SOURCES
    swhdatetime.c
    swhformat.c
    swhgeo.c
    swhmisc.c
    swhraman.c
    swhsearch.c
    swhutil.c )

set( HEADERS
    swephelp.h
    swhdatetime.h
    swhdef.h
    swhformat.h
    swhgeo.h
    swhmisc.h
    swhraman.h
    swhsearch.h
    swhutil.h
    swhwin.h )

include_directories( BEFORE .
    /usr/include
    /usr/include/swisseph
    /usr/local/include
    /usr/local/include/swisseph )

if ( MSVC )
    add_definitions( -D_CRT_SECURE_NO_WARNINGS )
else()
    add_definitions( -g -O9 -Wall )
    if ( NOT MINGW )
        add_definitions( -fPIC )
    endif()
endif()

add_library( swephelp STATIC ${SOURCES} )

install( TARGETS swephelp ARCHIVE DESTINATION lib )
install( FILES ${HEADERS} DESTINATION include/swephelp )

# vi: fenc=utf-8 ff=unix et ai sw=4 ts=3 sts=4
