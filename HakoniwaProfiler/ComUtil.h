#pragma once

#include <Windows.h>

void hrCheck(HRESULT hr);
template <class T> void SafeRelease(T **ppT)
{
	if (*ppT) {
		(*ppT)->Release();
		*ppT = nullptr;
	}
}

