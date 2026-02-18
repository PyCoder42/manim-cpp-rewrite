#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_book_metadata_cxx.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"
book_toml="${repo_root}/docs/book/book.toml"

if [[ ! -f "${book_toml}" ]]; then
  echo "Missing mdBook metadata file: ${book_toml}" >&2
  exit 1
fi

if rg -n --fixed-strings -- "Manim Community" "${book_toml}" >/dev/null 2>&1; then
  echo "book.toml contains legacy author metadata." >&2
  exit 1
fi

if ! rg -n --fixed-strings -- 'authors = ["manim-cpp contributors"]' "${book_toml}" >/dev/null 2>&1; then
  echo "book.toml must declare manim-cpp contributors as authors." >&2
  exit 1
fi

echo "mdBook metadata uses C++ monorepo author contract."
