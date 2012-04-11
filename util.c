#include <stdarg.h>
#include "imgui.h"

void __noreturn die(int code, char const * p, ...) {
	va_list vl;
	va_start(vl, p);
	vfprintf(stderr, p, vl);
	fputc('\n', stderr);
	va_end(vl);

	fflush(stderr);
	exit(code);
}
