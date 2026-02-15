#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
docs_dir="${script_dir}/../docs"

(
  cd "${docs_dir}"
  make html
)

index_path="${docs_dir}/build/html/index.html"

if command -v open >/dev/null 2>&1; then
  if [[ $# -ge 1 ]]; then
    open -a "$1" "${index_path}"
  else
    open "${index_path}"
  fi
elif command -v xdg-open >/dev/null 2>&1; then
  xdg-open "${index_path}" >/dev/null 2>&1 &
else
  echo "Documentation built at ${index_path}"
fi
