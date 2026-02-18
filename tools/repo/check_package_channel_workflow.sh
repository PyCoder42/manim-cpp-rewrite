#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_package_channel_workflow.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"
workflow_path="${repo_root}/.github/workflows/cxx-package-channels.yml"
manifest_script="${repo_root}/tools/release/generate_package_channel_manifests.sh"

if [[ ! -f "${workflow_path}" ]]; then
  echo "Missing workflow: ${workflow_path}" >&2
  exit 1
fi

if [[ ! -f "${manifest_script}" ]]; then
  echo "Missing package channel manifest generator: ${manifest_script}" >&2
  exit 1
fi

if rg -n "setup-python|uv run python|pip install" "${workflow_path}" >/dev/null 2>&1; then
  echo "Package channel workflow must not depend on Python." >&2
  exit 1
fi

if ! rg -n "workflow_dispatch|release:" "${workflow_path}" >/dev/null 2>&1; then
  echo "Package channel workflow must support release/workflow_dispatch triggers." >&2
  exit 1
fi

if ! rg -n "generate_package_channel_manifests.sh" "${workflow_path}" >/dev/null 2>&1; then
  echo "Package channel workflow is missing manifest generation step." >&2
  exit 1
fi

for channel in homebrew scoop winget chocolatey; do
  if ! rg -n -i "${channel}" "${workflow_path}" >/dev/null 2>&1; then
    echo "Package channel workflow must cover ${channel}." >&2
    exit 1
  fi
done

if ! rg -n "actions/upload-artifact@v5|softprops/action-gh-release@v2" "${workflow_path}" >/dev/null 2>&1; then
  echo "Package channel workflow is missing artifact upload/release publish steps." >&2
  exit 1
fi

echo "Package channel workflow check passed."
