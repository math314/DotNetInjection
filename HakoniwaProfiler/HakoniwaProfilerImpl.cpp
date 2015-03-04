#include "HakoniwaProfilerImpl.h"
#include "Debugger.h"
#include "ComUtil.h"
#include "FunctionInfo.h"

#include <string>
#include <wchar.h>
#include <corhlpr.h>
#include <memory>

#pragma comment (lib, "corguids.lib")

HakoniwaProfilerImpl::HakoniwaProfilerImpl() {
	mRefCount = 1;
}

HakoniwaProfilerImpl::~HakoniwaProfilerImpl() {
	SafeRelease(&mCorProfilerInfo2);
}

HRESULT HakoniwaProfilerImpl::SetProfilerEventMask() {
	// set the event mask 
	DWORD eventMask = 0;
	eventMask |= COR_PRF_MONITOR_NONE;
	eventMask |= COR_PRF_MONITOR_JIT_COMPILATION;
	eventMask |= COR_PRF_DISABLE_INLINING;
	eventMask |= COR_PRF_DISABLE_OPTIMIZATIONS;
	eventMask |= COR_PRF_USE_PROFILE_IMAGES;

	return mCorProfilerInfo2->SetEventMask(eventMask);
}


STDMETHODIMP HakoniwaProfilerImpl::QueryInterface(REFIID riid, void **ppObj) {
	*ppObj = nullptr;
	LPOLESTR clsid = nullptr;

	HRESULT hr = StringFromCLSID(riid, &clsid);
	if (SUCCEEDED(hr)) {
		Debugger::printf(L"QueryInterface(%s) called.", clsid);
		CoTaskMemFree(clsid);
	}
	if (riid == IID_IUnknown) {
		*ppObj = static_cast<IUnknown *>(static_cast<HakoniwaProfiler *>(this));
		AddRef();
		return S_OK;
	}

	if (riid == IID_HakoniwaProfiler) {
		*ppObj = static_cast<HakoniwaProfiler *>(this);
		AddRef();
		return S_OK;
	}

	if (riid == IID_ICorProfilerCallback) {
		*ppObj = static_cast<ICorProfilerCallback*>(this);
		AddRef();
		return S_OK;
	}

	if (riid == IID_ICorProfilerCallback2) {
		*ppObj = static_cast<ICorProfilerCallback2*>(this);
		AddRef();
		return S_OK;
	}

	if (riid == IID_ICorProfilerCallback3) {
		*ppObj = dynamic_cast<ICorProfilerCallback3*>(this);
		AddRef();
		return S_OK;
	}

	if (riid == IID_ICorProfilerInfo) {
		*ppObj = static_cast<ICorProfilerInfo *>(static_cast<ICorProfilerInfo2 *>(mCorProfilerInfo2));
		mCorProfilerInfo2->AddRef();
		return S_OK;
	}

	if (riid == IID_ICorProfilerInfo2) {
		*ppObj = mCorProfilerInfo2;
		mCorProfilerInfo2->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE HakoniwaProfilerImpl::AddRef() {
	return InterlockedIncrement(&mRefCount);
}

ULONG STDMETHODCALLTYPE HakoniwaProfilerImpl::Release() {
	long nRefCount = InterlockedDecrement(&mRefCount);
	if (nRefCount == 0) {
		delete this;
	}
	return nRefCount;
}

STDMETHODIMP HakoniwaProfilerImpl::Initialize(IUnknown *pICorProfilerInfoUnk) {
	Debugger::printf(L"HakoniwaProfilerImpl::Initialize()\n");

	HRESULT hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo2, (LPVOID*)&mCorProfilerInfo2);
	if (FAILED(hr)) {
		Debugger::printf(L"Error: Failed to get ICorProfiler2\n");
	} else {
		Debugger::printf(L"Got ICorProfilerInfo2\n");
	}

	hr = SetProfilerEventMask();
	if (FAILED(hr)) {
		Debugger::printf(L"Error: Failed to SetProfilerEventMask\n");
	} else {
		Debugger::printf(L"SetEventMask()\n");
	}

	Debugger::printf(L"Successfully initialized profiling\n");

	return S_OK;
}

const int MAX_LENGTH = 1024;

#include <memory>

void ReplaceTest(std::unique_ptr<FunctionInfo>& fi, const wchar_t* assemblyName, const wchar_t* fullyQualifiedClassName, const wchar_t* methodName){

}

STDMETHODIMP HakoniwaProfilerImpl::JITCompilationStarted(FunctionID functionID, BOOL fIsSafeToBlock) {
	std::unique_ptr<FunctionInfo> fi(FunctionInfo::CreateFunctionInfo(mCorProfilerInfo2, functionID));

	//output function information
	Debugger::printf(L"%s", fi->get_SignatureText().c_str());

	if (fi->get_ClassName() == L"System.Text.RegularExpressions.RegexReplacement" && fi->get_FunctionName() == L"Replace") {
		//get old method
		LPCBYTE oldMethodBytes;
		ULONG oldMethodSize;
		hrCheck(mCorProfilerInfo2->GetILFunctionBody(fi->get_ModuleID(), fi->get_Token(), &oldMethodBytes, &oldMethodSize));

		const COR_ILMETHOD_FAT* oldHeader = reinterpret_cast<const COR_ILMETHOD_FAT*>(oldMethodBytes);

		//allocate new method(=function) space
		IMethodMalloc* methodMalloc = NULL;
		hrCheck(mCorProfilerInfo2->GetILFunctionBodyAllocator(fi->get_ModuleID(), &methodMalloc));
		BYTE newMethod[] = {
			0x04,// ldarg.2
			0x2a // ret
		};
		ULONG newMethodSize = sizeof(COR_ILMETHOD_FAT) + sizeof(newMethod);
		void *allocated = methodMalloc->Alloc(newMethodSize);
		SafeRelease(&methodMalloc);

		//write new Header
		COR_ILMETHOD_FAT* newHeader = (COR_ILMETHOD_FAT *)allocated;
		memcpy(newHeader, oldHeader, sizeof(COR_ILMETHOD_FAT));
		newHeader->SetCodeSize(sizeof(newMethod)); // change code size only
		//write new IL
		memcpy((BYTE*)allocated + sizeof(COR_ILMETHOD_FAT), newMethod, sizeof(newMethod));

		//set new function
		hrCheck(mCorProfilerInfo2->SetILFunctionBody(fi->get_ModuleID(), fi->get_Token(), (LPCBYTE)allocated));
	}
	return S_OK;
}

