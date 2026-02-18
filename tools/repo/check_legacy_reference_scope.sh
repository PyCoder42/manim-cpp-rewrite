#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_legacy_reference_scope.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"
allowlist_path="${repo_root}/config/legacy_reference_allowlist.txt"

if [[ ! -f "${allowlist_path}" ]]; then
  echo "Missing allowlist: ${allowlist_path}" >&2
  exit 1
fi

normalized_allowlist="$(mktemp)"
trap 'rm -f "${normalized_allowlist}"' EXIT

while IFS= read -r line; do
  line="${line%%#*}"
  line="$(echo "${line}" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')"
  if [[ -z "${line}" ]]; then
    continue
  fi
  printf '%s\n' "${line}" >> "${normalized_allowlist}"
done < "${allowlist_path}"

matches="$(git -C "${repo_root}" grep -nE "ManimCommunity/manim|manimcommunity/manim|github.com/manimcommunity/manim|repos/ManimCommunity/manim" -- . || true)"
if [[ -z "${matches}" ]]; then
  echo "No legacy repository references detected."
  exit 0
fi

violations=()
while IFS= read -r entry; do
  [[ -z "${entry}" ]] && continue
  file_path="${entry%%:*}"
  if ! grep -Fxq "${file_path}" "${normalized_allowlist}"; then
    violations+=("${entry}")
  fi
done <<< "${matches}"

if [[ ${#violations[@]} -gt 0 ]]; then
  echo "Legacy repository references found outside allowlist:" >&2
  printf '%s\n' "${violations[@]}" | sed -n '1,40p' >&2
  exit 1
fi

echo "Legacy repository references are scoped to the allowlist."
