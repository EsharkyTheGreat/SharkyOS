#include "kernel.h"
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"
#include "fs/pparser.h"
#include "string/string.h"

#include <stdint.h>
#include <stddef.h>

uint16_t *vd_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

extern void enable_interrupts();

uint16_t terminal_make_char(char c, char colour)
{
  return (colour << 8) | c;
}

void terminal_putchar(int x, int y, char c, char colour){
  vd_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c,colour);
}

void terminal_writechar(char c, char colour){
  if (c == '\n'){
    terminal_row +=1;
    terminal_col = 0;
    return;
  }
  terminal_putchar(terminal_col,terminal_row,c,colour);
  terminal_col += 1;
  if (terminal_col >= VGA_WIDTH){
    terminal_col = 0;
    terminal_row += 1;
  }
}

void terminal_initialize()
{
  vd_mem = (uint16_t *)(0xB8000);
  terminal_row = 0;
  terminal_col = 0;
  for (int y = 0; y < VGA_HEIGHT; y++) {
    for (int x = 0; x < VGA_WIDTH; x++) {
      terminal_putchar(x,y,' ',0);
    }
  }
}

void print(const char *str)
{
  size_t len = strlen(str);
  for (int i = 0; i < len; i++) {
    terminal_writechar(str[i],15);
  }
}
static struct paging_4gb_chunk *kernel_chunk = 0;
void kernel_main()
{
  terminal_initialize();
  print("Hello World!\nWelcome to Sharky OS\n");
  kheap_init();

  // Search and Initialize the Disks
  disk_search_and_init();

  // Initialize the interrupt descriptor table
  idt_init();
  //Setup Paging
  kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
  // Switch to Kernel Paging Chunk
  paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));
  // Enale Paging
  enable_paging();

  // Enable the system Interrupts
  enable_interrupts();

  struct path_root *root_path = pathparser_parse("0:/bin/shell.exe", NULL);
  if (root_path) {
     
  }
}
