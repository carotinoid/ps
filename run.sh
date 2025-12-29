#!/usr/bin/env bash

###### Basic functions ######

msg()  { echo >&2 -e "${1-}"; }
msg_n()  { echo >&2 -e -n "${1-}"; }
cleanup_file()  {
    if [[ -f $1 ]]; then
        rm $1
    fi
}
print_line() {
    local title=${1-""}
    local char=${2-"="}
    local cols=$(tput cols || echo 80)
    
    if [[ -z "$title" ]]; then
        printf '%*s\n' "$cols" '' | tr ' ' "$char"
    else
        local line_len=$(( (cols - ${#title}) / 2 - 1 ))
        [[ $line_len -lt 0 ]] && line_len=0
        
        local line=$(printf '%*s' "$line_len" '' | tr ' ' "$char")
        printf "%s %s %s\n" "$line" "$title" "$line"
    fi
}
erase()  {
    for ((i=0;i<${1-1};i++)); do
        msg_n "\b \b"
    done
}

die()  {
    local msg=$1
    local code=${2-1}
    msg "$msg"
    exit "$code"
}

setup_colors()  {
    if [[ -t 2 ]] && [[ -z "${NO_COLOR-}" ]] && [[ "${TERM-}" != "dumb" ]]; then
        NOFORMAT='\033[0m' RED='\033[0;31m' GREEN='\033[0;32m' ORANGE='\033[0;33m' BLUE='\033[0;34m' PURPLE='\033[0;35m' CYAN='\033[0;36m' YELLOW='\033[1;33m'
    else
        NOFORMAT='' RED='' GREEN='' ORANGE='' BLUE='' PURPLE='' CYAN='' YELLOW=''
    fi
}

check_file_hash()  {
    local file_path=$1
    local file_name=$2
    local hash_dir=${3:-$TEMP}

    mkdir -p "$hash_dir"

    local file_hash=$(md5sum "$file_path"/"$file_name" | awk '{print $1}')
    local hash_file="$file_name.hash"

    if [[ -f "$hash_dir/$hash_file" ]]; then
        local stored_hash=$(cat "$hash_dir/$hash_file")

        if [[ "$file_hash" == "$stored_hash" ]]; then
            echo "1"
        else
            echo "$file_hash" > "$hash_dir/$hash_file"
            echo "2"
        fi
    else
        echo "$file_hash" > "$hash_dir/$hash_file"
        echo "0"
    fi
}

get_extension()  {
    local file_name=$1
    echo "${file_name##*.}"
}

###### End ######

set -Euo pipefail
script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" &>/dev/null && pwd -P)

WORKSPACE=$script_dir
TEMP=$WORKSPACE/.ps
SRC_TARGET=$WORKSPACE
SRC_OTHER=$WORKSPACE/src #input, output

FILE_CODE=main.cpp

USE_EXIST_INPUT=0
CHECK_TIMEOUT=0
CHECK_RUNTIME_ERROR=0
EXIT_CODE=0
ExecutionTime=2
VERSION=0.0.2
OLD_IFS=$IFS


usage() {
    cat <<EOF
Usage: $(basename "${BASH_SOURCE[0]}") [-h] [-v] [-f FILE] [-i] [-t TIME]
C++ Compile & Run Helper Script.

Options:
  -h, --help      Display this help message.
  -v, --version   Show version.
  -f, --file      Specify C++ file (Default: main.cpp).
  -i, --input     Use existing input.txt (Don't ask for input).
  -t, --time      Set timeout in seconds (Default: 2s).
  -d, --direct    Run existing binary directly.
  --flush         Clear cache and temp files.

EOF
    exit 0
}

cleanup() {
    trap - SIGINT SIGTERM ERR EXIT
    IFS=$OLD_IFS
    if [[ -f $SRC_OTHER/input.txt ]]; then
        print_line "Input" 
        msg "$(cat $SRC_OTHER/input.txt)"
        print_line 
    fi

    if (( CHECK_TIMEOUT )); then
        msg "${RED}[FAIL] Time Limit Exceeded${NOFORMAT} (> ${ExecutionTime}s)"
        print_line
    elif (( CHECK_RUNTIME_ERROR )); then
        msg "${RED}[FAIL] Runtime Error${NOFORMAT} (Exit Code: $EXIT_CODE)"
        print_line
    fi
    if [[ -s $SRC_OTHER/output.txt ]]; then
        print_line "Output" 
        msg "$(cat $SRC_OTHER/output.txt)"
        print_line
    fi
    if [[ -s $TEMP/exec_err ]]; then
        print_line "ERROR" 
        msg "$(cat $TEMP/exec_err)"
        print_line
    fi
}

parse_params() {
    while :; do
        case "${1-}" in
        --no-color) NO_COLOR=1 ;;
        --verbose) set -x ;;
        --flush) 
        rm $TEMP/*
        msg "The hash files and execution files were flushed."
        exit 0
        ;;
        -v | --version)
        msg "version: $VERSION"
        exit 0
        ;;
        -h | --help) usage ;;
        -i | --input) USE_EXIST_INPUT=1 ;; 
        -f | --file) 
        FILE_CODE="${2-}"
        shift ;;
        -t | --time)
        ExecutionTime="${2-}"
        shift ;;
        -d | --direct)
        g++ -o $TEMP/$FILE_CODE.run \
            -I$WORKSPACE \
            -O2 \
            -DDEBUGTOOLS \
            -fsanitize=undefined,address \
            -Wno-unused-but-set-variable \
            -Wno-unused-variable \
            -lm \
            -std=c++2a \
            -g $SRC_TARGET/$FILE_CODE 2> $TEMP/compile_err 
        msg "compile done"
        if [[ -f "$TEMP/$FILE_CODE.run" ]]; then
            "$TEMP/$FILE_CODE.run"
            exit $?
        else
            die "Error: Binary not found at $TEMP/$FILE_CODE.run"
        fi
        ;;
        -?*) die "Unknown option: $1" ;;
        *) break ;;
        esac
        shift
    done
    args=("$@")
    return 0
}

# Setup
parse_params "$@"
setup_colors

mkdir -p "$TEMP"
mkdir -p "$SRC_OTHER"

trap cleanup SIGINT SIGTERM ERR EXIT
cleanup_file $SRC_OTHER/output.txt
cleanup_file $TEMP/exec_err
cleanup_file $TEMP/compile_err

# Check update
need_compile=0
CompileDoneNumofBackspace=10

checking_hash=$(check_file_hash $SRC_TARGET $FILE_CODE)
if [[ $checking_hash -eq 0 ]]; then
    msg_n "No hash, ${ORANGE}compiling.${NOFORMAT}"
    need_compile=1
elif [[ $checking_hash -eq 1 ]]; then
    msg "Execution file already exists, ${GREEN}no compile${NOFORMAT}."
else
    msg_n "Updated, ${ORANGE}compiling.${NOFORMAT}"
    need_compile=1
fi

# Compile
time_startCompile=$(date +%s.%3N)
if (( need_compile )); then

    g++ -o $TEMP/$FILE_CODE.run \
        -I$WORKSPACE \
        -O2 \
        -DDEBUGTOOLS \
        -fsanitize=undefined,address \
        -Wno-unused-but-set-variable \
        -Wno-unused-variable \
        -lm \
        -std=c++2a \
        -g $SRC_TARGET/$FILE_CODE 2> $TEMP/compile_err &
    pid=$!
    while kill -0 $pid 2>/dev/null; do
        sleep 0.5
        (( CompileDoneNumofBackspace++ ))
        msg_n "${ORANGE}.${NOFORMAT}"
    done
    erase $CompileDoneNumofBackspace

    if wait $pid; then
        :
    else
        msg "${RED}Compile incomplete!${NOFORMAT}"
        cat "$TEMP/compile_err"
        rm "$TEMP/$FILE_CODE.hash"
        trap - SIGINT SIGTERM ERR EXIT
        exit 1
    fi

    time_endCompile=$(date +%s.%3N)
    time_elapsedCompile=$(echo "$time_endCompile - $time_startCompile" | bc)
    msg_n "${GREEN}Compile Done!${NOFORMAT} "
    msg_n "${GREEN}($time_elapsedCompile"
    msg "s)${NOFORMAT}"
fi

# Input
if ! (( USE_EXIST_INPUT )); then
    msg "${CYAN}[Input Data] Paste content and press Ctrl+D:${NOFORMAT}"
    cat > $SRC_OTHER/input.txt
fi

# Execution
set +e 
timeout "$ExecutionTime" "$TEMP/$FILE_CODE.run" < "$SRC_OTHER/input.txt" > "$SRC_OTHER/output.txt" 2> "$TEMP/exec_err"
EXIT_CODE=$?
set -e

if [[ $EXIT_CODE -eq 124 ]]; then
    CHECK_TIMEOUT=1
elif [[ $EXIT_CODE -ne 0 ]]; then
    CHECK_RUNTIME_ERROR=1
fi

# trap on exit