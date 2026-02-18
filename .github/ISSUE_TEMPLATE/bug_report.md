---
name: manim-cpp bug
about: Report a bug or unexpected behavior when running manim-cpp
title: ""
labels: bug
assignees: ''

---

## Description of bug / unexpected behavior
<!-- Add a clear and concise description of the problem you encountered. -->


## Expected behavior
<!-- Add a clear and concise description of what you expected to happen. -->


## How to reproduce the issue
<!-- Provide a C++ scene and CLI command illustrating the undesired behavior. -->

<details><summary>Code for reproducing the problem</summary>

```cpp
Paste your code here.
```

</details>

<details><summary>Build and render commands</summary>

```text
cmake -S . -B build/repro -DCMAKE_BUILD_TYPE=Debug
cmake --build build/repro -j
./build/repro/apps/manim_cpp_cli/manim-cpp render <scene-file>.cpp <SceneName> -v DEBUG
```

</details>


## Additional media files
<!-- Paste in the files manim-cpp produced while rendering the code above. -->

<details><summary>Images/GIFs</summary>

<!-- PASTE MEDIA HERE -->

</details>


## Logs
<details><summary>Terminal output</summary>
<!-- Add "-v DEBUG" when calling manim-cpp to generate more detailed logs -->

```
PASTE HERE OR PROVIDE LINK TO https://pastebin.com/ OR SIMILAR
```

<!-- Insert screenshots here (only when absolutely necessary, we prefer copy/pasted output!) -->

</details>


## System specifications

<details><summary>System Details</summary>

- OS (with version, e.g., Windows 10 v2004 or macOS 10.15 (Catalina)):
- RAM:
- Compiler (name and version):
- CMake version:
- Renderer backend (`cairo` or `opengl`):
- FFmpeg version:
- GPU and driver version (if using OpenGL):
```
PASTE HERE
```
</details>

<details><summary>LaTeX details</summary>

+ LaTeX distribution (e.g. TeX Live 2020):
+ Installed LaTeX packages:
<!-- output of `tlmgr list --only-installed` for TeX Live or a screenshot of the Packages page for MikTeX -->
</details>

## Additional comments
<!-- Add further context that you think might be relevant for this issue here. -->
