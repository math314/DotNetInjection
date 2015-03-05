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

	mdMemberRef Tranpoline::DefineInjectionMethod(const wchar_t* assemblyName, std::vector<BYTE>& publicToken, const wchar_t* fullyQualifiedClassName, const wchar_t* methodName);
	BYTE calcNewMethodArgCount();
	void* AllocateFuctionBody(DWORD size);
	std::vector<BYTE> ConstructTranpolineMethodIL(mdMemberRef mdCallFunctionRef);
	COR_ILMETHOD_FAT ConstructTranpolineMethodBody(DWORD codeSize);

private:
	Microsoft::WRL::ComPtr<ICorProfilerInfo2> info;
	std::shared_ptr<FunctionInfo> fi;
};