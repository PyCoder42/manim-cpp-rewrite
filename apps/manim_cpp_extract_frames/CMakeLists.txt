add_executable(manim-cpp-extract-frames main.cpp)
target_compile_features(manim-cpp-extract-frames PRIVATE cxx_std_23)
install(TARGETS manim-cpp-extract-frames)
