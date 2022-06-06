General description or introduction of the problem and your solution:

For this final project, I implemented 8 by 8 matrix multiplication on multi-core system. The reason that I chose to implement matrix multiplication is that it is able to be parallelized. Since we were required to use different number of PEs to implement the same function, matrix multiplication can be implemented with different number of PEs by changing the number of rows that each PE will calculate, refer to following picture. 

![](Aspose.Words.f61af2be-e54c-45c4-bc03-2a279ea17b82.001.png)

![](Aspose.Words.f61af2be-e54c-45c4-bc03-2a279ea17b82.002.png)

To make my final project more complete, I implemented it on RiscV vp platform and with different number of cores and PEs, also, I implemented it on Stratus with different number of PEs and observe the synthesis result.

Implementation details (data structure, flows and algorithms):

For matrix multiplication, there will be three matrices. The three matrices are A, B and C, and the formula is A\*B=C. So, on RiscV vp, each PE will receive several rows of A and whole B from software, and then after calculate, send the partial result of C back to software. So, the software part is to control which data to send and receive the result. How the data will be sent is related to the number of PEs. To control how the data is sent in software, we will have to implement it with multi-core. For example, if each PE calculates 2 rows of result, then, software part will send whole matrix B to each PE, and then send 2 rows of A to each PE, and then, receive the result from each PE, in this case, the number of PE is 4 and we will need 4 cores to control the way of sending data, each core corresponds to different PE that it sends data to and receive data from. And the calculation of each element is finished in the  PEs. And the overall system connection will be as following:

![](Aspose.Words.f61af2be-e54c-45c4-bc03-2a279ea17b82.003.png)

Additional features of your design and models:

The way that how the data is transferred will also be compared. I compared the case of using DMA with the case of using memcpy function. The comparison result will be showed at next section. Also, I used different number of PEs to implement the same function and then observe the results. The number of PEs I choose is 1, 2, and 4. Also, to implement with several PEs, multi-core is needed. So, 1 PE version will have 1 core, 2 PEs version will have 2 cores, and 4 PEs version will have 4 cores. And after the implementation on RiscV vp, I implement the same function on Stratus and then get the synthesis results when changing the number of PEs.

Experimental results (on different benchmarks and settings):

I experimented my implementation by changing the number of PEs on RiscV vp, using DMA or not and the synthesis result of different PEs. The comparison table of different numbers of PEs and if using DMA or not on RiscV vp is showed as following:

![](Aspose.Words.f61af2be-e54c-45c4-bc03-2a279ea17b82.004.png)![](Aspose.Words.f61af2be-e54c-45c4-bc03-2a279ea17b82.005.png)

![](Aspose.Words.f61af2be-e54c-45c4-bc03-2a279ea17b82.006.png)![](Aspose.Words.f61af2be-e54c-45c4-bc03-2a279ea17b82.007.png)![](Aspose.Words.f61af2be-e54c-45c4-bc03-2a279ea17b82.008.png)![](Aspose.Words.f61af2be-e54c-45c4-bc03-2a279ea17b82.009.png)

The comparison table of synthesis result of different number of PEs is showed as following:

![](Aspose.Words.f61af2be-e54c-45c4-bc03-2a279ea17b82.010.png)

Discussions and conclusions:

` `From the experimental result, if we observe the case of changing the number of PEs and transfer data with DMA, we can find that the simulation time of 2 PEs version is only 59% of the simulation time of 1 PE version. And the simulation time of 4 PEs version is 46% of the simulation time of 1 PE version (78% of the simulation time of 2 PEs version). The simulation time drop is more obvious when comparing 1 PE to 2 PEs version. I think the reason may be that since there are many data to transfer in the system, the system spends a lot of time transferring the data, the calculation will be started until the data it need is received. So, when we observe the simulation time in 4 PEs version, it contains a large part of simulation time for transferring data. Thus, it leads to that the simulation time is not that obvious. Also, while comparing the simulation time of using different numbers of PEs, at first, the simulation time drop is very little while adding PEs. And then I found that because I use printf to print out the result, and it occupies a lot of simulation time. So, after I checked the result is correct, I omitted the printf function in order to get more reality number of cycle drop for more PEs.

Also, compare the simulation time and instruction number of using DMA and not using DMA, we can find that when we use DMA for data transfer, the simulation time and number of instructions drops. The way I connected my system with DMA is that for every core in the system, it has its own DMA to transfer data. It is reasonable since that if we use DMA to transfer data, we do not need CPU to transfer data so the instructions in CPU will not have to wait for data to transfer. By comparing the simulation time of using DMA or not using DMA, I found that by using DMA to transfer data, for 4 bytes data transfer, using DMA can save about 34 cycles (1 cycle is 10 ns) for 4 bytes data comparing to not using DMA.

From Stratus synthesis result, we can estimate the end-end simulated cycles at software. Since the total latency for 1 PE version is 383 cycles, simulation time is 3830ns, so the end-end simulated cycles at software is 23057 cycles. The performance for different number of PEs by multiplying area and total cycles. I found that the performance shows that the performance of 1 PE is better than 4 PEs and is better than 2 PEs.
