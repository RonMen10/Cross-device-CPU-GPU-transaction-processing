# Cross-device-CPU-GPU-transaction-processing

The conceptual diagram below depicts the project conceptual design having 2 phases; hash build and hash join. Both phases have a router operator which is an additional component to  the  system  in  order to  perform  simultaneous tasks which  is  expected to  improve  the  system’s  overall time efficiency. Router operator would be receiving the data from DBMS as data tables, then partitions will be created and delivered to CPUs and GPUs for concurrent processing.
![](document/hashDiagram.png)
Radix partitioning was applied on the data relations, taking into account cache memory available. After that hash tables will be build and probed subsequently.

![](document/partitioning.png)
 
