# riscv-arm-asm-comparator

## Intro
Tool for comparing RISC-V and Arm64 assembly code difference with the same program.
The program will parse the input assembly files, split all subroutines, 
find the basic block in the subroutine and then write the basic-block-information-contained file out.

## Usage
- Compile and run the program, 
  the default input file will be `dumpfiles/aarch64_coremark.dump` and `dumpfiles/riscv64_coremark.dump`
  ```bash
  $ make run
  ```
  if you wish to specify the input file
  ```bash
  $ make run AARCH=[arm asm file] RISCV=[riscv asm file]
  ```
  (compile only)
  ```
  $ make
  ```
- Clean the output executable and output files
  ```bash
  $ make clean
  ```
- The output basic-block-information-contained files would be save under `bbroutines` folder, 
where the `a` folder contains ARM output files and the `r` folder contains RISC-V output files
