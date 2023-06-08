cmake_minimum_required(VERSION 3.16)
cmake_policy(VERSION 3.16)

##### lib log configuration #####

##### lib log configuration #####

project(log     LANGUAGES C
                VERSION 0.1.0
                DESCRIPTION "A project of liblog")

add_compile_options(-Wall -Wextra -std=c2x -g)

add_library(log)
target_include_directories(log PUBLIC inc)

add_subdirectory(src)
add_subdirectory(tests)
