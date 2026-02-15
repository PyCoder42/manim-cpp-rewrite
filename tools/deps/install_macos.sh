#!/usr/bin/env bash
set -euo pipefail

brew update
brew install \
  cmake \
  cairo \
  ffmpeg \
  glfw \
  freetype \
  pango \
  doxygen \
  mdbook
