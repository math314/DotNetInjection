#pragma once

#include <Unknwn.h>

class HakoniwaProfilerFactory : public IClassFactory {
public:
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	STDMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObject);
	STDMETHODIMP LockServer(BOOL fLock);

	static bool DllCanUnloadNow();

private:
	void LockServerCount(BOOL bLock);
};

