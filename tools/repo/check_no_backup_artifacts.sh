#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_no_backup_artifacts.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"
violations=()

while IFS= read -r path; do
  [[ -z "${path}" ]] && continue
  base_name="$(basename "${path}")"
  if [[ "${base_name}" == *~ || "${base_name}" == *.orig || "${base_name}" == *.rej || "${base_name}" == *.bak ]]; then
    violations+=("${path}")
  fi
done < <(
  {
    git -C "${repo_root}" ls-files
    git -C "${repo_root}" ls-files --others --exclude-standard
  } | sort -u
)

if [[ ${#violations[@]} -gt 0 ]]; then
  echo "Backup/conflict artifacts detected; remove these files:" >&2
  printf '%s\n' "${violations[@]}" | sed -n '1,40p' >&2
  exit 1
fi

echo "No backup/conflict artifacts detected."
