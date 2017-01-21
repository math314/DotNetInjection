#include "Debugger.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <Windows.h>

void DebugPrintf(const wchar_t *format, ...){
	static wchar_t buffer[10000];
	
	va_list arg;

	va_start(arg, format);
	vswprintf_s(buffer, format, arg);
	va_end(arg);

	OutputDebugString(buffer);
}
