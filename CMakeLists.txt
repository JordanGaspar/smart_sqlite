cmake_minimum_required(VERSION 3.2)

project(smart_sqlite LANGUAGES CXX VERSION 1.00)

add_library(smart_sqlite SHARED smart_sqlite.cpp smart_sqlite.hpp)

set_target_properties(smart_sqlite PROPERTIES PUBLIC_HEADER smart_sqlite.hpp CXX_STANDARD 23)

target_include_directories(smart_sqlite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(SQLite3 REQUIRED)
target_link_libraries(smart_sqlite PUBLIC SQLite::SQLite3)

add_executable(main main.cpp)
set_target_properties(main PROPERTIES CXX_STANDARD 23)
target_link_libraries(main PRIVATE smart_sqlite)

include(GNUInstallDirs)
install(TARGETS smart_sqlite
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)

set(CPACK_GENERATOR "DEB;RPM")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Jordan Gaspar <contato@jordangaspar.com>")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsqlite3-dev (>= 3.40)")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++ Wrapper for SQLite3 using Smart Pointers.")
include(CPack)
