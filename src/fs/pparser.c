#include "pparser.h"
#include "../config.h"
#include "../string/string.h"
#include "../memory/memory.h"
#include "../status.h"

static int pathparser_path_valid_format(const char *filename) {
  int len = strnlen(filename,SHARKYOS_MAX_PATH);
  return (len >=3 && isdigit(filename[0]) && memcmp((void *)&filename[1],":/",2));
}

static int pathparser_get_drive(const char **path) {
  if (!pathparser_path_valid_format(*path)) {
    return -EBADPATH;
  } 
  int drive_no = tonumericdigit(*path[0]);

  // Add 3 bytes to skip drive number 0:/ 1:/ 2:/
  *path+=3;
  return drive_no;
}
