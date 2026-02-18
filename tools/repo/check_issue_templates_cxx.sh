#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_issue_templates_cxx.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"
template_root="${repo_root}/.github/ISSUE_TEMPLATE"

required_templates=(
  "bug_report.md"
  "installation_issue.md"
  "feature_request.md"
)

for template in "${required_templates[@]}"; do
  template_path="${template_root}/${template}"
  if [[ ! -f "${template_path}" ]]; then
    echo "Missing issue template: ${template}" >&2
    exit 1
  fi
done

forbidden_tokens=(
  "Python version"
  "pip list"
  '```py'
  "python/py/python3 --version"
)

for template in "${required_templates[@]}"; do
  template_path="${template_root}/${template}"
  for token in "${forbidden_tokens[@]}"; do
    if rg -n --fixed-strings -- "${token}" "${template_path}" >/dev/null 2>&1; then
      echo "Issue template contains legacy Python-era token (${token}): ${template}" >&2
      exit 1
    fi
  done
done

if ! rg -n --fixed-strings -- "Compiler (name and version)" "${template_root}/bug_report.md" >/dev/null 2>&1; then
  echo "Bug template is missing C++ compiler diagnostics section." >&2
  exit 1
fi

if ! rg -n --fixed-strings -- "CMake version" "${template_root}/installation_issue.md" >/dev/null 2>&1; then
  echo "Installation template is missing CMake diagnostics section." >&2
  exit 1
fi

echo "Issue templates reflect C++ diagnostics expectations."
