#!/usr/bin/env bash

FILE=${1:-main.cpp}
INCLUDE_DIR="include"

BLACKLIST=("debugtools.h")

if ! command -v xclip &> /dev/null; then
    echo "Error: xclip is not installed."
    exit 1
fi

if [ ! -f "$FILE" ]; then
    echo "Error: File '$FILE' not found."
    exit 1
fi

(
    while IFS= read -r line || [ -n "$line" ]; do
        if [[ "$line" =~ ^[[:space:]]*#include[[:space:]]*\"include/(.+)\" ]]; then
            TARGET_FILE="${BASH_REMATCH[1]}"
            FULL_PATH="$INCLUDE_DIR/$TARGET_FILE"
            
            IS_BLACKLISTED=0
            for ban in "${BLACKLIST[@]}"; do
                if [[ "$ban" == "$TARGET_FILE" ]]; then
                    IS_BLACKLISTED=1
                    break
                fi
            done

            if [[ $IS_BLACKLISTED -eq 1 ]]; then
                echo "// [Excluded] $line"
            elif [ -f "$FULL_PATH" ]; then
                echo "// [Expanded] $line"
                cat "$FULL_PATH"
                echo "" 
            else
                echo "Warning: '$FULL_PATH' not found. Keeping original line." >&2
                echo "$line"
            fi
        else
            echo "$line"
        fi
    done < "$FILE"
) | xclip -selection clipboard

echo -e "\033[32mSuccessfully copied '$FILE' with headers expanded to clipboard!\033[0m"