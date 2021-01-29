# Description
This folder contains the code used to profile the benchmark of the baseline version of BioDynaMo to understand the computational bottlenecks.
We use a Flame Graph to visualize the profiling data.

# Steps to reproduce
Let's assume:
- `BDMSYS` = directory where we install BioDynaMo
- `ROOTSYS` = directory where we install ROOT
- `FLAMESYS` = directory where we install FlameGraph

1. Download and install the latest ROOT from https://root.cern to `$ROOTSYS`
2. `source $ROOTSYS/bin/thisroot.sh`
3. `git clone https://github.com/BioDynaMo/biodynamo.git $BDMSYS`
4. `git clone https://github.com/brendangregg/FlameGraph.git $FLAMESYS`
5. `cd $BDMSYS && git checkout paper-baseline`
6. `mkdir $BDMSYS/build; cd $BDMSYS/build`
7.  `cmake .. && make`
8.  `export BDMSYS=$BDMSYS; export FLAMESYS=$FLAMESYS`
9.  `./profile.sh` (profile.sh from this repository)


# Important
To run perf it might be necessary it set `/proc/sys/kernel/perf_event_paranoid` to -1.
