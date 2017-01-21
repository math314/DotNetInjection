// FunctionInfo class is originally from https://www.codeproject.com/articles/17275/really-easy-logging-using-il-rewriting-and-the-net
// modified by https://github.com/math314/

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
	mdTypeDef get_ClassTypeDef() const { return mClassTypeDef; }
	ModuleID get_ModuleID() const { return mModuleID; }
	mdToken get_FunctionToken() const { return mFunctionToken; }
	const std::wstring& get_ClassName() const { return mClassName; }
	const std::wstring& get_FunctionName() const { return mFunctionName; }
	const std::wstring& get_AssemblyName() const { return mAssemblyName; }
	const std::wstring& get_SignatureText() const { return mSignatureText; }
	const std::vector<BYTE>& get_SignatureBlob() const { return mSignatureBlob; }
	DWORD get_MethodAttributes() const { return mMethodAttributes; }
	ULONG get_ArgumentCount() const { return mArguments.size(); }
	const std::wstring& get_RetType() const { return mRetType; }
	const std::vector<std::wstring>& get_Arguments() const { return mArguments; }

	static PCCOR_SIGNATURE ParseSignature(IMetaDataImport *pMDImport, PCCOR_SIGNATURE signature, WCHAR* szBuffer);

private:
	FunctionInfo() {};

	FunctionID mFunctionID;
	ClassID mClassID;
	mdTypeDef mClassTypeDef;
	ModuleID mModuleID;
	mdToken mFunctionToken;
	std::wstring mClassName;
	std::wstring mFunctionName;
	std::wstring mAssemblyName;
	std::wstring mSignatureText;

	DWORD mMethodAttributes;
	std::vector<BYTE> mSignatureBlob;
	std::wstring mRetType;
	std::vector<std::wstring> mArguments;
};