#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_legacy_docs_purged.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"

required_paths=(
  "docs/book/book.toml"
  "docs/book/src/SUMMARY.md"
  "docs/api/Doxyfile"
  "docs/i18n/gettext/book.pot"
  "docs/i18n/gettext/api.pot"
)

for required_path in "${required_paths[@]}"; do
  if [[ ! -e "${repo_root}/${required_path}" ]]; then
    echo "Missing required docs asset: ${required_path}" >&2
    exit 1
  fi
done

legacy_paths=(
  "docs/source"
  "docs/html"
  "docs/requirements.txt"
  "docs/rtd-requirements.txt"
  "docs/skip-manim"
)

for legacy_path in "${legacy_paths[@]}"; do
  if [[ -e "${repo_root}/${legacy_path}" ]]; then
    echo "Legacy docs artifact should be removed: ${legacy_path}" >&2
    exit 1
  fi
done

echo "Legacy docs artifacts are purged; mdBook + Doxygen docs layout is clean."
