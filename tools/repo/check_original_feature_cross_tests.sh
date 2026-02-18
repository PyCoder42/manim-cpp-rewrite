#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 1 || $# -gt 2 ]]; then
  echo "Usage: check_original_feature_cross_tests.sh <repo_root> [build_dir]" >&2
  exit 2
fi

repo_root="$1"
build_dir="${2:-${repo_root}/build/debug}"
matrix_path="${repo_root}/config/original_manim_feature_cross_tests.tsv"

if [[ ! -f "${matrix_path}" ]]; then
  echo "Missing feature cross-test matrix: ${matrix_path}" >&2
  exit 1
fi

if [[ ! -d "${build_dir}" ]]; then
  echo "Missing build directory: ${build_dir}" >&2
  exit 1
fi

ctest_listing="$(ctest --test-dir "${build_dir}" -N 2>/dev/null)"
if [[ -z "${ctest_listing}" ]]; then
  echo "Unable to read CTest test listing from: ${build_dir}" >&2
  exit 1
fi

line_number=0
feature_count=0
missing_count=0
feature_id_tmp="$(mktemp)"
regex_tmp="$(mktemp)"
trap 'rm -f "${feature_id_tmp}" "${regex_tmp}"' EXIT

while IFS= read -r line || [[ -n "${line}" ]]; do
  line_number=$((line_number + 1))

  if [[ -z "${line}" || "${line}" == \#* ]]; then
    continue
  fi

  IFS=$'\t' read -r feature_id legacy_surface required_regex <<<"${line}"
  if [[ -z "${feature_id}" || -z "${legacy_surface}" || -z "${required_regex}" ]]; then
    echo "Malformed matrix row at ${matrix_path}:${line_number}" >&2
    exit 1
  fi

  feature_count=$((feature_count + 1))
  printf '%s\n' "${feature_id}" >> "${feature_id_tmp}"
  printf '%s\n' "${required_regex}" >> "${regex_tmp}"

  if ! printf '%s\n' "${ctest_listing}" | rg -n --pcre2 "${required_regex}" >/dev/null 2>&1; then
    echo "Missing cross-test coverage for feature '${feature_id}' (${legacy_surface})." >&2
    echo "Expected test regex: ${required_regex}" >&2
    missing_count=$((missing_count + 1))
  fi
done < "${matrix_path}"

duplicate_feature_ids="$(sort "${feature_id_tmp}" | uniq -d || true)"
if [[ -n "${duplicate_feature_ids}" ]]; then
  echo "Feature matrix contains duplicate feature_id values:" >&2
  printf '%s\n' "${duplicate_feature_ids}" >&2
  exit 1
fi

duplicate_regexes="$(sort "${regex_tmp}" | uniq -d || true)"
if [[ -n "${duplicate_regexes}" ]]; then
  echo "Feature matrix contains duplicate required_test_regex values:" >&2
  printf '%s\n' "${duplicate_regexes}" >&2
  exit 1
fi

if [[ "${feature_count}" -lt 60 ]]; then
  echo "Feature matrix is unexpectedly small (${feature_count} rows)." >&2
  exit 1
fi

if [[ "${missing_count}" -ne 0 ]]; then
  echo "Feature cross-test matrix has ${missing_count} uncovered feature(s)." >&2
  exit 1
fi

echo "Original feature cross-test matrix verified: ${feature_count} features mapped to CTest coverage."
