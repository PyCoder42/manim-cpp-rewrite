#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'EOF' >&2
Usage: run_original_feature_cross_tests.sh [repo_root] [build_dir]

Executes each feature bucket in config/original_manim_feature_cross_tests.tsv
against CTest to enforce explicit cross-testing of parity-scope features.
EOF
}

if [[ $# -gt 2 ]]; then
  usage
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"
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

feature_count=0
failed_count=0

while IFS= read -r line || [[ -n "${line}" ]]; do
  if [[ -z "${line}" || "${line}" == \#* ]]; then
    continue
  fi

  IFS=$'\t' read -r feature_id legacy_surface required_regex <<<"${line}"
  if [[ -z "${feature_id}" || -z "${required_regex}" ]]; then
    echo "Malformed matrix row: ${line}" >&2
    exit 1
  fi

  feature_count=$((feature_count + 1))
  echo "[cross-test] ${feature_id}: ${legacy_surface}"
  if ! ctest --test-dir "${build_dir}" -R "${required_regex}" --output-on-failure >/dev/null; then
    echo "Feature cross-test failed: ${feature_id}" >&2
    failed_count=$((failed_count + 1))
  fi
done < "${matrix_path}"

if [[ "${feature_count}" -eq 0 ]]; then
  echo "No feature rows found in matrix." >&2
  exit 1
fi

if [[ "${failed_count}" -ne 0 ]]; then
  echo "Feature cross-test run failed for ${failed_count}/${feature_count} features." >&2
  exit 1
fi

echo "Feature cross-test run passed for ${feature_count} parity-scope features."
