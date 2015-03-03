#pragma once

#include "HakoniwaProfiler_h.h"

#include <cor.h>
#include <corprof.h>

class HakoniwaProfilerBase :
	public ICorProfilerCallback3,
	public HakoniwaProfiler {
public:
	STDMETHOD(QueryInterface)(REFIID riid, void **ppObj) = 0;
	ULONG STDMETHODCALLTYPE AddRef() = 0;
	ULONG STDMETHODCALLTYPE Release() = 0;

	STDMETHOD(Initialize)(IUnknown *pICorProfilerInfoUnk) { return S_OK; }
	STDMETHOD(Shutdown)() { return S_OK; }
	STDMETHOD(AppDomainCreationStarted)(AppDomainID appDomainID) { return S_OK; }
	STDMETHOD(AppDomainCreationFinished)(AppDomainID appDomainID, HRESULT hrStatus) { return S_OK; }
	STDMETHOD(AppDomainShutdownStarted)(AppDomainID appDomainID) { return S_OK; }
	STDMETHOD(AppDomainShutdownFinished)(AppDomainID appDomainID, HRESULT hrStatus) { return S_OK; }
	STDMETHOD(AssemblyLoadStarted)(AssemblyID assemblyID) { return S_OK; }
	STDMETHOD(AssemblyLoadFinished)(AssemblyID assemblyID, HRESULT hrStatus) { return S_OK; }
	STDMETHOD(AssemblyUnloadStarted)(AssemblyID assemblyID) { return S_OK; }
	STDMETHOD(AssemblyUnloadFinished)(AssemblyID assemblyID, HRESULT hrStatus) { return S_OK; }
	STDMETHOD(ModuleLoadStarted)(ModuleID moduleID) { return S_OK; }
	STDMETHOD(ModuleLoadFinished)(ModuleID moduleID, HRESULT hrStatus) { return S_OK; }
	STDMETHOD(ModuleUnloadStarted)(ModuleID moduleID) { return S_OK; }
	STDMETHOD(ModuleUnloadFinished)(ModuleID moduleID, HRESULT hrStatus) { return S_OK; }
	STDMETHOD(ModuleAttachedToAssembly)(ModuleID moduleID, AssemblyID assemblyID) { return S_OK; }
	STDMETHOD(ClassLoadStarted)(ClassID classID) { return S_OK; }
	STDMETHOD(ClassLoadFinished)(ClassID classID, HRESULT hrStatus) { return S_OK; }
	STDMETHOD(ClassUnloadStarted)(ClassID classID) { return S_OK; }
	STDMETHOD(ClassUnloadFinished)(ClassID classID, HRESULT hrStatus) { return S_OK; }
	STDMETHOD(FunctionUnloadStarted)(FunctionID functionID) { return S_OK; }
	STDMETHOD(JITCompilationStarted)(FunctionID functionID, BOOL fIsSafeToBlock) { return S_OK; }
	STDMETHOD(JITCompilationFinished)(FunctionID functionID, HRESULT hrStatus, BOOL fIsSafeToBlock) { return S_OK; }
	STDMETHOD(JITCachedFunctionSearchStarted)(FunctionID functionID, BOOL *pbUseCachedFunction) { return S_OK; }
	STDMETHOD(JITCachedFunctionSearchFinished)(FunctionID functionID, COR_PRF_JIT_CACHE result) { return S_OK; }
	STDMETHOD(JITFunctionPitched)(FunctionID functionID) { return S_OK; }
	STDMETHOD(JITInlining)(FunctionID callerID, FunctionID calleeID, BOOL *pfShouldInline) { return S_OK; }
	STDMETHOD(ThreadCreated)(ThreadID threadID) { return S_OK; }
	STDMETHOD(ThreadDestroyed)(ThreadID threadID) { return S_OK; }
	STDMETHOD(ThreadAssignedToOSThread)(ThreadID managedThreadID, DWORD osThreadID) { return S_OK; }
	STDMETHOD(RemotingClientInvocationStarted)() { return S_OK; }
	STDMETHOD(RemotingClientSendingMessage)(GUID *pCookie, BOOL fIsAsync) { return S_OK; }
	STDMETHOD(RemotingClientReceivingReply)(GUID *pCookie, BOOL fIsAsync) { return S_OK; }
	STDMETHOD(RemotingClientInvocationFinished)() { return S_OK; }
	STDMETHOD(RemotingServerReceivingMessage)(GUID *pCookie, BOOL fIsAsync) { return S_OK; }
	STDMETHOD(RemotingServerInvocationStarted)() { return S_OK; }
	STDMETHOD(RemotingServerInvocationReturned)() { return S_OK; }
	STDMETHOD(RemotingServerSendingReply)(GUID *pCookie, BOOL fIsAsync) { return S_OK; }
	STDMETHOD(UnmanagedToManagedTransition)(FunctionID functionID, COR_PRF_TRANSITION_REASON reason) { return S_OK; }
	STDMETHOD(ManagedToUnmanagedTransition)(FunctionID functionID, COR_PRF_TRANSITION_REASON reason) { return S_OK; }
	STDMETHOD(RuntimeSuspendStarted)(COR_PRF_SUSPEND_REASON suspendReason) { return S_OK; }
	STDMETHOD(RuntimeSuspendFinished)() { return S_OK; }
	STDMETHOD(RuntimeSuspendAborted)() { return S_OK; }
	STDMETHOD(RuntimeResumeStarted)() { return S_OK; }
	STDMETHOD(RuntimeResumeFinished)() { return S_OK; }
	STDMETHOD(RuntimeThreadSuspended)(ThreadID threadid) { return S_OK; }
	STDMETHOD(RuntimeThreadResumed)(ThreadID threadid) { return S_OK; }
	STDMETHOD(MovedReferences)(ULONG cmovedObjectIDRanges, ObjectID oldObjectIDRangeStart[], ObjectID newObjectIDRangeStart[], ULONG cObjectIDRangeLength[]) { return S_OK; }
	STDMETHOD(ObjectAllocated)(ObjectID objectID, ClassID classID) { return S_OK; }
	STDMETHOD(ObjectsAllocatedByClass)(ULONG classCount, ClassID classIDs[], ULONG objects[]) { return S_OK; }
	STDMETHOD(ObjectReferences)(ObjectID objectID, ClassID classID, ULONG cObjectRefs, ObjectID objectRefIDs[]) { return S_OK; }
	STDMETHOD(RootReferences)(ULONG cRootRefs, ObjectID rootRefIDs[]) { return S_OK; }
	STDMETHOD(ExceptionThrown)(ObjectID thrownObjectID) { return S_OK; }
	STDMETHOD(ExceptionSearchFunctionEnter)(FunctionID functionID) { return S_OK; }
	STDMETHOD(ExceptionSearchFunctionLeave)() { return S_OK; }
	STDMETHOD(ExceptionSearchFilterEnter)(FunctionID functionID) { return S_OK; }
	STDMETHOD(ExceptionSearchFilterLeave)() { return S_OK; }
	STDMETHOD(ExceptionSearchCatcherFound)(FunctionID functionID) { return S_OK; }
	STDMETHOD(ExceptionCLRCatcherFound)() { return S_OK; }
	STDMETHOD(ExceptionCLRCatcherExecute)() { return S_OK; }
	STDMETHOD(ExceptionOSHandlerEnter)(FunctionID functionID) { return S_OK; }
	STDMETHOD(ExceptionOSHandlerLeave)(FunctionID functionID) { return S_OK; }
	STDMETHOD(ExceptionUnwindFunctionEnter)(FunctionID functionID) { return S_OK; }
	STDMETHOD(ExceptionUnwindFunctionLeave)() { return S_OK; }
	STDMETHOD(ExceptionUnwindFinallyEnter)(FunctionID functionID) { return S_OK; }
	STDMETHOD(ExceptionUnwindFinallyLeave)() { return S_OK; }
	STDMETHOD(ExceptionCatcherEnter)(FunctionID functionID, ObjectID objectID) { return S_OK; }
	STDMETHOD(ExceptionCatcherLeave)() { return S_OK; }
	STDMETHOD(COMClassicVTableCreated)(ClassID wrappedClassID, REFGUID implementedIID, void *pVTable, ULONG cSlots) { return S_OK; }
	STDMETHOD(COMClassicVTableDestroyed)(ClassID wrappedClassID, REFGUID implementedIID, void *pVTable) { return S_OK; }
	STDMETHOD(ThreadNameChanged)(ThreadID threadId, ULONG cchName, WCHAR name[]) { return S_OK; }
	STDMETHOD(GarbageCollectionStarted)(int cGenerations, BOOL generationCollected[], COR_PRF_GC_REASON reason) { return S_OK; }
	STDMETHOD(SurvivingReferences)(ULONG cSurvivingObjectIDRanges, ObjectID objectIDRangeStart[], ULONG cObjectIDRangeLength[]) { return S_OK; }
	STDMETHOD(GarbageCollectionFinished)() { return S_OK; }
	STDMETHOD(FinalizeableObjectQueued)(DWORD finalizerFlags, ObjectID objectID) { return S_OK; }
	STDMETHOD(RootReferences2)(ULONG cRootRefs, ObjectID rootRefIds[], COR_PRF_GC_ROOT_KIND rootKinds[], COR_PRF_GC_ROOT_FLAGS rootFlags[], UINT_PTR rootIds[]) { return S_OK; }
	STDMETHOD(HandleCreated)(GCHandleID handleId, ObjectID initialObjectId) { return S_OK; }
	STDMETHOD(HandleDestroyed)(GCHandleID handleId) { return S_OK; }
	STDMETHOD(InitializeForAttach)(IUnknown *pCorProfilerInfoUnk, void *pvClientData, UINT cbClientData) { return S_OK; }
	STDMETHOD(ProfilerAttachComplete)(void) { return S_OK; }
	STDMETHOD(ProfilerDetachSucceeded)(void) { return S_OK; }
};