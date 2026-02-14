#!/usr/bin/env bash
set -euo pipefail

strict=0
if [[ "${1:-}" == "--strict" ]]; then
  strict=1
fi

matches=$(git ls-files | rg -n '\.(py|pyi)$' || true)
if [[ -z "$matches" ]]; then
  echo "No tracked Python files detected."
  exit 0
fi

echo "Tracked Python files detected:"
echo "$matches"

if [[ "$strict" -eq 1 ]]; then
  echo "Strict mode enabled: failing due to Python files."
  exit 1
fi

echo "Strict mode disabled: reporting only."
exit 0
