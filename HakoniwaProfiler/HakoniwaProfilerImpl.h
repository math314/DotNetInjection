#pragma once

#include "HakoniwaProfilerBase.h"
#include <wrl/client.h>

class HakoniwaProfilerImpl :
	public HakoniwaProfilerBase
{
public:
	HakoniwaProfilerImpl();
	~HakoniwaProfilerImpl();

	STDMETHOD(QueryInterface)(REFIID riid, void **ppObj);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();
	STDMETHOD(Initialize)(IUnknown *pICorProfilerInfoUnk);


	STDMETHOD(JITCompilationStarted)(FunctionID functionID, BOOL fIsSafeToBlock);

private:
	HRESULT SetProfilerEventMask();

private:
	Microsoft::WRL::ComPtr<ICorProfilerInfo2> mCorProfilerInfo2;

	long mRefCount;
};