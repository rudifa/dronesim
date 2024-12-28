# dronesim

> a 1-D drone simulation

![Simulation Image](img/dronesim-02.png)

## build with cmake

```
dronesim % mkdir -p build && cd build
build % cmake ..
build % cmake --build .
```

## simple build and run

```
dronesim %cd src/dronesim
dronesim %g++ -std=c++23 -O2 main.cpp dronesim.cpp -o bin/dronesim
dronesim %bin/dronesim
```

Run will print a table of results:

```
dronesim % bin/dronesim                                                                     [cpp-print-results ↑1|●3✚1]
Time    Altitude    Velocity    Thrust  Acceleration    Target Alt
0   0.1019      1.019       30  10.19       100
0.1 0.305658        2.03758     30  10.1858     100
0.2 0.611146        3.05488     30  10.173      100
...
```

# MSPlot : a math simple plot

> A tiny subset of mathplotlib features
