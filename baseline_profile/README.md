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
5. `cd $BDMSYS && git checkout 8b3d6c7039fb6f4ed70d47950181d7416cb643e0`
6. `git apply fix.patch` (patch from this repository)
7. `mkdir $BDMSYS/build; cd $BDMSYS/build`
8.  `cmake .. && make`
9.  `export BDMSYS=$BDMSYS; export FLAMESYS=$FLAMESYS`
10. `./profile.sh` (profile.sh from this repository)


# Important
To run perf it might be necessary it set `/proc/sys/kernel/perf_event_paranoid` to -1.