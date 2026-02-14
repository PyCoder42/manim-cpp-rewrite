#!/usr/bin/env bash
set -euo pipefail

sudo apt-get update
sudo apt-get install -y \
  build-essential \
  cmake \
  pkg-config \
  libcairo2-dev \
  libgl1-mesa-dev \
  libglu1-mesa-dev \
  libglfw3-dev \
  libfreetype6-dev \
  libpango1.0-dev \
  ffmpeg
