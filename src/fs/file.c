#include "file.h"
#include "../config.h"
#include "../kernel.h"
#include "../memory/memory.h"
#include "../status.h"
#include "../memory/heap/kheap.h"

struct filesystem* filesystems[SHARKYOS_MAX_FILESYSTEM];
struct file_descriptor *file_descriptors[SHARKYOS_MAX_FILE_DESCRIPTORS];

static struct filesystem** fs_get_free_filesystem() {
  for (int i = 0; i < SHARKYOS_MAX_FILESYSTEM;i++) {
    if (filesystems[i] == 0) {
      return &filesystems[i];
    }
  }
  return 0x0;
}

void fs_insert_filesystem(struct filesystem *filesystem) {
  struct filesystem **fs;
  // if (filesystem == 0) {
  //   panic();
  // }
  fs = fs_get_free_filesystem();
  if(!fs) {print("Problem Inserting filesystem");while(1){}}
  *fs = filesystem;
}

static void fs_static_load() {
  // fs_insert_filesystem(fat16_init());
}

void fs_load() {
  memset(filesystems,0,sizeof(filesystems));
  fs_static_load();
}

void fs_init() {
  memset(file_descriptors,0,sizeof(file_descriptors));
  fs_load();
}

static int file_new_descriptor(struct file_descriptor **desc_out) {
  int res = -ENOMEM;
  for (int i = 0 ;i < SHARKYOS_MAX_FILE_DESCRIPTORS;i++) {
    if (file_descriptors[i] == 0) {
      struct file_descriptor *desc = kzalloc(sizeof(struct file_descriptor));
      // Descriptor Start at 1
      desc->index = i + 1;
      file_descriptors[i] = desc;
      *desc_out = desc;
      res = 0;
      break;
    }
  }
  return res;
}
