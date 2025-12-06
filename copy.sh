#!/usr/bin/env bash

FILE=${1:-main.cpp}

if ! command -v xclip &> /dev/null; then
    echo "Error: xclip is not installed."
    exit 1
fi

if [ ! -f "$FILE" ]; then
    echo "Error: File '$FILE' not found."
    exit 1
fi

declare -A PROCESSED_FILES

expand_file() {
    local current_file="$1"
    
    if [ ! -f "$current_file" ]; then
        echo "// Warning: Cannot read '$current_file'"
        return
    fi

    while IFS= read -r line || [ -n "$line" ]; do
        if [[ "$line" =~ ^[[:space:]]*#include[[:space:]]*\"(include/[^\"]+)\" ]]; then
            local target_path="${BASH_REMATCH[1]}"

            if [ -f "$target_path" ]; then
                if [ -z "${PROCESSED_FILES["$target_path"]}" ]; then
                    PROCESSED_FILES["$target_path"]=1
                    
                    echo "// [Expanded] $line"
                    expand_file "$target_path"
                    echo "" 
                else
                    echo "// [Skipped] $line (Already included)"
                fi
            else
                echo "// Warning: '$target_path' not found on disk." >&2
                echo "$line"
            fi
        else
            echo "$line"
        fi
    done < "$current_file"
}

PROCESSED_FILES["$FILE"]=1
expand_file "$FILE" | xclip -selection clipboard

echo -e "\033[32mSuccessfully copied '$FILE' with headers expanded to clipboard!\033[0m"