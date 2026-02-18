#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_no_duplicate_copy_artifacts.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"
violations=()

while IFS= read -r path; do
  [[ -z "${path}" ]] && continue
  base_name="$(basename "${path}")"
  if [[ "${base_name}" =~ [[:space:]][0-9]+\.[^.]+$ ]]; then
    violations+=("${path}")
    continue
  fi
  shopt -s nocasematch
  if [[ "${base_name}" =~ [[:space:]]copy(\.[^.]+)?$ ]]; then
    violations+=("${path}")
  fi
  shopt -u nocasematch
done < <(
  {
    git -C "${repo_root}" ls-files
    git -C "${repo_root}" ls-files --others --exclude-standard
  } | sort -u
)

if [[ ${#violations[@]} -gt 0 ]]; then
  echo "Duplicate/copy artifact filenames detected; remove accidental duplicates:" >&2
  printf '%s\n' "${violations[@]}" | sed -n '1,40p' >&2
  exit 1
fi

echo "No duplicate/copy artifact filenames detected."
