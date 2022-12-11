#include "idt.h"
#include "../config.h"
#include "../memory/memory.h"
#include "../kernel.h"
#include <stdint.h>

struct idt_desc idt_descriptors[SHARKYOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void idt_load(struct idtr_desc *ptr);

void idt_zero()
{
  print("Divide by zero Error\n");
}

void idt_set(int interrupt_no, void *address) {
  struct idt_desc* desc = &idt_descriptors[interrupt_no];
  desc->offset_1 = (uint32_t) address & 0x0000ffff;
  desc->selector = KERNEL_CODE_SELECTOR;
  desc->zero = 0x0;
  desc->type_attr = 0xee;
  desc->offset_2 = (uint32_t) address >> 16;
}

void idt_init() {
  memset(idt_descriptors,0,sizeof(idt_descriptors));
  idtr_descriptor.limit = sizeof(idt_descriptors) - 1;
  idtr_descriptor.base = (uint32_t)idt_descriptors;

  idt_set(0,idt_zero);

  // Load the Interrupt Descriptor Table
  idt_load(&idtr_descriptor);
}
