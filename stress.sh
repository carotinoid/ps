#!/usr/bin/env bash

set -Eeuo pipefail

# --- Configuration ---
VERSION=0.0.3
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" &>/dev/null && pwd -P)
WORKSPACE=$SCRIPT_DIR
TEMP=$WORKSPACE/.stress
SRC=$WORKSPACE/src

# Default Options
FILE_GEN="gen.cpp"
FILE_ANSWER="answer.cpp"
FILE_SUBMIT="submit.cpp"
ATTEMPTS_LIMIT=300
PRINT_CORRECT=0
NO_COLOR=0

# --- Colors ---
setup_colors() {
    if [[ -t 2 ]] && [[ "${NO_COLOR:-0}" -eq 0 ]] && [[ "${TERM-}" != "dumb" ]]; then
        NOFORMAT=$'\033[0m' RED=$'\033[0;31m' GREEN=$'\033[0;32m' ORANGE=$'\033[0;33m' BLUE=$'\033[0;34m' PURPLE=$'\033[0;35m' CYAN=$'\033[0;36m' YELLOW=$'\033[1;33m'
    else
        NOFORMAT='' RED='' GREEN='' ORANGE='' BLUE='' PURPLE='' CYAN='' YELLOW=''
    fi
}


# --- Utilities ---
msg() { echo >&2 -e "${1-}"; }
msg_n() { echo >&2 -e -n "${1-}"; }
die() { local msg=$1; local code=${2-1}; msg "${RED}Error: ${msg}${NOFORMAT}"; exit "$code"; }

usage() {
    cat <<EOF
Usage: $(basename "${BASH_SOURCE[0]}") [-h] [-v] [-g FILE] [-a FILE] [-s FILE]
A robust shell script to stress test a program.

    Options:
    -h, --help              Display this help message.
    -v, --version           Show the script version.
    -g, --gen FILE          Generator file (Default: gen.cpp)
    -a, --ans FILE          Answer file (Default: answer.cpp)
    -s, --sub FILE          Submission file (Default: submit.cpp)
    -m, --more              Print the correct output on success (Default: 0)
    -l, --limit NUM         Attempt limit (Default: 300)
    --flush                 Remove all cached and generated files.
    --no-color              Disable color output.

EOF
    exit 0
}

cleanup() {
    trap - SIGINT SIGTERM ERR EXIT
    # Remove temp files on exit if needed, or keep them for debugging
    # rm -f "$TEMP/input.txt" "$TEMP/output.txt" "$TEMP/answer.txt"
}

get_extension() { echo "${1##*.}"; }

check_hash() {
    local file_path="$1"
    local file_name="$2"
    local hash_dir="${3:-$TEMP}"

    mkdir -p "$hash_dir"
    local full_path="$file_path/$file_name"

    if [[ ! -f "$full_path" ]]; then
        return 0 # File doesn't exist (handled later by compiler)
    fi

    local file_hash
    # Linux(md5sum) vs macOS(md5) compatibility
    if command -v md5sum >/dev/null 2>&1; then
        file_hash=$(md5sum "$full_path" | awk '{print $1}')
    else
        file_hash=$(md5 -q "$full_path")
    fi
    
    local hash_file="$hash_dir/$file_name.hash"
    local stored_hash=""

    if [[ -f "$hash_file" ]]; then
        stored_hash=$(cat "$hash_file")
    fi

    if [[ "$file_hash" == "$stored_hash" ]]; then
        echo "1" # Match
    else
        echo "$file_hash" > "$hash_file"
        echo "0" # Mismatch or New
    fi
}

compile_src() {
    local file_name=$1
    local output_bin=$2
    local extension=$(get_extension "$file_name")
    local source="$SRC/$file_name"
    local compile_log="$TEMP/compile_err"

    msg_n "Compiling $file_name... "

    local ret=0
    case "$extension" in
        cpp)
            g++ -o "$output_bin" -O2 -Wall -std=c++20 "$source" 2> "$compile_log" || ret=$?
            ;;
        c)
            gcc -o "$output_bin" -O2 -Wall -std=c11 "$source" 2> "$compile_log" || ret=$?
            ;;
        java)
            javac -d "$TEMP" "$source" 2> "$compile_log" || ret=$?
            # Create a runner script for consistency
            echo "#!/bin/bash" > "$output_bin"
            echo "java -cp $TEMP ${file_name%.*}" >> "$output_bin"
            chmod +x "$output_bin"
            ;;
        kt)
            kotlinc "$source" -include-runtime -d "${output_bin}.jar" 2> "$compile_log" || ret=$?
            echo "#!/bin/bash" > "$output_bin"
            echo "java -jar ${output_bin}.jar" >> "$output_bin"
            chmod +x "$output_bin"
            ;;
        py)
            # Just check syntax
            if command -v pypy3 >/dev/null 2>&1; then
                pypy3 -c "import py_compile; py_compile.compile('$source')" 2> "$compile_log" || ret=$?
                echo "#!/bin/bash" > "$output_bin"
                echo "pypy3 $source" >> "$output_bin"
            else
                python3 -c "import py_compile; py_compile.compile('$source')" 2> "$compile_log" || ret=$?
                echo "#!/bin/bash" > "$output_bin"
                echo "python3 $source" >> "$output_bin"
            fi
            chmod +x "$output_bin"
            ;;
        *)
            die "Unsupported extension: $extension"
            ;;
    esac

    if [[ $ret -ne 0 ]]; then
        echo -e "${RED}FAILED${NOFORMAT}"
        cat "$compile_log"
        exit 1
    else
        echo -e "${GREEN}DONE${NOFORMAT}"
    fi
}

parse_params() {
    while :; do
        case "${1-}" in
            -h | --help) usage ;;
            -v | --version) msg "version: $VERSION"; exit 0 ;;
            --no-color) NO_COLOR=1 ;;
            --flush) rm -rf "$TEMP"; msg "Cache flushed."; exit 0 ;;
            -g | --gen) FILE_GEN="${2-}"; shift ;;
            -a | --answer) FILE_ANSWER="${2-}"; shift ;;
            -s | --submit) FILE_SUBMIT="${2-}"; shift ;;
            -m | --more) PRINT_CORRECT=1 ;;
            -l | --limit) ATTEMPTS_LIMIT="${2-}"; shift ;;
            -?*) die "Unknown option: $1" ;;
            *) break ;;
        esac
        shift
    done
    return 0
}

# --- Main ---
setup_colors
parse_params "$@"
mkdir -p "$TEMP"

# Check & Compile
for file in "$FILE_GEN" "$FILE_ANSWER" "$FILE_SUBMIT"; do
    if [[ ! -f "$SRC/$file" ]]; then
        die "File not found: $SRC/$file"
    fi
    
    status=$(check_hash "$SRC" "$file")
    binary="$TEMP/${file}.run"
    
    # Recompile if hash changed or binary doesn't exist
    if [[ "$status" == "0" ]] || [[ ! -f "$binary" ]]; then
        compile_src "$file" "$binary"
    else
        msg "Skipping compilation for $file (Up to date)."
    fi
done

msg "${CYAN}Start Stress Testing...${NOFORMAT}"

attempts=0
while (( attempts < ATTEMPTS_LIMIT )); do
    attempts=$((attempts+1))
    
    # Progress Bar (overwrite line)
    printf "\r${YELLOW}[TEST %d / %d]${NOFORMAT} Running..." "$attempts" "$ATTEMPTS_LIMIT"

    # 1. Generate Input
    "$TEMP/${FILE_GEN}.run" > "$TEMP/input.txt"

    # 2. Run Answer (Reference)
    "$TEMP/${FILE_ANSWER}.run" < "$TEMP/input.txt" > "$TEMP/answer.txt"

    # 3. Run Submission (Target)
    "$TEMP/${FILE_SUBMIT}.run" < "$TEMP/input.txt" > "$TEMP/output.txt"

    # 4. Compare (Use diff -w to ignore whitespace diffs)
    if diff -w -q "$TEMP/answer.txt" "$TEMP/output.txt" > /dev/null; then
        # Success
        if (( PRINT_CORRECT )); then
            # If user wants verbose success, clear line and print details
            printf "\r\033[K" 
            msg "Test case $attempts: ${GREEN}Accepted${NOFORMAT}"
            msg "${CYAN}Input:${NOFORMAT}"; cat "$TEMP/input.txt"
            msg "${YELLOW}Output:${NOFORMAT}"; cat "$TEMP/output.txt"
        fi
    else
        # Failure
        printf "\r\033[K" # Clear the current line
        msg "Test case $attempts: ${RED}Wrong Answer${NOFORMAT}"
        msg "${PURPLE}Input:${NOFORMAT}"
        head -n 20 "$TEMP/input.txt"
        [[ $(wc -l < "$TEMP/input.txt") -gt 20 ]] && echo "... (truncated)"
        
        msg "${CYAN}Expected:${NOFORMAT}"
        head -n 20 "$TEMP/answer.txt"
        
        msg "${YELLOW}Received:${NOFORMAT}"
        head -n 20 "$TEMP/output.txt"
        
        die "Found a counter example!"
    fi
done

printf "\r\033[K"
msg "${GREEN}All $attempts test cases passed!${NOFORMAT}"