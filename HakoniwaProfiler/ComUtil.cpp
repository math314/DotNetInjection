#include "ComUtil.h"
#include "Debugger.h"

void hrCheck(HRESULT hr){
	if (FAILED(hr)) {
		Debugger::printf(L"failed.(HRESULT = %08X)", hr);
		exit(-1);
	}
}
