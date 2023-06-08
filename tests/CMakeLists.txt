add_executable(tests)

target_link_libraries(tests log)
target_sources(tests PUBLIC main.c)
