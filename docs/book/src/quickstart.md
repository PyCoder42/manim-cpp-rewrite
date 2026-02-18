# Quickstart

Create a scene template and render it with the C++ CLI.

## 1. Initialize Project

```sh
manim-cpp init project ./demo_project
cd ./demo_project
```

## 2. Edit Scene

`scenes/main_scene.cpp` already contains `MANIM_REGISTER_SCENE(MainScene)`.
Replace `construct()` with your animation logic.

## 3. Render Scene

```sh
manim-cpp render scenes/main_scene.cpp --scene MainScene --renderer cairo --format mp4
```

## 4. Try OpenGL

```sh
manim-cpp render scenes/main_scene.cpp --scene MainScene --renderer opengl --watch --interactive
```

Media artifacts are emitted according to `manim.cfg` output directory templates.
