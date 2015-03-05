#pragma once

#include <Windows.h>
#include <cor.h>
#include <corprof.h>
#include <string>
#include <vector>

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
	const std::vector<BYTE>& get_SignatureBlob() const { return mSignatureBlob; }
	const DWORD get_MethodAttributes() const { return mMethodAttributes; }

	static PCCOR_SIGNATURE ParseSignature(IMetaDataImport *pMDImport, PCCOR_SIGNATURE signature, WCHAR* szBuffer);

private:
	FunctionInfo() {};

	FunctionID mFunctionID;
	ClassID mClassID;
	ModuleID mModuleID;
	mdToken mToken;
	std::wstring mClassName;
	std::wstring mFunctionName;
	std::wstring mAssemblyName;
	std::wstring mSignatureText;

	DWORD mMethodAttributes;
	std::vector<BYTE> mSignatureBlob;
};