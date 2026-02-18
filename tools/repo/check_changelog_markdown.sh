#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_changelog_markdown.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"
script_path="${repo_root}/scripts/dev_changelog.sh"

if [[ ! -f "${script_path}" ]]; then
  echo "Missing changelog script: ${script_path}" >&2
  exit 1
fi

required_patterns=(
  "docs/book/src/changelog"
  "%s-changelog.md"
  "## Pull Requests Merged"
  "https://github.com/%s/pull/%s"
)

for pattern in "${required_patterns[@]}"; do
  if ! rg -n "${pattern}" "${script_path}" >/dev/null 2>&1; then
    echo "Changelog script missing required Markdown migration contract: ${pattern}" >&2
    exit 1
  fi
done

forbidden_patterns=(
  "docs/source/changelog"
  ':pr:`'
)

for pattern in "${forbidden_patterns[@]}"; do
  if rg -n --fixed-strings -- "${pattern}" "${script_path}" >/dev/null 2>&1; then
    echo "Changelog script contains legacy RST contract: ${pattern}" >&2
    exit 1
  fi
done

echo "Changelog markdown contract check passed."
