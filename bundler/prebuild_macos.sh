#!/usr/bin/env bash
set -euo pipefail

# Inputs
SCHEME="${1:-}"

# Config
IDE="xcode"
PREBUILD_JS="${SRCROOT}/../../../../skxx/bundler/sk_prebuild_script.js"
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
