#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_no_python_script_runtime.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"

shebang_matches="$(rg -n --pcre2 '^#!.*\bpython(3(\.[0-9]+)?)?\b' "${repo_root}/scripts" "${repo_root}/tools" || true)"
command_matches="$(rg -n --pcre2 --glob '*.sh' --glob '*.bash' --glob '*.zsh' --glob '*.ps1' '^[[:space:]]*(python3?|pip3?)([[:space:]]|$)' "${repo_root}/scripts" "${repo_root}/tools" || true)"

if [[ -n "${shebang_matches}" ]]; then
  echo "Python shebang usage is forbidden in scripts/tools:" >&2
  printf '%s\n' "${shebang_matches}" >&2
  exit 1
fi

if [[ -n "${command_matches}" ]]; then
  echo "python/pip command usage is forbidden in scripts/tools:" >&2
  printf '%s\n' "${command_matches}" >&2
  exit 1
fi

echo "Script runtime check passed (no Python shebang or python/pip execution)."
