#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_codeql_cpp.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"
workflow_path="${repo_root}/.github/workflows/codeql.yml"

if [[ ! -f "${workflow_path}" ]]; then
  echo "Missing workflow: .github/workflows/codeql.yml" >&2
  exit 1
fi

if rg -n --fixed-strings -- "python" "${workflow_path}" >/dev/null 2>&1; then
  echo "CodeQL workflow must not target Python for the C++ monorepo." >&2
  exit 1
fi

if ! rg -n --fixed-strings -- "cpp" "${workflow_path}" >/dev/null 2>&1; then
  echo "CodeQL workflow must target cpp language analysis." >&2
  exit 1
fi

echo "CodeQL workflow targets C++ analysis."
