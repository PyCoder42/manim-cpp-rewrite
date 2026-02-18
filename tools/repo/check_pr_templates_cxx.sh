#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_pr_templates_cxx.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"
template_root="${repo_root}/.github"

required_templates=(
  "PULL_REQUEST_TEMPLATE.md"
  "PULL_REQUEST_TEMPLATE/bugfix.md"
  "PULL_REQUEST_TEMPLATE/documentation.md"
  "PULL_REQUEST_TEMPLATE/hackathon.md"
)

for template in "${required_templates[@]}"; do
  template_path="${template_root}/${template}"
  if [[ ! -f "${template_path}" ]]; then
    echo "Missing PR template: ${template}" >&2
    exit 1
  fi
done

forbidden_tokens=(
  "docs.manim.community"
  "ManimCommunity"
  '```python'
)

for template in "${required_templates[@]}"; do
  template_path="${template_root}/${template}"
  for token in "${forbidden_tokens[@]}"; do
    if rg -n --fixed-strings -- "${token}" "${template_path}" >/dev/null 2>&1; then
      echo "PR template contains legacy token (${token}): ${template}" >&2
      exit 1
    fi
  done
done

if ! rg -n --fixed-strings -- "manim-cpp" "${template_root}/PULL_REQUEST_TEMPLATE.md" >/dev/null 2>&1; then
  echo "Top-level PR template must refer to manim-cpp." >&2
  exit 1
fi

echo "PR templates reflect C++ repository wording."
