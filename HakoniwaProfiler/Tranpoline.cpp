#include "Tranpoline.h"

#include "FunctionInfo.h"
#include "Debugger.h"
#include "ComUtil.h"

#include <vector>

using Microsoft::WRL::ComPtr;

mdMemberRef Tranpoline::DefineInjectionMethod(const wchar_t* assemblyName, std::vector<BYTE>& publicToken, const wchar_t* fullyQualifiedClassName, const wchar_t* methodName) {
	//query interface
	ComPtr<IMetaDataEmit> metaDataEmit;
	hrCheck(info->GetModuleMetaData(fi->get_ModuleID(), ofRead | ofWrite, IID_IMetaDataEmit, (IUnknown**)metaDataEmit.GetAddressOf()));

	// __uuidof(IMetaDataAssemblyEmit) undefined,
	// so we can not compile """metaDataEmit->As(metaDataAssemblyEmit)""" ... 
	IMetaDataAssemblyEmit* _metaDataAssemblyEmit;
	ComPtr<IMetaDataAssemblyEmit> metaDataAssemblyEmit;
	hrCheck(metaDataEmit.Get()->QueryInterface(IID_IMetaDataAssemblyEmit, (void**)&_metaDataAssemblyEmit)); 
	metaDataAssemblyEmit.Attach(_metaDataAssemblyEmit);

	mdAssemblyRef assemblyRef = mdAssemblyRefNil;
	ASSEMBLYMETADATA assemblyMetaData = { 0 };
	assemblyMetaData.usMajorVersion = 1; //assembly version is 1.0.0.0
	hrCheck(metaDataAssemblyEmit->DefineAssemblyRef(&(publicToken[0]), publicToken.size(), assemblyName, &assemblyMetaData, NULL, 0, 0, &assemblyRef));

	mdTypeRef typeRef = mdTypeRefNil;
	hrCheck(metaDataEmit->DefineTypeRefByName(assemblyRef, fullyQualifiedClassName, &typeRef));

	std::vector<BYTE> defineSignatureBlob = fi->get_SignatureBlob();
	Debugger::printf(L"buffer size = %d", defineSignatureBlob.size());
	defineSignatureBlob[0] = IMAGE_CEE_CS_CALLCONV_DEFAULT;

	if (IsMdStatic(fi->get_MethodAttributes()) == 0) {
		//todo : non static -> static
		// signatureBlob extention(insert arg0 = class)
		Debugger::printf(L"ReplaceTest : no static method replacement : %08X", fi->get_MethodAttributes());
		exit(-1);
	}

	mdMemberRef memberRef = mdMemberRefNil;
	hrCheck(metaDataEmit->DefineMemberRef(typeRef, methodName, &(defineSignatureBlob[0]), defineSignatureBlob.size(), &memberRef));

	return memberRef;
}

BYTE Tranpoline::calcNewMethodStackSize() {
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

std::vector<BYTE> Tranpoline::ConstructTranpolineMethodIL(mdMemberRef mdCallFunctionRef) {
	std::vector<BYTE> newILs;
	ULONG newArguments = calcNewMethodStackSize();

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

COR_ILMETHOD_FAT Tranpoline::ConstructTranpolineMethodBody(DWORD codeSize) {
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

void* Tranpoline::AllocateFuctionBody(DWORD size) {
	ComPtr<IMethodMalloc> methodMalloc;
	hrCheck(info->GetILFunctionBodyAllocator(fi->get_ModuleID(), &methodMalloc));
	void *allocated = methodMalloc->Alloc(size);
	return allocated;
}

void Tranpoline::Update(const wchar_t* className, const wchar_t* methodName) {
	const WCHAR moduleName[] = L"HakoniwaProfiler.MethodHook";
	const BYTE publicToken[] = { 0x41, 0x91, 0x82, 0x76, 0xff, 0x21, 0x51, 0x80 };
	std::vector<BYTE> _publicToken(publicToken, publicToken + sizeof(publicToken));
	mdMemberRef newMemberRef = DefineInjectionMethod(moduleName, _publicToken, className, methodName);

	std::vector<BYTE> newILs = ConstructTranpolineMethodIL(newMemberRef);
	COR_ILMETHOD_FAT newHeader = ConstructTranpolineMethodBody(newILs.size());

	ULONG newMethodSize = sizeof(COR_ILMETHOD_FAT)+newILs.size();
	void *allocated = AllocateFuctionBody(newMethodSize);

	//write new Header
	memcpy(allocated, &newHeader, sizeof(COR_ILMETHOD_FAT));
	//write new IL
	memcpy((BYTE*)allocated + sizeof(COR_ILMETHOD_FAT), &newILs[0], newILs.size());

	//set new function
	hrCheck(info->SetILFunctionBody(fi->get_ModuleID(), fi->get_Token(), (LPCBYTE)allocated));
}