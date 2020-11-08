#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(void)
{
  printf(1, "Hello, world!\n");
  printf(1, "%d\n", getNumFreePages());
  printf(1, "%d\n", getNumPhysPages());
  printf(1, "%d\n", getNumVirtPages());
  printf(1, "%d\n", getNumPTPages());
  exit();
}
