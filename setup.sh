#!/bin/bash

script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" &>/dev/null && pwd -P)
MARKER="### alias commands for ps tools ###"

if grep -qF "$MARKER" ~/.bashrc; then
    echo "Alias commands already exist in ~/.bashrc"
    exit 0
fi

echo "" >> ~/.bashrc
echo "### alias commands for ps tools ###" >> ~/.bashrc
echo "alias run=$script_dir/run.sh" >> ~/.bashrc
echo "alias stress=$script_dir/stress.sh" >> ~/.bashrc
echo "alias copy=$script_dir/copy.sh" >> ~/.bashrc
echo "### End of alias commands ###" >> ~/.bashrc

echo "The alias commands were added in ~/.bashrc"

mkdir -p "$script_dir/src"
touch "$script_dir/src/input.txt"
touch "$script_dir/src/output.txt"
touch "$script_dir/src/answer.cpp"
touch "$script_dir/src/gen.cpp"
touch "$script_dir/src/submit.cpp"
 
echo "Created src/ directory with template files."