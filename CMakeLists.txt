cmake_minimum_required(VERSION 3.7)
PROJECT(pdlib)

SET(PDLIB_VERSION 0.1.0)

file(GLOB_RECURSE SRC_LIST FOLLOW_SYMLINKS src/*.cc)
set(SOURCE_FILES pdlib.cc php_pdlib.h)

add_executable(pdlib ${SRC_LIST} ${SOURCE_FILES})

# Define path to php-src, change accordingly
set(PHP_SOURCE ../../)
# Import php sources
include_directories(
        ${PHP_SOURCE}
        ${PHP_SOURCE}/main
        ${PHP_SOURCE}/Zend
        ${PHP_SOURCE}/TSRM
        ${PHP_SOURCE}/ext
        ${PHP_SOURCE}/sapi
        ${PHP_SOURCE}/pear
)