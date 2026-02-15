#!/usr/bin/env bash
set -euo pipefail

usage() {
  echo "Usage: package_binaries.sh <build_dir> <output_dir>" >&2
}

if [[ $# -ne 2 ]]; then
  usage
  exit 2
fi

build_dir="$1"
output_dir="$2"
repo_root="$(cd "$(dirname "$0")/../.." && pwd)"

if [[ ! -d "${build_dir}" ]]; then
  echo "Build directory not found: ${build_dir}" >&2
  exit 1
fi

mkdir -p "${output_dir}"
staging_dir="$(mktemp -d)"
trap 'rm -rf "${staging_dir}"' EXIT

platform="$(uname -s | tr '[:upper:]' '[:lower:]')"
arch="$(uname -m | tr '[:upper:]' '[:lower:]')"
archive_name="manim-cpp-${platform}-${arch}.tar.gz"

mkdir -p "${staging_dir}/bin"
mkdir -p "${staging_dir}/config"
mkdir -p "${staging_dir}/share/shaders"

copy_binary() {
  local binary_path="$1"
  local binary_name="$2"
  if [[ -f "${binary_path}" ]]; then
    cp "${binary_path}" "${staging_dir}/bin/${binary_name}"
    chmod +x "${staging_dir}/bin/${binary_name}"
  else
    echo "Missing required binary: ${binary_path}" >&2
    exit 1
  fi
}

copy_binary "${build_dir}/apps/manim_cpp_cli/manim-cpp" "manim-cpp"
copy_binary "${build_dir}/apps/manim_alias_manim/manim" "manim"
copy_binary "${build_dir}/apps/manim_alias_manimce/manimce" "manimce"
copy_binary "${build_dir}/apps/manim_cpp_migrate/manim-cpp-migrate" "manim-cpp-migrate"
copy_binary "${build_dir}/apps/manim_cpp_extract_frames/manim-cpp-extract-frames" "manim-cpp-extract-frames"

cp "${repo_root}/config/manim.cfg.default" "${staging_dir}/config/manim.cfg.default"
cp -R "${repo_root}/src/manim_cpp/renderer/shaders/." "${staging_dir}/share/shaders/"

tar -C "${staging_dir}" -czf "${output_dir}/${archive_name}" .
echo "Created release archive: ${output_dir}/${archive_name}"
