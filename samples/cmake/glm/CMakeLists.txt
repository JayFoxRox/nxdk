add_subdirectory(glm)

add_executable(default main.cpp)
target_link_libraries(default glm::glm)

create_xiso("${CMAKE_BINARY_DIR}/glm.iso" "${CMAKE_BINARY_DIR}/")
