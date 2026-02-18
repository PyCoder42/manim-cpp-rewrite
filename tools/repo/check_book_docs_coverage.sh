#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_book_docs_coverage.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"
book_root="${repo_root}/docs/book/src"
summary_path="${book_root}/SUMMARY.md"

if [[ ! -d "${book_root}" ]]; then
  echo "Missing docs/book source directory: ${book_root}" >&2
  exit 1
fi

if [[ ! -f "${summary_path}" ]]; then
  echo "Missing mdBook summary file: ${summary_path}" >&2
  exit 1
fi

required_chapters=(
  "index.md"
  "architecture.md"
  "installation.md"
  "quickstart.md"
  "cli.md"
  "plugins.md"
  "migration.md"
  "testing.md"
  "i18n.md"
)

for chapter in "${required_chapters[@]}"; do
  chapter_path="${book_root}/${chapter}"
  if [[ ! -f "${chapter_path}" ]]; then
    echo "Missing required mdBook chapter: ${chapter}" >&2
    exit 1
  fi
  if ! rg -n --fixed-strings -- "(${chapter})" "${summary_path}" >/dev/null 2>&1; then
    echo "SUMMARY.md is missing chapter link: ${chapter}" >&2
    exit 1
  fi
done

min_line_count=12
for chapter in "${required_chapters[@]}"; do
  chapter_path="${book_root}/${chapter}"
  line_count="$(wc -l < "${chapter_path}")"
  if [[ "${line_count}" -lt "${min_line_count}" ]]; then
    echo "Chapter is too small and still placeholder-like: ${chapter} (${line_count} lines)" >&2
    exit 1
  fi
done

echo "mdBook coverage check passed."
