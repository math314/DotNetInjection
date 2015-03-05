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

mdMemberRef DefineInjectionMethod(ICorProfilerInfo2* info, FunctionInfo* fi, const wchar_t* assemblyName, std::vector<BYTE>& publicToken, const wchar_t* fullyQualifiedClassName, const wchar_t* methodName) {
	//query interface
	IMetaDataEmit* metaDataEmit = NULL;
	hrCheck(info->GetModuleMetaData(fi->get_ModuleID(), ofRead | ofWrite, IID_IMetaDataEmit, (IUnknown**)&metaDataEmit));
	IMetaDataAssemblyEmit* metaDataAssemblyEmit = NULL;
	hrCheck(metaDataEmit->QueryInterface(IID_IMetaDataAssemblyEmit, (void**)&metaDataAssemblyEmit));

	mdAssemblyRef assemblyRef = mdAssemblyRefNil;
	ASSEMBLYMETADATA assemblyMetaData = { 0 };
	assemblyMetaData.usMajorVersion = 1; //assembly version is 1.0.0.0
	hrCheck(metaDataAssemblyEmit->DefineAssemblyRef(&(publicToken[0]), publicToken.size(), assemblyName, &assemblyMetaData, NULL, 0, 0, &assemblyRef));

	mdTypeRef typeRef = mdTypeRefNil;
	hrCheck(metaDataEmit->DefineTypeRefByName(assemblyRef, fullyQualifiedClassName, &typeRef));

	std::vector<BYTE> defineSignatureBlob = fi->get_SignatureBlob();
	Debugger::printf(L"buffer size = %d",defineSignatureBlob.size());
	defineSignatureBlob[0] = IMAGE_CEE_CS_CALLCONV_DEFAULT;

	if (IsMdStatic(fi->get_MethodAttributes()) == 0) {
		//todo : non static -> static
		// signatureBlob extention(insert arg0 = class)
		Debugger::printf(L"ReplaceTest : no static method replacement : %08X",fi->get_MethodAttributes());
		exit(-1);
	}

	mdMemberRef memberRef = mdMemberRefNil;
	hrCheck(metaDataEmit->DefineMemberRef(typeRef, methodName, &(defineSignatureBlob[0]), defineSignatureBlob.size(), &memberRef));

	SafeRelease(&metaDataEmit);
	SafeRelease(&metaDataAssemblyEmit);

	return memberRef;
}

BYTE calcNewMethodStackSize(FunctionInfo* fi) {
	ULONG newArguments = fi->getArgumentCount();
	if (!IsMdStatic(fi->get_MethodAttributes())) {
		newArguments++; //push caller object
	}

	if (newArguments > 0xFF) {
		Debugger::printf(L"arguments too many");
		exit(-1);
	}

	return (BYTE)newArguments;
}

std::vector<BYTE> ConstructTranpolineMethodIL(ICorProfilerInfo2* info, FunctionInfo* fi,mdMemberRef mdCallFunctionRef) {
	std::vector<BYTE> newILs;
	ULONG newArguments = calcNewMethodStackSize(fi);

	// ldarg.0, ldarg.1 ... ldarg. newArguments - 1
	for (BYTE i = 0; i < (BYTE)newArguments; i++) {
		if (i < 4) {
			newILs.push_back(0x02 + i); //ldarg.0 ~ ldarg.3
		} else {
			newILs.push_back(0x0E);
			newILs.push_back(i); //ldarg.s <index>
		}
	}

	// method call
	newILs.push_back(0x28);
	newILs.push_back((mdCallFunctionRef >> 0) & 0xFF);
	newILs.push_back((mdCallFunctionRef >> 8) & 0xFF);
	newILs.push_back((mdCallFunctionRef >> 16) & 0xFF);
	newILs.push_back((mdCallFunctionRef >> 24) & 0xFF);

	// ret
	newILs.push_back(0x2a);

	return newILs;
}

COR_ILMETHOD_FAT ConstructTranpolineMethodBody(ICorProfilerInfo2* info, FunctionInfo* fi, DWORD codeSize) {
	COR_ILMETHOD_FAT* oldHeader;
	ULONG size;
	info->GetILFunctionBody(fi->get_ModuleID(), fi->get_Token(), (LPCBYTE *)&oldHeader, &size);

	if (!oldHeader->IsFat()) {
		Debugger::printf(L"not fat");
		exit(-1);
	}
	// dump(oldHeader, size);

	COR_ILMETHOD_FAT fatHeader;
	memcpy(&fatHeader, oldHeader, sizeof(COR_ILMETHOD));
	fatHeader.SetCodeSize(codeSize);

	return fatHeader;
}

void* AllocateFuctionBody(ICorProfilerInfo2* info, FunctionInfo* fi, DWORD size) {
	IMethodMalloc* methodMalloc = nullptr;
	hrCheck(info->GetILFunctionBodyAllocator(fi->get_ModuleID(), &methodMalloc));
	void *allocated = methodMalloc->Alloc(size);
	SafeRelease(&methodMalloc);
	return allocated;
}

void ReplaceTest(ICorProfilerInfo2* info, FunctionInfo* fi, const wchar_t* className, const wchar_t* methodName) {
	const WCHAR moduleName[] = L"HakoniwaProfiler.MethodHook";
	const BYTE publicToken[] = { 0x41, 0x91, 0x82, 0x76, 0xff, 0x21, 0x51, 0x80 };
	std::vector<BYTE> _publicToken(publicToken, publicToken + sizeof(publicToken));
	mdMemberRef newMemberRef = DefineInjectionMethod(info, fi, moduleName, _publicToken, className, methodName);

	std::vector<BYTE> newILs = ConstructTranpolineMethodIL(info, fi, newMemberRef);
	COR_ILMETHOD_FAT newHeader = ConstructTranpolineMethodBody(info, fi, newILs.size());

	ULONG newMethodSize = sizeof(COR_ILMETHOD_FAT)+newILs.size();
	void *allocated = AllocateFuctionBody(info, fi, newMethodSize);

	//write new Header
	memcpy(allocated, &newHeader, sizeof(COR_ILMETHOD));
	//write new IL
	memcpy((BYTE*)allocated + sizeof(COR_ILMETHOD_FAT), &newILs[0], newILs.size());

	//set new function
	hrCheck(info->SetILFunctionBody(fi->get_ModuleID(), fi->get_Token(), (LPCBYTE)allocated));

}

STDMETHODIMP HakoniwaProfilerImpl::JITCompilationStarted(FunctionID functionID, BOOL fIsSafeToBlock) {
	FunctionInfo* fi(FunctionInfo::CreateFunctionInfo(mCorProfilerInfo2, functionID));

	//output function information
	// Debugger::printf(L"%s", fi->get_SignatureText().c_str());

	//if (fi->get_ClassName() == L"System.Text.RegularExpressions.RegexReplacement" && fi->get_FunctionName() == L"Replace"
	//	&& (IsMdStatic(fi->get_MethodAttributes()) != 0)
	//	){
	//	Debugger::printf(L"%s", fi->get_SignatureText().c_str());
	//	ReplaceTest(mCorProfilerInfo2, fi, L"HakoniwaProfiler.MethodHook.RegexReplacement", L"Replace");
	//}

	if (fi->get_ClassName() == L"System.DateTime" && fi->get_FunctionName() == L"get_Now") {
		Debugger::printf(L"%s", fi->get_SignatureText().c_str());
		ReplaceTest(mCorProfilerInfo2, fi, L"HakoniwaProfiler.MethodHook.RegexReplacement", L"get_Now");
	}

	if (fi->get_ClassName() == L"ConsoleAppTest.Program" && fi->get_FunctionName() == L"getStr1") {
		Debugger::printf(L"%s", fi->get_SignatureText().c_str());
		ReplaceTest(mCorProfilerInfo2, fi, L"HakoniwaProfiler.MethodHook.RegexReplacement", L"getStr1");
	}

	delete fi;

	return S_OK;
}

