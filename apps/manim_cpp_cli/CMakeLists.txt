add_executable(manim-cpp main.cpp)
target_link_libraries(manim-cpp PRIVATE manim_cpp_core)
install(TARGETS manim-cpp)
