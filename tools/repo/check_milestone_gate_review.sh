#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_milestone_gate_review.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"

templates=(
  ".github/PULL_REQUEST_TEMPLATE.md"
  ".github/PULL_REQUEST_TEMPLATE/bugfix.md"
  ".github/PULL_REQUEST_TEMPLATE/documentation.md"
  ".github/PULL_REQUEST_TEMPLATE/hackathon.md"
)

required_checkbox="- [ ] Milestone gate review completed for the current plan tranche."
for template in "${templates[@]}"; do
  template_path="${repo_root}/${template}"
  if [[ ! -f "${template_path}" ]]; then
    echo "Missing PR template: ${template_path}" >&2
    exit 1
  fi
  if ! rg -n --fixed-strings -- "${required_checkbox}" "${template_path}" >/dev/null 2>&1; then
    echo "Missing milestone gate checkbox in template: ${template_path}" >&2
    exit 1
  fi
done

workflow_path="${repo_root}/.github/workflows/milestone-gate-review.yml"
if [[ ! -f "${workflow_path}" ]]; then
  echo "Missing workflow: ${workflow_path}" >&2
  exit 1
fi

if ! rg -n "pull_request" "${workflow_path}" >/dev/null 2>&1; then
  echo "Milestone gate workflow must trigger on pull_request." >&2
  exit 1
fi

if ! rg -n --fixed-strings -- "- [x] Milestone gate review completed for the current plan tranche." "${workflow_path}" >/dev/null 2>&1; then
  echo "Milestone gate workflow must enforce checked review checkbox in PR body." >&2
  exit 1
fi

echo "Milestone gate review check passed."
