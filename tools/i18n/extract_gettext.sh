#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'EOF' >&2
Usage: extract_gettext.sh [--check-only] [repo_root]

Generates deterministic gettext templates for the C++ docs pipeline:
  docs/i18n/gettext/book.pot
  docs/i18n/gettext/api.pot
EOF
}

check_only=0
repo_root=""

while [[ $# -gt 0 ]]; do
  case "$1" in
    --check-only)
      check_only=1
      shift
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      if [[ -n "${repo_root}" ]]; then
        usage
        exit 2
      fi
      repo_root="$1"
      shift
      ;;
  esac
done

if [[ -z "${repo_root}" ]]; then
  repo_root="$(cd "$(dirname "$0")/../.." && pwd)"
fi

book_root="${repo_root}/docs/book/src"
api_root="${repo_root}/include/manim_cpp"
gettext_root="${repo_root}/docs/i18n/gettext"
book_pot="${gettext_root}/book.pot"
api_pot="${gettext_root}/api.pot"

if [[ ! -d "${book_root}" ]]; then
  echo "Missing mdBook source directory: ${book_root}" >&2
  exit 1
fi
if [[ ! -d "${api_root}" ]]; then
  echo "Missing public API headers directory: ${api_root}" >&2
  exit 1
fi

tmp_dir="$(mktemp -d)"
trap 'rm -rf "${tmp_dir}"' EXIT

book_tsv="${tmp_dir}/book.tsv"
api_tsv="${tmp_dir}/api.tsv"
new_book_pot="${tmp_dir}/book.pot"
new_api_pot="${tmp_dir}/api.pot"

generate_markdown_tsv() {
  local output_tsv="$1"
  local markdown_files=()
  while IFS= read -r file; do
    markdown_files+=("${file}")
  done < <(find "${book_root}" -type f -name '*.md' | LC_ALL=C sort)

  if [[ ${#markdown_files[@]} -eq 0 ]]; then
    : >"${output_tsv}"
    return
  fi

  awk -v root="${book_root}/" '
    function trim(text) {
      sub(/^[[:space:]]+/, "", text)
      sub(/[[:space:]]+$/, "", text)
      return text
    }
    BEGIN {
      in_code_block = 0
    }
    {
      line = $0
      gsub(/\r/, "", line)

      if (line ~ /^[[:space:]]*```/) {
        in_code_block = !in_code_block
        next
      }
      if (in_code_block) {
        next
      }

      while (match(line, /\[[^][]+\]\([^()]*\)/)) {
        prefix = substr(line, 1, RSTART - 1)
        token = substr(line, RSTART, RLENGTH)
        suffix = substr(line, RSTART + RLENGTH)
        sub(/^\[/, "", token)
        sub(/\]\([^()]*\)$/, "", token)
        line = prefix token suffix
      }

      sub(/^[[:space:]]*#+[[:space:]]*/, "", line)
      sub(/^[[:space:]]*[-*+][[:space:]]+/, "", line)
      sub(/^[[:space:]]*[0-9]+\.[[:space:]]+/, "", line)

      line = trim(line)
      if (line == "") {
        next
      }
      if (line ~ /^<!--/) {
        next
      }

      rel = FILENAME
      if (index(rel, root) == 1) {
        rel = substr(rel, length(root) + 1)
      }
      print line "\t" rel ":" FNR
    }
  ' "${markdown_files[@]}" >"${output_tsv}"
}

generate_doxygen_tsv() {
  local output_tsv="$1"
  local header_files=()
  while IFS= read -r file; do
    header_files+=("${file}")
  done < <(find "${api_root}" -type f \( -name '*.hpp' -o -name '*.h' \) | LC_ALL=C sort)

  if [[ ${#header_files[@]} -eq 0 ]]; then
    : >"${output_tsv}"
    return
  fi

  awk -v root="${repo_root}/" '
    function trim(text) {
      sub(/^[[:space:]]+/, "", text)
      sub(/[[:space:]]+$/, "", text)
      return text
    }
    {
      line = $0
      rel = FILENAME
      if (index(rel, root) == 1) {
        rel = substr(rel, length(root) + 1)
      }

      if (line ~ /^[[:space:]]*\/\/\//) {
        sub(/^[[:space:]]*\/\/\/[<!]?[[:space:]]?/, "", line)
        line = trim(line)
        if (line != "") {
          print line "\t" rel ":" FNR
        }
      }
    }
  ' "${header_files[@]}" >"${output_tsv}"
}

write_pot_from_tsv() {
  local input_tsv="$1"
  local output_pot="$2"
  local project_component="$3"

  {
    cat <<EOF
msgid ""
msgstr ""
"Project-Id-Version: manim-cpp ${project_component}\\n"
"POT-Creation-Date: 1970-01-01 00:00+0000\\n"
"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\\n"
"Last-Translator: FULL NAME <EMAIL@ADDRESS>\\n"
"Language-Team: LANGUAGE <LL@li.org>\\n"
"MIME-Version: 1.0\\n"
"Content-Type: text/plain; charset=UTF-8\\n"
"Content-Transfer-Encoding: 8bit\\n"

EOF

    if [[ -s "${input_tsv}" ]]; then
      LC_ALL=C sort -t $'\t' -k1,1 -k2,2 "${input_tsv}" | awk -F '\t' '
        function escape_po(text) {
          gsub(/\\/, "\\\\", text)
          gsub(/"/, "\\\"", text)
          return text
        }
        function flush_entry() {
          if (current_msgid == "") {
            return
          }
          print "#: " refs
          print "msgid \"" escape_po(current_msgid) "\""
          print "msgstr \"\""
          print ""
        }
        {
          msgid = $1
          ref = $2
          if (msgid != current_msgid) {
            flush_entry()
            current_msgid = msgid
            refs = ref
            previous_ref = ref
            next
          }

          if (ref != previous_ref) {
            refs = refs " " ref
            previous_ref = ref
          }
        }
        END {
          flush_entry()
        }
      '
    fi
  } >"${output_pot}"
}

ensure_matches_expected() {
  local generated_file="$1"
  local expected_file="$2"

  if [[ ! -f "${expected_file}" ]]; then
    echo "Missing tracked gettext template: ${expected_file}" >&2
    return 1
  fi

  if ! diff -u "${expected_file}" "${generated_file}" >/dev/null; then
    echo "Gettext template drift detected: ${expected_file}" >&2
    echo "Run tools/i18n/extract_gettext.sh to regenerate templates." >&2
    return 1
  fi

  return 0
}

generate_markdown_tsv "${book_tsv}"
generate_doxygen_tsv "${api_tsv}"
write_pot_from_tsv "${book_tsv}" "${new_book_pot}" "book"
write_pot_from_tsv "${api_tsv}" "${new_api_pot}" "api"

mkdir -p "${gettext_root}"

if [[ "${check_only}" -eq 1 ]]; then
  ensure_matches_expected "${new_book_pot}" "${book_pot}"
  ensure_matches_expected "${new_api_pot}" "${api_pot}"
  echo "Gettext templates are up to date."
  exit 0
fi

cp "${new_book_pot}" "${book_pot}"
cp "${new_api_pot}" "${api_pot}"
echo "Updated gettext templates:"
echo "  ${book_pot}"
echo "  ${api_pot}"
