#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_legacy_artifacts_purged.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"

forbidden_paths=(
  "pyproject.toml"
  "uv.lock"
  "mypy.ini"
  "Plan- Manim Conversion to C++.md"
)

for path in "${forbidden_paths[@]}"; do
  if [[ -e "${repo_root}/${path}" ]]; then
    echo "Legacy artifact must be removed from final C++ cutover: ${path}" >&2
    exit 1
  fi
done

tracked_legacy_tree="$(git -C "${repo_root}" ls-files 'manim/**')"
if [[ -n "${tracked_legacy_tree}" ]]; then
  echo "Tracked legacy tree must be removed from final C++ cutover: manim/" >&2
  echo "${tracked_legacy_tree}" | sed -n '1,10p' >&2
  exit 1
fi

docker_readme="${repo_root}/docker/readme.md"
if [[ -f "${docker_readme}" ]]; then
  if rg -n "ManimCommunity/manim|manimcommunity/manim" "${docker_readme}" >/dev/null 2>&1; then
    echo "Docker readme must not reference legacy repository/image names." >&2
    exit 1
  fi
fi

changelog_script="${repo_root}/scripts/dev_changelog.sh"
if [[ ! -f "${changelog_script}" ]]; then
  echo "Missing changelog script: ${changelog_script}" >&2
  exit 1
fi

if rg -n "ManimCommunity/manim" "${changelog_script}" >/dev/null 2>&1; then
  echo "Changelog script must not hardcode legacy repository slug." >&2
  exit 1
fi

if ! rg -n "repos/\\$\\{repo_slug\\}" "${changelog_script}" >/dev/null 2>&1; then
  echo "Changelog script must resolve GitHub API repository slug dynamically." >&2
  exit 1
fi

doc_template="${repo_root}/scripts/template_docsting_with_example.rst"
if [[ -f "${doc_template}" ]] && rg -n "ManimCommunity/manim" "${doc_template}" >/dev/null 2>&1; then
  echo "Docstring template must not reference legacy ManimCommunity URL." >&2
  exit 1
fi

echo "Legacy artifact purge check passed."
