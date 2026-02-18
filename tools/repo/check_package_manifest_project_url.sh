#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_package_manifest_project_url.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"
generator="${repo_root}/tools/release/generate_package_channel_manifests.sh"

if [[ ! -x "${generator}" ]]; then
  echo "Missing or non-executable manifest generator: ${generator}" >&2
  exit 1
fi

tmp_dir="$(mktemp -d)"
trap 'rm -rf "${tmp_dir}"' EXIT

(
  cd "${repo_root}"
  "${generator}" "0.0.0-test" "https://example.com/manim-cpp.tar.gz" "abc123" "${tmp_dir}/manifests" >/dev/null
)

if rg -n "github.com/ManimCommunity/manim" "${tmp_dir}/manifests" >/dev/null 2>&1; then
  echo "Generated manifests must not reference legacy ManimCommunity/manim URLs." >&2
  exit 1
fi

if ! rg -n "github.com/" "${tmp_dir}/manifests" >/dev/null 2>&1; then
  echo "Generated manifests must contain a GitHub project URL." >&2
  exit 1
fi

echo "Package manifests use non-legacy project URLs."
