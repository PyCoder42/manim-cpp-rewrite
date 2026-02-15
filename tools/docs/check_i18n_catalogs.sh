#!/usr/bin/env bash
set -euo pipefail

repo_root="${1:-}"
if [[ -z "${repo_root}" ]]; then
  repo_root="$(cd "$(dirname "$0")/../.." && pwd)"
fi

i18n_root="${repo_root}/docs/i18n"
gettext_root="${i18n_root}/gettext"
required_locales=(fr hi pt sv)

if [[ ! -d "${i18n_root}" ]]; then
  echo "Missing docs/i18n directory: ${i18n_root}" >&2
  exit 1
fi

if [[ ! -d "${gettext_root}" ]]; then
  echo "Missing gettext templates directory: ${gettext_root}" >&2
  exit 1
fi

missing=0
for locale in "${required_locales[@]}"; do
  locale_root="${i18n_root}/${locale}/LC_MESSAGES"
  if [[ ! -d "${locale_root}" ]]; then
    echo "Missing locale catalog directory: ${locale_root}" >&2
    missing=1
    continue
  fi

  if ! find "${locale_root}" -type f -name '*.po' -print -quit | grep -q .; then
    echo "No .po files found under: ${locale_root}" >&2
    missing=1
  fi
done

if ! find "${gettext_root}" -type f -name '*.pot' -print -quit | grep -q .; then
  echo "No gettext template (.pot) files found under: ${gettext_root}" >&2
  missing=1
fi

if [[ "${missing}" -ne 0 ]]; then
  exit 1
fi

echo "i18n catalogs verified for locales: ${required_locales[*]}"
