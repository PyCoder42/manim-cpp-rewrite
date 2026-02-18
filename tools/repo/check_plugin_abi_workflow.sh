#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_plugin_abi_workflow.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"
workflow_path="${repo_root}/.github/workflows/cxx-plugin-abi-matrix.yml"

if [[ ! -f "${workflow_path}" ]]; then
  echo "Missing workflow: ${workflow_path}" >&2
  exit 1
fi

if ! rg -n "os:\\s*\\[ubuntu-latest, macos-latest, windows-latest\\]" "${workflow_path}" \
  >/dev/null 2>&1; then
  echo "Plugin ABI workflow must run on linux, macOS, and windows matrix targets." >&2
  exit 1
fi

if ! rg -n "ctest .* -R " "${workflow_path}" >/dev/null 2>&1; then
  echo "Plugin ABI workflow must run ctest with plugin-specific selection." >&2
  exit 1
fi

if ! grep -F -n 'Plugin|Cli\\.PluginsLoad' "${workflow_path}" >/dev/null 2>&1; then
  echo "Plugin ABI workflow must run plugin-specific ctest selection." >&2
  exit 1
fi

if rg -n "setup-python|uv run python|pip install" "${workflow_path}" >/dev/null 2>&1; then
  echo "Plugin ABI workflow must not depend on Python steps." >&2
  exit 1
fi

echo "Plugin ABI workflow check passed."
