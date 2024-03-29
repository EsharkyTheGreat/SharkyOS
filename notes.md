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

### Alignment Issue
Since our kernel.asm code is first in the binary and C functions require stack to be aligned we align our kernel asm to 512 bytes 

### Text Mode
It allows you to write ASCII directly to video memory instead of setting pixels for coloured displays start at address 0xb8000 it supports 16 different colours for monovhrom displays 0xB0000 ascii -> pixel conversion is done automatically

References a font table for conversion -> Each Character 2 bytes, byte 0 -> ASCII and byte 1 -> Colour Code

### Print Hello World
We Use the text mode to set all characters to space and create a functiont that iterates over the rows and columns of the screen and prints out our desired character

### Interrupt Descriptor Table
- Describes how interrupts are invoked in protected mode
- Can be mapped anywhere in memory
- Different from the interrupt vector table
- Code pointers to different `int` like `int 80`

__attribute__((packed)) aligns struct

### Registering our Interrupt
We create the array of interrupts set an interrupt in it to out function create a descriptor of it like we did before with address and size, then load it with `lidt` instruction again
we registered int 0 which can be called by division by zero error by the processor or by the assembly instruction int 0

### Programmable Interrupt Controller
- Allows Hardware to Interrupt the processor state
- Programmable
- Requires interrupt acknowledgment

Hardware interrupts the processor (harddisk,keyboard)
Standard ISA IRQs table (mouse,keyboard, timer interrupt, disk etc)
irq = baseinterrupt + num
by default it is 8-15 but these are reserved in protected mode for exceptions so we are required to remap the PIC(Programmable Interrupt Controller)
System has two PIC one for master ports and other for slave ports
master -0 to 7
slave - 8 to 15
PIC control ports
0x20 and 0x21 are master IRQs
0xA0 and 0xA1 are slave IRQs

### Heap
Giant memory region which can be shared in a controllable manner
can ask and tell the heap for need of memory and when ur done
heap implementations are essetial system memory managers

- Video memory takes up portions of RAM
- Hardware memory takes up portion of RAM
- Unused part for us
## What is Paging ?
- Paging allows us to remamp memory addresses to point to other memory addresses
- Can be used to provide the illusion we have the maximum amount of RAM installed
- Can be used to hide memory from other processes
### Remapping Memory
- Paging allows us to remap one memory address to another, so 0x100000 could point to 0x200000
- Paging works in 4096 byte blocks by default. These blocks are called pages.
- When pagin is enabled the MMU (Memory Management Unit) will look at your allocated page table to resolve virtual addresses into physical addresses.
- Paging allows us to pretend memory exists when it does not.
### Virtual Addresses vs Physical Addresses
- Virtual addresses that are not pointing to the address in memory that their value says they are.
- Physical addresses are absolute addresses in memory whose value points to the same address in memory.
### SharkyOS Paging Structure
- 1024 page directories which point to 1024 page tables
- 1024 page table entries per page tables
- Each Page table entry covers 4096 bytes of memory
- Each "4096" bytes of memory is called a page
- 1024 * 1024 * 4096 = 4,294,967,296 Bytes / 4GB of addressable memory
- 32 bit architecture

### What is a PCI IDE Controller ?
IDE refers to the electrical specifiaction of cables which connect ATA drives to another device
IDE allows up to 4 drives to be connected

- ATA   (serial)
- ATA   (parallel)
- ATAPI (Serial)
- ATAPI (Parallel)

Don't understand this yet

### File System
- A file system is a structure that describes how information is layed out in the disk
- Disks are not aware of files
- The operating system knows the filesystem structures so knows how to read files from the disk

### Disks
- Hard disks can be thought of as just a gian array of information split into sectors (usually 512 bytes)
- Each sector can be read into memory and is given a LBA(Logical Block Address) number
- Files do not exists on the disk
- Disks have no concepts of files

### Filesystem Structure
- Containes raw data for files
- Contains the filesystem structure header which can explain things such as how many files are on the disk, where the root directory is located and so on..
- The way files are laid out on disk is different depending on the filesystem you are using e.g "FAT16", FAT32, NTFS and many more
- Without file system we would be forced to read and write data through the use of sector numbers, structure would not exsist and corruption would likely occur

### FAT16 (File Allocation Table) 16 Bits
- The first sector in the filesystem format is the bootsector on the disk. Fields also exist in this first sector that describe the filesystem such as how many reserved sectros follow this sector
- Then follows the reserved sectors thesea re sectors ignored by the filesystem. There is a field  in the boot sector that specifies how many reserved sectors that are. (Operating System has to take care of this)
- Now we have our first file allocation table, this table contains values that represent which clusters on the disk are taken and whicha re free.(A cluster is just a certain number of sectors joined together to represent one cluster)
- Next comes our second file allocation table it's optional though and depends on the FAT16 header in the boot sector
- Now comes our root directory this explains what files/directories are in the root directory of the filesystem. Each entry has a relative name that represents the file or directory name, attributes such as read only, the address of the first cluster representing the data on the disk. And more..
- Finally we have our data region, all the data is here

### File Allocation Table
- The file allocation table is a system developed by microsoft
- it consists of a series of clusters that hold data and a table that determines the state of a cluster
- the boot sector stores information about the filesystem

### FAT 16 File System
- Uses clusters to represent data and subdirectories
- Each Cluster uses a fixed amount of sectors which is specified in the boot sector
- Every file in FAT16 needs to use atlease one cluster for its data this means a lot of sotrage is wasterd for small files
- FAT16 cannot store files larger than 2GB without large file support. With large file support 4GB is the maximum.

jmp short then nop instruction at the first
OEMIdentifier 8bytes (Name)

### VFS Layer
- The virtual filesystem layer allows a kernel to support an infinite amount of filesystem
- They virtual filesystem layer allows us to abstract out low level filesystem code
- Allows filesystem functionality to be loaded or unloaded to the kernel at any time
