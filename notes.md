### Memory
- Memory is a piece of hardware that allows us computer to store information
- RAM(Random Access Memory) is your computers main memory, here programs can read and write information
- ROM(Read Only Memory) is a form of memory that can only be read from

- RAM is readable and writeable memory, it is used for temporary storage and all data in it is lost when you power down the computer
- ROM cannot be written to through normal means, it is a permanent storage and will maintain data after power loss, used foe programs taht will never change e.g embedded devices - code in kettle, oven, BIOS is stored in ROM  

### The Boot Process
- The BIOS is executed directly from ROM
- The BIOS loads the bootloader into address 0x7c00
- The Bootloader loads the kernel

### What is a bootloader
Small program responsible for loading kernel of an OS, small programs

### BIOS
The BIOS is executed directly from ROM, it will also initialize essential hardware
Loads bootloader by searching all storage mediums for the boot signature 0x55aa at the end of a sector (512 bytes usually)
Loads the bootloader into RAM at absolute address (physical address) 0x7c00
Performs jump to that address

- The BIOS contains routins to assist our bootloader in booting our kernel
- The BIOS is 16 bit code
- BIOS routines are standard and generic

### boot.asm
ORG -> Origin 0x7c00 so that the assembler knows the start of the code (assembly origin for offseting data)
BITS 16 -> assembler knows 16 bit opcodes to be produces and instructions accordingly
We move values into lower and upper part of eax register one byte each
We then call int 0x10 this intterupt is registered by the BIOS, here BIOS is acting as a mini kernel

ah - 0e - Command
int 0x10 - Write to Screen
al - 'A' - Character to write

https://www.ctyme.com/rbrown.htm - contains Documented and Non Documented Interrupts according to it
`Int 10/AH=0Eh - VIDEO - TELETYPE OUTPUT`

We can optionally set the page number argument in bx

We use assembly macro to fill the rest bytes till 512th byte with 0x00
Then we append the bootsignature in little endian

We also append a jmp to self instruction so that code does'nt go further and execute our null bytes and boot signature
We then compile it using `nasm -f bin boot.asm -o boot.bin`, we use bin because we want it in binary format and not ELF or ObjectFile , this is also a sector so running file command on it show that its a MBR boot sector
`ndisasm` to disassemble the binary file

We then create a function that will print any char and we use the argument to it as the si register
`lodsb` is an instruction which takes the byte at the address of `si` and copies it to the `al` register and icrements the `si` register, we use this to iterate over a string and print it.

### Real Mode
All modern intel processors when they are switched on are in this compatibility mode with 1MB of RAM accessible, No security, based on the original x86 design and 16bits accessible at one time 

### Segmentation Memory Model
- Memory is accessed by a segment and an offset
- Programs can be loaded in different areas of memory but run without any problems
- Multiple Segments are available through the use of segment registers

### 8086 Segment Registers
CS - Code Segment
SS - Stack Segment
DS - Data Segment
ES - Extra Segment

### Absolute Offsets
Calculating Absolute Offest = Segment Register * 16 + Offset (This is how we access 1MB of RAM)
Example Offsets For Absolute Address 0x7cff
 - Segment 0 offset 0x7cff
 - Segment 0x7c0 offset 0xff
 - Segment 0x7cf offset 0xf
Different Instructions use Different Segment Registers
`lodsb` uses DS:SI register combination

Setting ORG to 0x7c00 is bad because we don't know the value of the segment register
cli -> Clear Interrupts
sti -> Enables Interrupts

We clear interrupts before setting up segment registers so that a interrup does'nt occur in between as messes things up (Unclear)
Can't move value to ds,es only registers 
We set up ds to be 0x7c0 so that all memory accesses are now from base 0x7c0 * 16 = 0x7c00
and we set up ss to be 0 and sp to be 0x7c00 from here on stack grows backwards

jmp 0x7c0:start sets up `cs` to be 0x7c0 and jumps to the offset start from it

Certain BIOS require some more information in the sector of the bootloader (checkout os.dev.wiki)

### Interrup Vector Tables
Interrupts - are like subroutines,but you don't need to know their memory address to execute them
Called through interrupt numbers
Interrupts can be setup by as well to point to code at certain address

When you Invoke an Interrupt
- Processor is interrupts
- Old state is saved on stack
- interrupt is executed

- table describing 256 interrupt handlers
- each entry 5 bytes
- interrupts are in numerical order in table 

We register a int by moving 2 bytes 1 for segment and 1 for offset to the start of the RAM + 4 * intterupt number to register it

we can then call it using int 0x10 e.g

### Disk Access and How it Works
- Files on disk are Conecpts devised by OS
- Disk holds data blocks called sectors and have a data structure in it
- Kernel needs a fs driver to parse that data structure to create a file system
- Data is read and written in sectors of typically 512 bytes

int 13/ah=0x2 -> is the interrupt provided by BIOS to read a sector into memory

### Protected Mode
Protected Mode is like kernel mode it is a processor state in x86 architecture which gives access to memory protection , 4GB address space and much more
32 bit address space

ring0 - kernel
ring1 - device drivers
ring2 - device drivers
ring3 - userspace applications

Segment Registers are converted to Selector Registers
Memory is paged here

### Selector Memory Scheme
Selectors point to data structure that describe memory ranges and the permissions (ring level) required to access a given range

### Paging Memory Scheme
Memory is virtual and what you adddress can point to somewhere entirely different memory
paging is the most popular choice in kernel/OS


### Entering Protected Mode
We disable interrupts, load the global descriptor table and perform a jump to 32 bit code

GDT - Contains enteries telling the CPU about memory Segments
Loaded using LGDT assembly instruction - expects GDT description structure ( size, offset )

We create 3 gdt enteries
1st is NULL which should be like that
2nd is for code segments we set its range to be from 0 to 0xffff
3rd is for data segment we set the range to be same

then we create the struct of the gdt_descriptor ->
which is 
  size - 1
  address of start

we load this address using the lgdt instruction 
change cr0 register to go into protected mode and jump according to the new code segment offset to our 32 bit code
we change the cr0 register to enable the protect bit
Here according to out new data segment we set out data segment registers and also our stack 

### Enabling A20 Line
It is important to do this otherwise we won't be able to access the 21st bit number of any memory access.

### Cross-Compiler
Followed the instructions on os.dev.wiki to build a GCC-Cross-Compiler which we'll use to build the kernel, this gcc doesnt have the standard functions built into it and we'll have to create them ourselves

### Linker Script
We create a linker script `linker.ld` this is used for the linking process here we specify we want to load our kernel at address 1M or 0x100000

