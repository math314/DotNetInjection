#include "HakoniwaProfilerImpl.h"
#include "Debugger.h"
#include "ComUtil.h"
#include "FunctionInfo.h"
#include "Tranpoline.h"

#include <string>
#include <wchar.h>
#include <corhlpr.h>
#include <memory>

#pragma comment (lib, "corguids.lib")

HakoniwaProfilerImpl::HakoniwaProfilerImpl() {
	mRefCount = 1;
}

HakoniwaProfilerImpl::~HakoniwaProfilerImpl() {
}

HRESULT HakoniwaProfilerImpl::SetProfilerEventMask() {
	DWORD eventMask = 0;
	eventMask |= COR_PRF_MONITOR_JIT_COMPILATION;
	eventMask |= COR_PRF_DISABLE_OPTIMIZATIONS;
	eventMask |= COR_PRF_USE_PROFILE_IMAGES;

	return mCorProfilerInfo2->SetEventMask(eventMask);
}

STDMETHODIMP HakoniwaProfilerImpl::QueryInterface(REFIID riid, void **ppObj) {
	*ppObj = nullptr;
	
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

	if (riid == IID_ICorProfilerInfo || riid == IID_ICorProfilerInfo2) {
		mCorProfilerInfo2.CopyTo(riid, ppObj);
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

	ICorProfilerInfo2* iInfo2;
	HRESULT hr = pICorProfilerInfoUnk->QueryInterface(__uuidof(iInfo2), (LPVOID*)&iInfo2);
	if (FAILED(hr)) {
		DebugPrintf(L"Error: Failed to get ICorProfiler2\n");
		exit(-1);
	} 
	mCorProfilerInfo2.Attach(iInfo2);

	hr = SetProfilerEventMask();
	if (FAILED(hr)) {
		DebugPrintf(L"Error: Failed in SetProfilerEventMask\n");
		exit(-1);
	}
	DebugPrintf(L"Successfully initialized profiling\n");

	return S_OK;
}

#include <memory>

STDMETHODIMP HakoniwaProfilerImpl::JITCompilationStarted(FunctionID functionID, BOOL fIsSafeToBlock) {
	std::shared_ptr<FunctionInfo> fi(FunctionInfo::CreateFunctionInfo(mCorProfilerInfo2.Get(), functionID));

	//クラス名，メソッド名が一致する物を置き換える
	if (fi->get_ClassName() == L"System.DateTime" && fi->get_FunctionName() == L"get_Now") {
		DebugPrintf(L"%s", fi->get_SignatureText().c_str());
		Tranpoline tranpoline(mCorProfilerInfo2, fi);
		tranpoline.Update(L"HakoniwaProfiler.MethodHook.MethodHook", L"get_Now");
	}

	if (fi->get_ClassName() == L"System.Text.RegularExpressions.Regex" && fi->get_FunctionName() == L"Replace") {
		//overloadされているメソッドは，引数を確認して置き換え対象か判断する
		if (fi->get_ArgumentCount() == 3 && fi->get_Arguments()[0] == L"string" && fi->get_Arguments()[1] == L"string" && fi->get_Arguments()[2] == L"string") {
			// staticなメソッドの置き換え
			DebugPrintf(L"%s", fi->get_SignatureText().c_str());
			Tranpoline tranpoline(mCorProfilerInfo2, fi);
			tranpoline.Update(L"HakoniwaProfiler.MethodHook.MethodHook", L"Replace");
		} else if (fi->get_ArgumentCount() == 2 && fi->get_Arguments()[0] == L"string" && fi->get_Arguments()[1] == L"string") {
			// 非staticなメソッドの置き換え
			DebugPrintf(L"%s", fi->get_SignatureText().c_str());
			Tranpoline tranpoline(mCorProfilerInfo2, fi);
			tranpoline.Update(L"HakoniwaProfiler.MethodHook.MethodHook", L"Replace");
		}
	}

	if (fi->get_ClassName() == L"ConsoleAppTest.Program" && fi->get_FunctionName() == L"getStr1") {
		//テストプログラムのメソッドを置き換える
		DebugPrintf(L"%s", fi->get_SignatureText().c_str());
		Tranpoline tranpoline(mCorProfilerInfo2, fi);
		tranpoline.Update(L"HakoniwaProfiler.MethodHook.MethodHook", L"getStr1");
	}

	if (fi->get_ClassName() == L"ConsoleAppTest.Program" && fi->get_FunctionName() == L"haveArguments") {
		DebugPrintf(L"%s", fi->get_SignatureText().c_str());
		Tranpoline tranpoline(mCorProfilerInfo2, fi);
		tranpoline.Update(L"HakoniwaProfiler.MethodHook.MethodHook", L"haveArguments");
	}

	if(fi->get_ClassName() == L"ConsoleAppTest.Program" && fi->get_FunctionName() == L"haveManyArguments") {
		DebugPrintf(L"%s", fi->get_SignatureText().c_str());
		Tranpoline tranpoline(mCorProfilerInfo2, fi);
		tranpoline.Update(L"HakoniwaProfiler.MethodHook.MethodHook", L"haveManyArguments");
	}

	if (fi->get_ClassName() == L"ConsoleAppTest.TestClass" && fi->get_FunctionName() == L"test1") {
		// staticなメソッドの置き換え
		DebugPrintf(L"%s", fi->get_SignatureText().c_str());
		Tranpoline tranpoline(mCorProfilerInfo2, fi);
		tranpoline.Update(L"HakoniwaProfiler.MethodHook.MethodHook", L"test1");
	}

	if (fi->get_ClassName() == L"ConsoleAppTest.TestClass" && fi->get_FunctionName() == L"test2") {
		// 非staticなメソッドの置き換え
		DebugPrintf(L"%s", fi->get_SignatureText().c_str());
		Tranpoline tranpoline(mCorProfilerInfo2, fi);
		tranpoline.Update(L"HakoniwaProfiler.MethodHook.MethodHook", L"test2");
	}

	return S_OK;
}

