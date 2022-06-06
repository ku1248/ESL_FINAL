1.Download the updated software source code to your working directory $EE6470.<br>
cd $EE6470<br>
git clone https://github.com/ku1248/ESL_FINAL.git<br>
cp -r ESL_FINAL/riscv-vp/vp/src/platform/* $EE6470/riscv-vp/vp/src/platform<br>
cp -r ESL_FINAL/riscv-vp/sw/* $EE6470/riscv-vp/sw<br>
<br>
2.Build the "matrix-core1" "matrix-core2" "matrix-core4" platform of riscv-vp:<br>
cd $EE6470<br>
cd riscv-vp/vp/build<br>
cmake ..<br>
make install<br><br>
Build matrix multiplication software:<br>
cd $EE6470<br>
cd riscv-vp/sw<br>

1 PE version:<br>
cd matrix-core1<br>
make<br>
make sim<br>

2 PE version:<br>
cd matrix-core2<br>
make<br>
make sim<br>

4 PE version:<br>
cd matrix-core4<br>
make<br>
make sim<br>
<br><br><br>
HLS part:<br>
// go to stratus directory<br>
cd stratus<br>
<br>
// The behavioral simulation (C++ only).<br>
make sim_B<br>
<br>
//The Verilog simulation for HLS config "BASIC". <br>
make sim_V_BASIC<br>
<br>
//The Verilog simulation for HLS config "UNROLL". <br>
make sim_V_UNROLL<br>
