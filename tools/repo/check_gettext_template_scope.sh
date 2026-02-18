#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_gettext_template_scope.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"
gettext_root="${repo_root}/docs/i18n/gettext"

if [[ ! -d "${gettext_root}" ]]; then
  echo "Missing gettext template directory: ${gettext_root}" >&2
  exit 1
fi

required_templates=(
  "book.pot"
  "api.pot"
)

for template in "${required_templates[@]}"; do
  if [[ ! -f "${gettext_root}/${template}" ]]; then
    echo "Missing required gettext template: ${template}" >&2
    exit 1
  fi
done

unexpected_templates="$(
  find "${gettext_root}" -mindepth 1 -type f -name '*.pot' \
    ! -name 'book.pot' ! -name 'api.pot' -print
)"
if [[ -n "${unexpected_templates}" ]]; then
  echo "Unexpected legacy gettext templates remain:" >&2
  echo "${unexpected_templates}" >&2
  exit 1
fi

unexpected_dirs="$(
  find "${gettext_root}" -mindepth 1 -type d -print
)"
if [[ -n "${unexpected_dirs}" ]]; then
  echo "Unexpected legacy gettext subdirectories remain:" >&2
  echo "${unexpected_dirs}" >&2
  exit 1
fi

echo "Gettext template scope is limited to book/api templates."
