#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_cxx_release_workflow.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"
workflow_path="${repo_root}/.github/workflows/cxx-release.yml"

if [[ ! -f "${workflow_path}" ]]; then
  echo "Missing workflow: ${workflow_path}" >&2
  exit 1
fi

if rg -n "setup-python|uv run python|pip install" "${workflow_path}" >/dev/null 2>&1; then
  echo "C++ release workflow must not depend on Python publishing steps." >&2
  exit 1
fi

if ! rg -n "cmake -S|cmake --build|ctest --test-dir" "${workflow_path}" >/dev/null 2>&1; then
  echo "C++ release workflow is missing configure/build/test steps." >&2
  exit 1
fi

if ! rg -n "actions/upload-artifact@v5|softprops/action-gh-release@v2" "${workflow_path}" >/dev/null 2>&1; then
  echo "C++ release workflow is missing artifact upload/release publishing steps." >&2
  exit 1
fi

echo "C++ release workflow check passed."
