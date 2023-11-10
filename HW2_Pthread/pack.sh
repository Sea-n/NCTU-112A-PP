#!/usr/bin/env bash
set -Eeuo pipefail
cd "$(dirname "$0")" || exit 1

7z a HW2_312551015.zip part1/{Makefile,pi.cpp} part2/mandelbrotThread.cpp url.txt
ls -l *.zip
