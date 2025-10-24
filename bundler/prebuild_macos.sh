#!/usr/bin/env bash
set -euo pipefail


find_rezonant() {
  local start="${1:-$PWD}"
  local dir

  dir="$(cd -P "$start" 2>/dev/null && pwd)" || return 1
  while :; do
    if [ -d "$dir/rezonant" ]; then
      printf '%s\n' "$dir/rezonant"
      return 0
    fi
    [ "$dir" = "/" ] && return 2
    dir="$(dirname "$dir")"
  done
}

REZONANT_DIR="$(find_rezonant "${SRCROOT:-$PWD}")" || {
  echo "[prebuild] ERROR: 'rezonant' folder not found (start=${SRCROOT:-$PWD})"
  exit 3
}


# Inputs
SCHEME="${1:-}"

# Config
IDE="xcode"
PREBUILD_JS="${REZONANT_DIR}/skxx/bundler/sk_prebuild_script.js"
INSPECT_PORT="${SK_NODE_INSPECT_PORT:-9229}"

# Context
echo "[prebuild] Scheme: ${SCHEME:-<none passed>}"
echo "[prebuild] IDE: ${IDE}"
echo "[prebuild] Prebuild JS: ${PREBUILD_JS}"
echo "[prebuild] Inspector: --inspect-brk=127.0.0.1:${INSPECT_PORT}"

# Checks
[[ -f "$PREBUILD_JS" ]] || { echo "[prebuild] ERROR: script not found: $PREBUILD_JS"; exit 3; }
if ! /usr/bin/env node -v >/dev/null 2>&1; then
  echo "[prebuild] ERROR: Node.js not found in PATH for this build env."
  exit 1
fi

# Run with inspector (breaks on first line)
echo "[prebuild] Exec: node --inspect-brk=127.0.0.1:${INSPECT_PORT} \"$PREBUILD_JS\" --ide \"$IDE\" --config \"$SCHEME\""
exec /usr/bin/env node "$PREBUILD_JS" --ide "$IDE" --config "$SCHEME"
