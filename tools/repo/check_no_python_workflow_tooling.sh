#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_no_python_workflow_tooling.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"
workflows_dir="${repo_root}/.github/workflows"

if [[ ! -d "${workflows_dir}" ]]; then
  echo "Missing workflows directory: ${workflows_dir}" >&2
  exit 1
fi

setup_matches="$(rg -n --glob '*.yml' "actions/setup-python|python-version:" "${workflows_dir}" || true)"
command_matches="$(rg -n --pcre2 --glob '*.yml' '(^|[;&|])[[:space:]]*(python3?|pip3?)([[:space:]]|$)' "${workflows_dir}" || true)"

if [[ -n "${setup_matches}" ]]; then
  echo "Python setup tooling is forbidden in C++-only workflows:" >&2
  printf '%s\n' "${setup_matches}" >&2
  exit 1
fi

if [[ -n "${command_matches}" ]]; then
  echo "Direct python/pip command usage is forbidden in workflows:" >&2
  printf '%s\n' "${command_matches}" >&2
  exit 1
fi

echo "Workflow tooling check passed (no setup-python/python/pip runtime usage)."
