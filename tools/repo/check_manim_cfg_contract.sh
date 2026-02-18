#!/usr/bin/env bash
set -euo pipefail

if [[ $# -gt 1 ]]; then
  echo "Usage: check_manim_cfg_contract.sh [repo_root]" >&2
  exit 2
fi

repo_root="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"
cfg_path="${repo_root}/config/manim.cfg.default"

if [[ ! -f "${cfg_path}" ]]; then
  echo "Missing config contract file: ${cfg_path}" >&2
  exit 1
fi

section_has_key() {
  local section="$1"
  local key="$2"
  awk -v section="${section}" -v key="${key}" '
    BEGIN { in_section = 0; found = 0 }
    /^\[.*\]$/ {
      in_section = ($0 == "[" section "]")
      next
    }
    in_section && $0 ~ "^[[:space:]]*" key "[[:space:]]*=" {
      found = 1
      exit 0
    }
    END {
      if (found == 1) {
        exit 0
      }
      exit 1
    }
  ' "${cfg_path}"
}

required_sections=(
  "CLI"
  "custom_folders"
  "logger"
  "ffmpeg"
  "jupyter"
)

required_keys=(
  "CLI:renderer"
  "CLI:format"
  "CLI:media_dir"
  "CLI:log_dir"
  "CLI:video_dir"
  "CLI:sections_dir"
  "CLI:images_dir"
  "CLI:partial_movie_dir"
  "CLI:frame_rate"
  "CLI:pixel_height"
  "CLI:pixel_width"
  "CLI:enable_gui"
  "CLI:window_position"
  "CLI:window_size"
  "CLI:window_monitor"
  "custom_folders:media_dir"
  "custom_folders:video_dir"
  "custom_folders:sections_dir"
  "custom_folders:images_dir"
  "custom_folders:text_dir"
  "custom_folders:tex_dir"
  "custom_folders:log_dir"
  "custom_folders:partial_movie_dir"
  "logger:logging_level_info"
  "logger:logging_level_error"
  "logger:log_timestamps"
  "ffmpeg:loglevel"
  "jupyter:media_embed"
  "jupyter:media_width"
)

for section in "${required_sections[@]}"; do
  if ! rg -n --fixed-strings "[${section}]" "${cfg_path}" >/dev/null 2>&1; then
    echo "Missing required section [${section}] in ${cfg_path}" >&2
    exit 1
  fi
done

for entry in "${required_keys[@]}"; do
  section="${entry%%:*}"
  key="${entry##*:}"
  if ! section_has_key "${section}" "${key}"; then
    echo "Missing required key '${key}' in section [${section}] of ${cfg_path}" >&2
    exit 1
  fi
done

echo "manim.cfg section/key compatibility contract verified."
