# The Goal

Simply put, the main objective for this segment of the program is to bring images into memory as quickly as possible.  
Furthermore, given how fast CPUs and GPUs are its highly likely that at any given stage loading images off an SSD let alone an
HDD will remain a significant bottleneck. Important to consider as well is that immediately following this stage is the CPU
intensive debayering step. Ensuring that the thread pool in that segment always has something to work on is imparitive. \
\

## Benchmarking

We must also keep track of if the changes we are making are meaningful hence we will benchmark and profile as we go. But how we benchmark is
extremely important. We must accurately recreate how the debayering stage acts for initial developement as we make our first improvements.  
However there comes a point where any additional optimizations runs the risk of spending a lot of effort when some other stage is the bottleneck.
Therefore, I think its best that we stop after implementing and benchmarking the general implementations and avoiding fine tuning. \
\
For tools to use in benchmarking I will use perf to profile beyond simple timing and use python scripts to efficiently repeat tests and to compare
different methods. \
\
