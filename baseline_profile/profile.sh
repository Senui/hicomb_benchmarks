#!/bin/bash

if [ -z ${BDMSYS} ]; then
  echo "BDMSYS was not set. Please set it to the directory to where BioDynaMo is installed"
  exit 1;
fi

if [ -z ${FLAMESYS} ]; then
  echo "FLAMESYS was not set. Please set it to the directory to where FlameGraph is installed"
  exit 1;
fi

if ! [ -f ${BDMSYS}/build/cell_growth ]; then
  echo "${BDMSYS}/build/cell_growth was not found. Please make sure that you specified BDMSYS correctly and that you ran cmake and make"
  exit 1;
fi

# We run cell_growth with 64^3 number of cells for 5 iterations with one thread
${BDMSYS}/build/cell_growth 64 5 1 &

# We profile the cell_growth program for at most 60 seconds and generate the
# corresponding Flame Graph
echo "Started profiling..."
perf record --call-graph dwarf -F 99 -p $! -g -- sleep 60
perf script > out.perf
${FLAMESYS}/stackcollapse-perf.pl out.perf > out.folded
${FLAMESYS}/flamegraph.pl out.folded > flamegraph.svg

echo ""
echo "Profiling finished. The Flame Graph can be found at $(pwd)/flamegraph.svg"
