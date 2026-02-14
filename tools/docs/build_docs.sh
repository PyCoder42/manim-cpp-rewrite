#!/usr/bin/env bash
set -euo pipefail

check_only=0
if [[ "${1:-}" == "--check-only" ]]; then
  check_only=1
  shift
fi

repo_root="${1:-}"
if [[ -z "${repo_root}" ]]; then
  repo_root="$(cd "$(dirname "$0")/../.." && pwd)"
fi

book_root="${repo_root}/docs/book"
book_config="${book_root}/book.toml"
api_doxyfile="${repo_root}/docs/api/Doxyfile"

if [[ ! -f "${book_config}" ]]; then
  echo "Missing mdBook config: ${book_config}" >&2
  exit 1
fi
if [[ ! -f "${api_doxyfile}" ]]; then
  echo "Missing Doxygen config: ${api_doxyfile}" >&2
  exit 1
fi

if [[ "${check_only}" -eq 1 ]]; then
  echo "Docs tooling config verified: mdBook + Doxygen inputs present."
  exit 0
fi

if ! command -v mdbook >/dev/null 2>&1; then
  echo "mdbook executable not found on PATH." >&2
  exit 1
fi
if ! command -v doxygen >/dev/null 2>&1; then
  echo "doxygen executable not found on PATH." >&2
  exit 1
fi

(
  cd "${repo_root}"
  mdbook build docs/book
  doxygen docs/api/Doxyfile
)

echo "Built docs/book and docs/api output."
