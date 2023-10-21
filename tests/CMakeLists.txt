include_directories (${Boost_INCLUDE_DIRS})
link_directories (${Boost_LIBRARY_DIRS})
add_compile_definitions (BOOST_USE_WINAPI_VERSION=0x0600)

add_executable (icenfsd_tests
    settings_tests.cpp
    main.cpp
)

target_link_libraries (icenfsd_tests
    ws2_32
    ${Boost_LIBRARIES}
)

target_precompile_headers (icenfsd_tests
    PRIVATE
    stdafx.h
)