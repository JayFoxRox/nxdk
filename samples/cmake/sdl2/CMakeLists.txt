find_package(SDL2 REQUIRED)

add_executable(default main.c)
target_link_libraries(default SDL2::SDL2)
