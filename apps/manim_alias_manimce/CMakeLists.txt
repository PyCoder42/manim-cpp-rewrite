add_executable(manimce main.cpp)
target_link_libraries(manimce PRIVATE manim_cpp_core)
install(TARGETS manimce)
