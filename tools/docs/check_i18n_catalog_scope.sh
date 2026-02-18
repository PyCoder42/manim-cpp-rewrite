#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_i18n_catalog_scope.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"
i18n_root="${repo_root}/docs/i18n"
required_locales=(fr hi pt sv)
required_catalogs=(book.po api.po)

missing=0

for locale in "${required_locales[@]}"; do
  locale_root="${i18n_root}/${locale}/LC_MESSAGES"
  if [[ ! -d "${locale_root}" ]]; then
    echo "Missing locale catalog directory: ${locale_root}" >&2
    missing=1
    continue
  fi

  for catalog in "${required_catalogs[@]}"; do
    catalog_path="${locale_root}/${catalog}"
    if [[ ! -f "${catalog_path}" ]]; then
      echo "Missing required locale catalog: ${catalog_path}" >&2
      missing=1
    fi
  done

  while IFS= read -r catalog_path; do
    catalog_name="$(basename "${catalog_path}")"
    if [[ "${catalog_name}" != "book.po" && "${catalog_name}" != "api.po" ]]; then
      echo "Unexpected legacy locale catalog outside scope: ${catalog_path}" >&2
      missing=1
    fi
  done < <(find "${locale_root}" -type f -name '*.po' | LC_ALL=C sort)
done

if [[ "${missing}" -ne 0 ]]; then
  exit 1
fi

echo "Locale catalogs are scoped to book/api templates for locales: ${required_locales[*]}"
