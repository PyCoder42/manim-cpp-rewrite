#!/usr/bin/env bash
set -euo pipefail

allowlist_path=""
repo_root=""

while [[ $# -gt 0 ]]; do
  case "$1" in
    --allowlist)
      if [[ $# -lt 2 ]]; then
        echo "--allowlist requires a path argument" >&2
        exit 2
      fi
      allowlist_path="$2"
      shift 2
      ;;
    -h|--help)
      echo "Usage: check_python_inventory.sh [--allowlist <file>] [repo_root]"
      exit 0
      ;;
    *)
      if [[ -n "${repo_root}" ]]; then
        echo "Unexpected extra argument: $1" >&2
        exit 2
      fi
      repo_root="$1"
      shift
      ;;
  esac
done

if [[ -z "${repo_root}" ]]; then
  repo_root="$(cd "$(dirname "$0")/../.." && pwd)"
fi
if [[ -z "${allowlist_path}" ]]; then
  allowlist_path="${repo_root}/config/python_allowlist.txt"
fi

if [[ ! -f "${allowlist_path}" ]]; then
  echo "Missing allowlist file: ${allowlist_path}" >&2
  exit 1
fi

python_files=()
while IFS= read -r line; do
  python_files+=("${line}")
done < <(git -C "${repo_root}" ls-files '*.py' '*.pyi')
if [[ ${#python_files[@]} -eq 0 ]]; then
  echo "No tracked Python files found."
  exit 0
fi

allow_patterns=()
while IFS= read -r pattern; do
  case "${pattern}" in
    ''|'#'*) continue ;;
  esac
  allow_patterns+=("${pattern}")
done < "${allowlist_path}"

if [[ ${#allow_patterns[@]} -eq 0 ]]; then
  echo "Allowlist is empty: ${allowlist_path}" >&2
  exit 1
fi

unmatched=()
for path in "${python_files[@]}"; do
  allowed=0
  for pattern in "${allow_patterns[@]}"; do
    if [[ "${path}" == ${pattern} ]]; then
      allowed=1
      break
    fi
  done
  if [[ ${allowed} -eq 0 ]]; then
    unmatched+=("${path}")
  fi
done

if [[ ${#unmatched[@]} -gt 0 ]]; then
  echo "Python files outside allowlist (${allowlist_path}):" >&2
  for path in "${unmatched[@]}"; do
    echo "  ${path}" >&2
  done
  exit 1
fi

echo "Python inventory check passed: ${#python_files[@]} tracked Python file(s), all allowlisted."
