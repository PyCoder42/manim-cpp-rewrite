#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_docker_tag_contract.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"
workflow_path="${repo_root}/.github/workflows/publish-docker.yml"

if [[ ! -f "${workflow_path}" ]]; then
  echo "Missing workflow: ${workflow_path}" >&2
  exit 1
fi

if rg -n "manimcommunity/" "${workflow_path}" >/dev/null 2>&1; then
  echo "Docker publish workflow must not hardcode the manimcommunity namespace." >&2
  exit 1
fi

if ! rg -n "IMAGE_NAMESPACE:" "${workflow_path}" >/dev/null 2>&1; then
  echo "Docker publish workflow must define IMAGE_NAMESPACE." >&2
  exit 1
fi

if ! rg -n "vars\\.DOCKER_IMAGE_NAMESPACE" "${workflow_path}" >/dev/null 2>&1; then
  echo "Docker publish workflow must allow IMAGE_NAMESPACE override via vars.DOCKER_IMAGE_NAMESPACE." >&2
  exit 1
fi

if ! rg -n "'manimcommunity'" "${workflow_path}" >/dev/null 2>&1; then
  echo "Docker publish workflow must include a default IMAGE_NAMESPACE fallback." >&2
  exit 1
fi

required_tags=(
  '${{ env.IMAGE_NAMESPACE }}/manim-cpp:latest'
  '${{ env.IMAGE_NAMESPACE }}/manim:latest'
  '${{ env.IMAGE_NAMESPACE }}/manim-cpp:stable'
  '${{ env.IMAGE_NAMESPACE }}/manim-cpp:${{ steps.create_release.outputs.tag_name }}'
  '${{ env.IMAGE_NAMESPACE }}/manim:stable'
  '${{ env.IMAGE_NAMESPACE }}/manim:${{ steps.create_release.outputs.tag_name }}'
)

for tag in "${required_tags[@]}"; do
  if ! rg -n -F "${tag}" "${workflow_path}" >/dev/null 2>&1; then
    echo "Docker publish workflow is missing required namespace-aware tag: ${tag}" >&2
    exit 1
  fi
done

echo "Docker publish tag contract check passed."
