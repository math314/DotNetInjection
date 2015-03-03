#include "HakoniwaProfilerImpl.h"
#include "Debugger.h"
#include "ComUtil.h"

#include <string>
#include <wchar.h>
#include <corhlpr.h>
#include <memory>

#pragma comment (lib, "corguids.lib")

HakoniwaProfilerImpl::HakoniwaProfilerImpl() {
	mRefCount = 1;
	InitializeCriticalSection(&mCriticalSection);
}

HakoniwaProfilerImpl::~HakoniwaProfilerImpl() {
	SafeRelease(&mCorProfilerInfo2);
	DeleteCriticalSection(&mCriticalSection);
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

STDMETHODIMP HakoniwaProfilerImpl::ProfilerAttachComplete(void) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ProfilerDetachSucceeded(void) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::InitializeForAttach(IUnknown *pCorProfilerInfoUnk, void *pvClientData, UINT cbClientData) { return S_OK; }

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

STDMETHODIMP HakoniwaProfilerImpl::ThreadAssignedToOSThread(ThreadID managedThreadID, DWORD osThreadID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::Shutdown() { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::AppDomainCreationStarted(AppDomainID appDomainID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::AppDomainCreationFinished(AppDomainID appDomainID, HRESULT hrStatus) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::AppDomainShutdownStarted(AppDomainID appDomainID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::AppDomainShutdownFinished(AppDomainID appDomainID, HRESULT hrStatus) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::AssemblyLoadStarted(AssemblyID assemblyID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::AssemblyLoadFinished(AssemblyID assemblyID, HRESULT hrStatus) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::AssemblyUnloadStarted(AssemblyID assemblyID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::AssemblyUnloadFinished(AssemblyID assemblyID, HRESULT hrStatus) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ModuleLoadStarted(ModuleID moduleID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ModuleLoadFinished(ModuleID moduleID, HRESULT hrStatus) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ModuleUnloadStarted(ModuleID moduleID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ModuleUnloadFinished(ModuleID moduleID, HRESULT hrStatus) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ModuleAttachedToAssembly(ModuleID moduleID, AssemblyID assemblyID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ClassLoadStarted(ClassID classID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ClassLoadFinished(ClassID classID, HRESULT hrStatus) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ClassUnloadStarted(ClassID classID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ClassUnloadFinished(ClassID classID, HRESULT hrStatus) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::FunctionUnloadStarted(FunctionID functionID) { return S_OK; }

const int MAX_LENGTH = 1024;

STDMETHODIMP HakoniwaProfilerImpl::JITCompilationStarted(FunctionID functionID, BOOL fIsSafeToBlock) {
	EnterCriticalSection(&mCriticalSection);

	ClassID classID;
	ModuleID moduleID;
	mdToken token;
	mCorProfilerInfo2->GetFunctionInfo(functionID, &classID, &moduleID, &token);

	//get function name
	IMetaDataImport* metaDataImport = NULL;
	mdToken functionToken = NULL;
	mCorProfilerInfo2->GetTokenAndMetaDataFromFunction(functionID, IID_IMetaDataImport, (LPUNKNOWN *)&metaDataImport, &functionToken);

	mdTypeDef classTypeDef;
	WCHAR functionName[MAX_LENGTH];
	WCHAR className[MAX_LENGTH];
	PCCOR_SIGNATURE signatureBlob;
	ULONG signatureBlobLength;
	DWORD methodAttributes = 0;
	metaDataImport->GetMethodProps(functionToken, &classTypeDef, functionName, MAX_LENGTH, 0, &methodAttributes, &signatureBlob, &signatureBlobLength, NULL, NULL);
	metaDataImport->GetTypeDefProps(classTypeDef, className, MAX_LENGTH, 0, NULL, NULL);
	SafeRelease(&metaDataImport);

	//output function name
	if (lstrcmp(L"Replace", functionName) == 0) {
		Debugger::printf(L"JIT Start : %s.%s (functionID = %p)", className, functionName, functionID);
	}
	if (lstrcmp(L"System.Text.RegularExpressions.RegexReplacement", className) == 0 && lstrcmp(L"Replace", functionName) == 0) {
		//get old method
		LPCBYTE oldMethodBytes;
		ULONG oldMethodSize;
		hrCheck(mCorProfilerInfo2->GetILFunctionBody(moduleID, token, &oldMethodBytes, &oldMethodSize));

		const COR_ILMETHOD_FAT* oldHeader = reinterpret_cast<const COR_ILMETHOD_FAT*>(oldMethodBytes);
		Debugger::printf(L"IsFat : %d", oldHeader->IsFat());

		//allocate new method(=function) space
		IMethodMalloc* methodMalloc = NULL;
		hrCheck(mCorProfilerInfo2->GetILFunctionBodyAllocator(moduleID, &methodMalloc));
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
		hrCheck(mCorProfilerInfo2->SetILFunctionBody(moduleID, token, (LPCBYTE)allocated));
	}
	LeaveCriticalSection(&mCriticalSection);
	return S_OK;
}

STDMETHODIMP HakoniwaProfilerImpl::JITCompilationFinished(FunctionID functionID, HRESULT hrStatus, BOOL fIsSafeToBlock) {
	// Debugger::printf(L"JITCompilationFinished(funtionID = %p)",functionID);
	return S_OK;
}

STDMETHODIMP HakoniwaProfilerImpl::JITCachedFunctionSearchStarted(FunctionID functionID, BOOL *pbUseCachedFunction) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::JITCachedFunctionSearchFinished(FunctionID functionID, COR_PRF_JIT_CACHE result) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::JITFunctionPitched(FunctionID functionID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::JITInlining(FunctionID callerID, FunctionID calleeID, BOOL *pfShouldInline) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::UnmanagedToManagedTransition(FunctionID functionID, COR_PRF_TRANSITION_REASON reason) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ManagedToUnmanagedTransition(FunctionID functionID, COR_PRF_TRANSITION_REASON reason) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ThreadCreated(ThreadID threadID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ThreadDestroyed(ThreadID threadID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::RemotingClientInvocationStarted() { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::RemotingClientSendingMessage(GUID *pCookie, BOOL fIsAsync) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::RemotingClientReceivingReply(GUID *pCookie, BOOL fIsAsync) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::RemotingClientInvocationFinished() { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::RemotingServerReceivingMessage(GUID *pCookie, BOOL fIsAsync) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::RemotingServerInvocationStarted() { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::RemotingServerInvocationReturned() { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::RemotingServerSendingReply(GUID *pCookie, BOOL fIsAsync) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::RuntimeSuspendStarted(COR_PRF_SUSPEND_REASON suspendReason) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::RuntimeSuspendFinished() { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::RuntimeSuspendAborted() { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::RuntimeResumeStarted() { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::RuntimeResumeFinished() { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::RuntimeThreadSuspended(ThreadID threadID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::RuntimeThreadResumed(ThreadID threadID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::MovedReferences(ULONG cmovedObjectIDRanges, ObjectID oldObjectIDRangeStart[], ObjectID newObjectIDRangeStart[], ULONG cObjectIDRangeLength[]) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ObjectAllocated(ObjectID objectID, ClassID classID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ObjectsAllocatedByClass(ULONG classCount, ClassID classIDs[], ULONG objects[]) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ObjectReferences(ObjectID objectID, ClassID classID, ULONG objectRefs, ObjectID objectRefIDs[]) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::RootReferences(ULONG rootRefs, ObjectID rootRefIDs[]) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ExceptionThrown(ObjectID thrownObjectID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ExceptionUnwindFunctionEnter(FunctionID functionID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ExceptionUnwindFunctionLeave() { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ExceptionSearchFunctionEnter(FunctionID functionID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ExceptionSearchFunctionLeave() { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ExceptionSearchFilterEnter(FunctionID functionID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ExceptionSearchFilterLeave() { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ExceptionSearchCatcherFound(FunctionID functionID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ExceptionCLRCatcherFound() { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ExceptionCLRCatcherExecute() { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ExceptionOSHandlerEnter(FunctionID functionID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ExceptionOSHandlerLeave(FunctionID functionID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ExceptionUnwindFinallyEnter(FunctionID functionID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ExceptionUnwindFinallyLeave() { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ExceptionCatcherEnter(FunctionID functionID, ObjectID objectID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ExceptionCatcherLeave() { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::COMClassicVTableCreated(ClassID wrappedClassID, REFGUID implementedIID, void *pVTable, ULONG cSlots) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::COMClassicVTableDestroyed(ClassID wrappedClassID, REFGUID implementedIID, void *pVTable) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::ThreadNameChanged(ThreadID threadID, ULONG cchName, WCHAR name[]) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::GarbageCollectionStarted(int cGenerations, BOOL generationCollected[], COR_PRF_GC_REASON reason) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::SurvivingReferences(ULONG cSurvivingObjectIDRanges, ObjectID objectIDRangeStart[], ULONG cObjectIDRangeLength[]) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::GarbageCollectionFinished() { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::FinalizeableObjectQueued(DWORD finalizerFlags, ObjectID objectID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::RootReferences2(ULONG cRootRefs, ObjectID rootRefIDs[], COR_PRF_GC_ROOT_KIND rootKinds[], COR_PRF_GC_ROOT_FLAGS rootFlags[], UINT_PTR rootIDs[]) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::HandleCreated(GCHandleID handleID, ObjectID initialObjectID) { return S_OK; }
STDMETHODIMP HakoniwaProfilerImpl::HandleDestroyed(GCHandleID handleID) { return S_OK; }
