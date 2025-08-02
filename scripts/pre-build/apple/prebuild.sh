echo "------  SK++ Pre-Build Script - START ------"
echo ""

echo "Bundle mode = $SK_BUNDLE_MODE"
echo ""

OUTPUT_FILE="${SRCROOT}/../../../../sk_target_build_defs.h"


echo "Creating directory: $(dirname "$OUTPUT_FILE")"
mkdir -p "$(dirname "$OUTPUT_FILE")"
chmod 600 "$OUTPUT_FILE"
echo ""

echo "Clearing contents of: $OUTPUT_FILE"
> "$OUTPUT_FILE"
echo ""

case "$SK_BUNDLE_MODE" in
  none)
    #...
    ;;
  shallow)
    SHALLOW_DATA_PATH=$(realpath "${SRCROOT}/../../../../sk_soft_backend_bundle/shallow/groups/data/")
    echo "Shallow data path is: $SHALLOW_DATA_PATH"
    echo ""
    ;;
  deep)
    #...
    ;;
  *)
    echo "[ERROR] No bundling mode set"
    exit 1
    ;;
esac






echo "Writing macros to: $OUTPUT_FILE"
echo ""
echo "Writing bundle mode..."
case "$SK_BUNDLE_MODE" in
  none)
    printf "#define SK_BUNDLE_MODE_NONE 1\n" >> "$OUTPUT_FILE"
    printf "#define SK_BUNDLE_MODE \"none\"\n" >> "$OUTPUT_FILE"
    ;;
  shallow)
    printf "#define SK_ROUTE_FS_TO_BDFS 1\n" >> "$OUTPUT_FILE"
    printf "#define SK_BUNDLE_MODE_SHALLOW 1\n" >> "$OUTPUT_FILE"
    printf "#define SK_BUNDLE_MODE \"shallow\"\n" >> "$OUTPUT_FILE"
    ;;
  deep)
    printf "#define SK_ROUTE_FS_TO_BDFS 1\n" >> "$OUTPUT_FILE"
    printf "#define SK_BUNDLE_MODE_DEEP 1\n" >> "$OUTPUT_FILE"
    printf "#define SK_BUNDLE_MODE \"deep\"\n" >> "$OUTPUT_FILE"
    ;;
esac
echo ""

case "$SK_BUNDLE_MODE" in
  shallow)
    echo "Writing shallow data path..."
    SHALLOW_DATA_PATH=$(realpath "${SRCROOT}/../../../../sk_soft_backend_bundle/shallow/groups/data/")
    
    echo "Shallow data path is: $SHALLOW_DATA_PATH"

    echo "Writing shallow data path..."
    printf "static std::string SK_BUNDLER_SHALLOW_DATA_PATH(\"%s\");\n" "$SHALLOW_DATA_PATH" >> "$OUTPUT_FILE"
    echo ""
    ;;
esac




case "$SK_BUNDLE_MODE" in
  shallow | deep)
    echo "Running prebuild script..."
    node "${SRCROOT}/../../../../skxx/bundler/sk_prebuild_script.js" $SK_BUNDLE_MODE

    echo "Running bundler..."
    node "${SRCROOT}/../../../../skxx/bundler/sk_bundler.js"
    ;;
esac

echo "Applying permissions of \"sk_target_build_defs.h\" to \"600\"..."
chmod 600 "$OUTPUT_FILE"
echo ""

echo "------  SK++ Pre-Build Script - END ------"
