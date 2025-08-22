#include <stdio.h>
#include <stdarg.h>

int xprintf(const char *fmt, ...)
{
  int res;
	va_list va;
	va_start(fmt,va);
	res = vs_printf(fmt,va);
	va_end(va);
	return res;
}

int main()
{
  printf("OK");
  return 0;
}
