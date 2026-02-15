#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'EOF'
Generate contributor and pull request lists for release changelogs.

Usage:
  ./scripts/dev_changelog.sh [OPTIONS] TOKEN PRIOR TAG [ADDITIONAL_PR...]

Arguments:
  TOKEN              GitHub token (read-only access is enough)
  PRIOR              Previous release tag/commit
  TAG                New release tag/commit
  ADDITIONAL_PR...   Optional additional PR numbers to include

Options:
  -o, --outfile PATH Write changelog to PATH
  -h, --help         Show this help
EOF
}

require_tool() {
  local tool="$1"
  if ! command -v "${tool}" >/dev/null 2>&1; then
    echo "Required tool not found: ${tool}" >&2
    exit 1
  fi
}

resolve_output_file() {
  local repo_root="$1"
  local tag="$2"
  local outfile_arg="$3"
  if [[ -n "${outfile_arg}" ]]; then
    printf '%s\n' "${outfile_arg}"
    return
  fi
  local normalized="${tag#v}"
  printf '%s/docs/source/changelog/%s-changelog.rst\n' "${repo_root}" "${normalized}"
}

extract_between_markers() {
  local body="$1"
  printf '%s\n' "${body}" | awk '
    /<!--changelog-start-->/ { inside=1; next }
    /<!--changelog-end-->/ { inside=0 }
    inside
  '
}

label_index_for_pr() {
  local label_blob="$1"
  local -a keys=(
    "breaking changes"
    "highlight"
    "pr:deprecation"
    "new feature"
    "enhancement"
    "pr:bugfix"
    "documentation"
    "testing"
    "infrastructure"
    "maintenance"
    "revert"
    "release"
  )

  local i
  for ((i = 0; i < ${#keys[@]}; ++i)); do
    if printf '%s\n' "${label_blob}" | rg -x -F "${keys[$i]}" >/dev/null 2>&1; then
      printf '%s\n' "${i}"
      return
    fi
  done

  # unlabeled bucket
  printf '%s\n' "12"
}

update_citation_file() {
  local repo_root="$1"
  local tag="$2"
  local today_iso="$3"
  local template_file="${repo_root}/scripts/TEMPLATE.cff"
  local citation_file="${repo_root}/CITATION.cff"

  if [[ ! -f "${template_file}" ]]; then
    return
  fi

  sed \
    -e "s/<version>/${tag}/g" \
    -e "s/<date_released>/${today_iso}/g" \
    "${template_file}" > "${citation_file}"
}

main() {
  local outfile=""
  while [[ $# -gt 0 ]]; do
    case "$1" in
      -o|--outfile)
        if [[ $# -lt 2 ]]; then
          echo "--outfile requires a path argument" >&2
          exit 2
        fi
        outfile="$2"
        shift 2
        ;;
      -h|--help)
        usage
        exit 0
        ;;
      *)
        break
        ;;
    esac
  done

  if [[ $# -lt 3 ]]; then
    usage >&2
    exit 2
  fi

  require_tool git
  require_tool curl
  require_tool jq
  require_tool rg

  local token="$1"
  local prior="$2"
  local tag="$3"
  shift 3

  local -a additional_prs=("$@")
  local script_dir repo_root
  script_dir="$(cd "$(dirname "$0")" && pwd)"
  repo_root="$(cd "${script_dir}/.." && pwd)"

  local output_path
  output_path="$(resolve_output_file "${repo_root}" "${tag}" "${outfile}")"
  mkdir -p "$(dirname "${output_path}")"

  local today_iso today_pretty
  today_iso="$(date '+%Y-%m-%d')"
  today_pretty="$(date '+%B %d, %Y')"
  update_citation_file "${repo_root}" "${tag}" "${today_iso}"

  echo "Getting PR numbers:"
  mapfile -t merge_prs < <(
    git -C "${repo_root}" log --oneline --merges "${prior}..${tag}" \
      | sed -nE 's/.*\(#([0-9]+)\).*/\1/p'
  )
  mapfile -t squash_lines < <(
    git -C "${repo_root}" log --oneline --no-merges --first-parent "${prior}..${tag}" \
      | rg -v 'pre-commit autoupdate|New Crowdin updates' || true
  )
  mapfile -t squash_prs < <(
    printf '%s\n' "${squash_lines[@]-}" | sed -nE 's/^.*\(#([0-9]+)\)$/\1/p'
  )

  local -a all_prs=("${merge_prs[@]-}" "${squash_prs[@]-}" "${additional_prs[@]-}")
  mapfile -t pr_numbers < <(
    printf '%s\n' "${all_prs[@]-}" | rg '^[0-9]+$' | sort -n | uniq
  )

  local tmp_dir
  tmp_dir="$(mktemp -d)"
  trap 'rm -rf "${tmp_dir}"' EXIT

  local -a label_titles=(
    "Breaking changes"
    "Highlights"
    "Deprecated classes and functions"
    "New features"
    "Enhancements"
    "Fixed bugs"
    "Documentation-related changes"
    "Changes concerning the testing system"
    "Changes to our development infrastructure"
    "Code quality improvements and similar refactors"
    "Changes that needed to be reverted again"
    "New releases"
    "Unclassified changes"
  )
  local i
  for ((i = 0; i < ${#label_titles[@]}; ++i)); do
    : > "${tmp_dir}/label_${i}.txt"
  done

  local authors_file reviewers_file
  authors_file="${tmp_dir}/authors.txt"
  reviewers_file="${tmp_dir}/reviewers.txt"
  : > "${authors_file}"
  : > "${reviewers_file}"

  local pr_num
  for pr_num in "${pr_numbers[@]-}"; do
    local pr_json
    if ! pr_json="$(
      curl -sfL \
        -H "Accept: application/vnd.github+json" \
        -H "Authorization: Bearer ${token}" \
        "https://api.github.com/repos/ManimCommunity/manim/pulls/${pr_num}"
    )"; then
      echo "Skipping PR ${pr_num}: failed to query metadata" >&2
      continue
    fi

    local title author body label_blob section_index summary
    title="$(printf '%s' "${pr_json}" | jq -r '.title // ""')"
    author="$(printf '%s' "${pr_json}" | jq -r '.user.login // ""')"
    body="$(printf '%s' "${pr_json}" | jq -r '.body // ""')"
    label_blob="$(printf '%s' "${pr_json}" | jq -r '.labels[]?.name // empty')"
    section_index="$(label_index_for_pr "${label_blob}")"

    if [[ -n "${author}" ]]; then
      printf '%s\n' "${author}" >> "${authors_file}"
    fi

    printf '* :pr:`%s`: %s\n' "${pr_num}" "${title}" >> "${tmp_dir}/label_${section_index}.txt"
    summary="$(extract_between_markers "${body}")"
    if [[ -n "${summary}" ]]; then
      while IFS= read -r summary_line; do
        printf '   %s\n' "${summary_line}" >> "${tmp_dir}/label_${section_index}.txt"
      done <<< "${summary}"
    fi
    printf '\n' >> "${tmp_dir}/label_${section_index}.txt"

    curl -sfL \
      -H "Accept: application/vnd.github+json" \
      -H "Authorization: Bearer ${token}" \
      "https://api.github.com/repos/ManimCommunity/manim/pulls/${pr_num}/reviews" \
      | jq -r '.[]?.user.login // empty' >> "${reviewers_file}" || true
  done

  local unique_authors unique_reviewers contributor_count
  unique_authors="$(sort -u "${authors_file}" | rg -v '^$' || true)"
  unique_reviewers="$(sort -u "${reviewers_file}" | rg -v '^$' || true)"
  contributor_count="$(
    {
      printf '%s\n' "${unique_authors}"
      printf '%s\n' "${unique_reviewers}"
    } | rg -v '^$' | sort -u | wc -l | tr -d '[:space:]'
  )"

  {
    printf '%s\n' "$(printf '%*s' "${#tag}" '' | tr ' ' '*')"
    printf '%s\n' "${tag}"
    printf '%s\n\n' "$(printf '%*s' "${#tag}" '' | tr ' ' '*')"
    printf ':Date: %s\n\n' "${today_pretty}"

    printf 'Contributors\n'
    printf '============\n\n'
    printf 'A total of %s people contributed to this release.\n\n' "${contributor_count}"

    if [[ -n "${unique_authors}" ]]; then
      while IFS= read -r author; do
        printf '* %s\n' "${author}"
      done <<< "${unique_authors}"
      printf '\n'
    fi

    printf 'The patches included in this release have been reviewed by the following contributors.\n\n'
    if [[ -n "${unique_reviewers}" ]]; then
      while IFS= read -r reviewer; do
        printf '* %s\n' "${reviewer}"
      done <<< "${unique_reviewers}"
      printf '\n'
    fi

    printf 'Pull requests merged\n'
    printf '====================\n\n'
    printf 'A total of %s pull requests were merged for this release.\n\n' "${#pr_numbers[@]}"

    for ((i = 0; i < ${#label_titles[@]}; ++i)); do
      if [[ ! -s "${tmp_dir}/label_${i}.txt" ]]; then
        continue
      fi
      printf '%s\n' "${label_titles[$i]}"
      printf '%*s\n\n' "${#label_titles[$i]}" '' | tr ' ' '-'
      cat "${tmp_dir}/label_${i}.txt"
    done
  } > "${output_path}"

  printf 'Wrote changelog to: %s\n' "${output_path}"
}

main "$@"
