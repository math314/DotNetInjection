#include "HakoniwaProfilerImpl.h"

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
	if (mCorProfilerInfo != nullptr) {
		mCorProfilerInfo->Release();
		mCorProfilerInfo = nullptr;
	}
	if (mCorProfilerInfo2 != nullptr) {
		mCorProfilerInfo2->Release();
		mCorProfilerInfo2 = nullptr;
	}
	DeleteCriticalSection(&mCriticalSection);
}

HRESULT HakoniwaProfilerImpl::SetProfilerEventMask() {
	// set the event mask 
	DWORD eventMask = 0;
	eventMask |= COR_PRF_MONITOR_NONE;
	eventMask |= COR_PRF_MONITOR_JIT_COMPILATION;
	eventMask |= COR_PRF_DISABLE_INLINING;
	eventMask |= COR_PRF_DISABLE_OPTIMIZATIONS;

	return mCorProfilerInfo->SetEventMask(eventMask);
}


STDMETHODIMP HakoniwaProfilerImpl::QueryInterface(REFIID riid, void **ppObj) {
	*ppObj = nullptr;
	LPOLESTR clsid = nullptr;

	HRESULT hr = StringFromCLSID(riid, &clsid);
	if (SUCCEEDED(hr)) {
		std::wstring clsidString(clsid);
		// g_debugLogger << "HakoniwaProfilerImpl::QueryInterface(" << ConvertStlString(clsidString).c_str() << ")" << std::endl;
		::CoTaskMemFree(clsid);
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
		*ppObj = mCorProfilerInfo;
		mCorProfilerInfo->AddRef();
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
	fprintf(stderr,"HakoniwaProfilerImpl::Initialize()\n");

	// get the ICorProfilerInfo interface
	HRESULT hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo, (LPVOID*)&mCorProfilerInfo);
	if (FAILED(hr)) {
		fprintf(stderr,"Error: Failed to get ICorProfilerInfo\n");
		return E_FAIL;
	} else {
		fprintf(stderr,"Got ICorProfilerInfo\n");
	}

	hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo2, (LPVOID*)&mCorProfilerInfo2);
	if (FAILED(hr)) {
		mCorProfilerInfo2 = nullptr;
		fprintf(stderr,"Error: Failed to get ICorProfiler2\n");
	} else {
		fprintf(stderr,"Got ICorProfilerInfo2\n");
	}

	// Tell the profiler API which events we want to listen to
	// Some events fail when we attach afterwards

	hr = SetProfilerEventMask();
	if (FAILED(hr)) {
		fprintf(stderr,"Error: Failed to SetProfilerEventMask\n");
	} else {
		fprintf(stderr,"SetEventMask()\n");
	}

	fprintf(stderr, "Successfully initialized profiling\n");
	OutputDebugString(L"Successfully initialized profiling\n");

	// m_rewritehelper.SetCorProfilerInfo(mCorProfilerInfo);
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

STDMETHODIMP HakoniwaProfilerImpl::JITCompilationStarted(FunctionID functionID, BOOL fIsSafeToBlock) {
	EnterCriticalSection(&mCriticalSection);

	LeaveCriticalSection(&mCriticalSection);
	return S_OK;
}

STDMETHODIMP HakoniwaProfilerImpl::JITCompilationFinished(FunctionID functionID, HRESULT hrStatus, BOOL fIsSafeToBlock) {
	//std::ostringstream o;
	//o << "HakoniwaProfilerImpl::JITCompilationFinished(";
	//o << functionID;
	//o << ")" << std::endl << std::endl;
	//std::string msg = o.str();
	//g_debugLogger << msg.c_str();
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
