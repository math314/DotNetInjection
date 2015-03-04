#pragma once

#include <Windows.h>
#include <cor.h>
#include <corprof.h>
#include <string>

class FunctionInfo {
public:
	static FunctionInfo *CreateFunctionInfo(ICorProfilerInfo *profilerInfo, FunctionID functionID);
	~FunctionInfo() {};

	FunctionID get_FunctionID() const { return mFunctionID; }
	ClassID get_ClassID() const { return mClassID; }
	ModuleID get_ModuleID() const { return mModuleID; }
	mdToken get_Token() const { return mToken; }
	const std::wstring& get_ClassName() const { return mClassName; }
	const std::wstring& get_FunctionName() const { return mFunctionName; }
	const std::wstring& get_AssemblyName() const { return mAssemblyName; }
	const std::wstring& get_SignatureText() const { return mSignatureText; }

private:
	FunctionInfo(FunctionID functionID, ClassID classID, ModuleID moduleID, mdToken token, LPWSTR functionName, LPWSTR className, LPWSTR assemblyName, LPWSTR signatureText);
	
	static PCCOR_SIGNATURE ParseSignature(IMetaDataImport *pMDImport, PCCOR_SIGNATURE signature, WCHAR* szBuffer);
	
	FunctionID mFunctionID;
	ClassID mClassID;
	ModuleID mModuleID;
	mdToken mToken;
	std::wstring mClassName;
	std::wstring mFunctionName;
	std::wstring mAssemblyName;
	std::wstring mSignatureText;
};