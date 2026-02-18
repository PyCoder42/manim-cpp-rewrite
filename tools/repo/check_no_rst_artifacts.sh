#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_no_rst_artifacts.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"

tracked_rst="$(git -C "${repo_root}" ls-files '*.rst')"
if [[ -n "${tracked_rst}" ]]; then
  echo "Tracked .rst artifacts are not allowed in mdBook-only docs workflow." >&2
  echo "${tracked_rst}" | sed -n '1,20p' >&2
  exit 1
fi

if [[ ! -f "${repo_root}/scripts/template_docsting_with_example.md" ]]; then
  echo "Missing Markdown doc template: scripts/template_docsting_with_example.md" >&2
  exit 1
fi

echo "No tracked .rst artifacts detected."
