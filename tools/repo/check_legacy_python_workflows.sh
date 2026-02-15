#!/usr/bin/env bash
set -euo pipefail

repo_root=""
if [[ $# -gt 1 ]]; then
  echo "Usage: check_legacy_python_workflows.sh [repo_root]" >&2
  exit 2
fi
if [[ $# -eq 1 ]]; then
  repo_root="$1"
else
  repo_root="$(cd "$(dirname "$0")/../.." && pwd)"
fi

legacy_workflows=(
  ".github/workflows/ci.yml"
  ".github/workflows/python-publish.yml"
  ".github/workflows/release-publish-documentation.yml"
)

missing=0
invalid=0

for workflow in "${legacy_workflows[@]}"; do
  workflow_path="${repo_root}/${workflow}"
  if [[ ! -f "${workflow_path}" ]]; then
    echo "Missing workflow: ${workflow}" >&2
    missing=1
    continue
  fi

  on_block="$(
    awk '
      BEGIN { in_on = 0 }
      /^on:[[:space:]]*$/ { in_on = 1; next }
      in_on && /^[^[:space:]#][^:]*:[[:space:]]*$/ { in_on = 0 }
      in_on { print }
    ' "${workflow_path}"
  )"

  if ! grep -Eq '^[[:space:]]*workflow_dispatch:[[:space:]]*$' <<<"${on_block}"; then
    echo "Workflow must expose workflow_dispatch only: ${workflow}" >&2
    invalid=1
  fi

  if grep -Eq '^[[:space:]]*(push|pull_request|release):[[:space:]]*$' <<<"${on_block}"; then
    echo "Workflow still has automatic trigger (push/pull_request/release): ${workflow}" >&2
    invalid=1
  fi
done

if [[ ${missing} -ne 0 || ${invalid} -ne 0 ]]; then
  exit 1
fi

echo "Legacy Python workflows are manual-only (workflow_dispatch)."
