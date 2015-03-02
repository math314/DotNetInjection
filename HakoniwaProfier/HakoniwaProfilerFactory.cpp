#include "HakoniwaProfilerFactory.h"
#include "HakoniwaProfilerImpl.h"

STDMETHODIMP HakoniwaProfilerFactory::QueryInterface(REFIID riid, void **ppvObject) {
	*ppvObject = nullptr;

	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
		*ppvObject = static_cast<IClassFactory *>(this);
	else
		return E_NOINTERFACE;

	AddRef();
	return S_OK;
}

STDMETHODIMP_(ULONG) HakoniwaProfilerFactory::AddRef() {
	LockServerCount(TRUE);
	return 2;
}

STDMETHODIMP_(ULONG) HakoniwaProfilerFactory::Release() {
	LockServerCount(FALSE);
	return 1;
}

STDMETHODIMP HakoniwaProfilerFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObject) {
	*ppvObject = nullptr;
	if (pUnkOuter != nullptr)
		return CLASS_E_NOAGGREGATION;
	
	HakoniwaProfiler* p = new HakoniwaProfilerImpl();
	if (p == nullptr) return E_OUTOFMEMORY;

	auto hr = p->QueryInterface(riid, ppvObject);
	p->Release();

	return hr;
}

STDMETHODIMP HakoniwaProfilerFactory::LockServer(BOOL fLock) {
	LockServerCount(fLock);
	return S_OK;
}

static long g_lockServerCount = 0;

void HakoniwaProfilerFactory::LockServerCount(BOOL bLock) {
	if (bLock) {
		InterlockedIncrement(&g_lockServerCount);
	} else {
		InterlockedDecrement(&g_lockServerCount);
	}
}

bool HakoniwaProfilerFactory::DllCanUnloadNow() {
	return g_lockServerCount == 0 ? S_OK : S_FALSE;
}