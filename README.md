# lc3-vm
LC-3 Virtual Machine written in C. 

## Overview
A virtual machine (VM) is simply a program that acts like a computer as it simulates a CPU along with a few other hardware components. It performs arithmetic, read and write to memory, and interact with I/O devices. Most importantly, it's able to understand machine language. 

Our version will simulate a LC-3, which is an educational computer architecture commonly used in universities. It's great for us that it has a simplified instruction set, but demonstrates the main ideas used by modern CPUs. This allows us to run simple games such as 2048, and rogue on the terminal. 

Building this was fun since due to my keen knowledge on low level architecture both in C and assembly, and my continued interest in areas of low level specifically. ~~I did this side project to grasp a real project...~~

## How to Run:
- Run `make` to compile the `lc3vm.c` file.
- Run `./lc3 roms/FILENAME.obj` to run games!

**NOTE**, MSYS2 MINGW64 (gcc) is a requirement.

## Source: 
- [LC-3 VM Tutorial](https://www.jmeiners.com/lc3-vm/#what-is-a-virtual-machine-)
- [High level overview of all instructions](https://www.jmeiners.com/lc3-vm/supplies/lc3-isa.pdf)

