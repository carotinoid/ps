# PS Environment Setup

> **One-command to setup my problem solving environment.**

A streamlined workflow for Competitive Programming using automated Bash scripts and optimized C++ templates.

## Structure

  - `include/`: Custom library headers (FastIO, SegTree, Mint, Poly, etc.)
  - `src/`: Temporary workspace for I/O and stress testing files.
  - `run.sh` / `stress.sh`: Core automation scripts.

## Features

- unner (`run`): Compiles only when file hash changes. Integrated with `fsanitize` for debugging.
- Stress Tester (`stress`): Automated counter-example finder supporting C++, Python, and Java.
- Submit Bundler (`copy.sh`): Recursively expands local headers (`#include "..."`) into a single file and copies to the clipboard via `xclip`.
- Core Library (`include/`): Template snippets.
    * `base.h`: default includes and macros.
    * `debugtools.h`: debugging utilities with colored output.
    * `fastio.h`: fast input using `mmap`.
    * `fenwick.h`: Fenwick Tree implementation.
    * `segtree.h`: Segment Tree implementation.
    * `mint.h`: Modular Integer class.
    * `polynomial.h`: Polynomial class with NTT.
    * `parametric`: Parametric Search utilities.
    * `pbds.h`: pbds includes wrapper.

## Quick Start

```bash
# 1. Install dependency (xclip, compiler)
sudo apt install xclip g++ 

# 2. Setup aliases (run, stress)
chmod +x setup.sh
./setup.sh
source ~/.bashrc
```

## Usage

### Compile & Run

```bash
run                # Run main.cpp (Default)
run -f A.cpp       # Run specific file
run -i             # Use src/input.txt as stdin
run -t 5           # Set timeout to 5 seconds
run --flush        # Clear cache
```

### Stress Testing

```bash
# Compare 'submit.cpp' vs 'answer.cpp' using 'gen.cpp'
stress -s submit.cpp -a answer.cpp -g gen.cpp
```

### Submission

```bash
# Expand headers and copy to clipboard
./copy.sh main.cpp
```

### and more...
- Utilize `tasks.json` and `keybindings.json` for VSCode integration, you can bind tasks to keys (e.g., F9 for running, F10 for copying).