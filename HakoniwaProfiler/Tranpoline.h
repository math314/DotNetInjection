#pragma once

#include <cor.h>
#include <corprof.h>
#include <corhlpr.h>

#include <memory>
#include <vector>
#include <wrl/client.h>

class FunctionInfo;

class Tranpoline {
public:
	Tranpoline(Microsoft::WRL::ComPtr<ICorProfilerInfo2>& info, std::shared_ptr<FunctionInfo>& fi)
		: info(info), fi(fi)
	{
	}

	void Update(const wchar_t* className, const wchar_t* methodName);

private:
	std::vector<BYTE> Tranpoline::GetFunctionSignatureBlob();
	mdMemberRef Tranpoline::DefineHakoniwaMethodIntoThisAssembly(const wchar_t* fullyQualifiedClassName, const wchar_t* methodName);
	ULONG calcNewMethodArgCount();
	void* AllocateFuctionBody(DWORD size);
	std::vector<BYTE> ConstructTranpolineMethodIL(mdMemberRef mdCallFunctionRef);
	std::vector<BYTE> ConstructTranpolineMethodHeader(DWORD codeSize);

	Microsoft::WRL::ComPtr<ICorProfilerInfo2> info;
	std::shared_ptr<FunctionInfo> fi;
};