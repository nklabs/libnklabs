#include <stdio.h>

main()
{
int y, x;
for (y = 0; y != 3; ++y)
 {
 for (x = 0; x != 128; ++x)
  {
  putchar('A' + y);
  }
 }
}
