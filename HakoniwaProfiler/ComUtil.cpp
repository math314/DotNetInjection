#include "ComUtil.h"
#include "Debugger.h"

void hrCheck(HRESULT hr){
	if (FAILED(hr)) {
		DebugPrintf(L"failed.(HRESULT = %08X)", hr);
		exit(-1);
	}
}
