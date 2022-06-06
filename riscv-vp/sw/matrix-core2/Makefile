all : main.cpp bootstrap.S
	riscv32-unknown-elf-g++ main.cpp bootstrap.S -o main -nostartfiles -march=rv32ima -mabi=ilp32
	
sim: all
	$(EE6470)/riscv-vp/vp/build/bin/riscv-vp-matrix-core2 --intercept-syscalls main
	
clean:
	rm -f main