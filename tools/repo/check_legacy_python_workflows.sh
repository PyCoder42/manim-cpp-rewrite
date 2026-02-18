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

removed_workflows=(
  ".github/workflows/ci.yml"
  ".github/workflows/python-publish.yml"
  ".github/workflows/release-publish-documentation.yml"
)

invalid=0

for workflow in "${removed_workflows[@]}"; do
  workflow_path="${repo_root}/${workflow}"
  if [[ -f "${workflow_path}" ]]; then
    echo "Legacy Python workflow should be removed from repository: ${workflow}" >&2
    invalid=1
  fi
done

publish_docker_workflow="${repo_root}/.github/workflows/publish-docker.yml"
if [[ -f "${publish_docker_workflow}" ]]; then
  if rg -n "shell:[[:space:]]*python|setup-python|uv run python|pip install" "${publish_docker_workflow}" >/dev/null 2>&1; then
    echo "Docker workflow must not invoke Python runtime commands." >&2
    invalid=1
  fi
fi

if [[ ${invalid} -ne 0 ]]; then
  exit 1
fi

echo "Legacy Python workflows removed and Docker workflow is Python-free."
