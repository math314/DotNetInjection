#include "Tranpoline.h"

#include "FunctionInfo.h"
#include "Debugger.h"
#include "ComUtil.h"

#include <vector>

using Microsoft::WRL::ComPtr;

void debug(IMetaDataImport* metaDataImport, FunctionInfo* fi, PCCOR_SIGNATURE signatureBlob) {
	PCCOR_SIGNATURE signatureBlobOrigin = signatureBlob;

	ULONG callConvension = IMAGE_CEE_CS_CALLCONV_MAX;
	signatureBlob += CorSigUncompressData(signatureBlob, &callConvension);
	// DebugPrintf(L"callConvension = %X", callConvension);

	ULONG argumentCount;
	signatureBlob += CorSigUncompressData(signatureBlob, &argumentCount);
	// DebugPrintf(L"argumentCount = %d", argumentCount);

	WCHAR returnType[2048];
	returnType[0] = '\0';
	signatureBlob = FunctionInfo::ParseSignature(metaDataImport, signatureBlob, returnType);
	// DebugPrintf(L"returnType = %s", returnType);

	WCHAR signatureText[2048] = L"";
	wsprintf(signatureText, L"fid=%08X|%s %s %s::%s",
		fi->get_FunctionID(),
		(fi->get_MethodAttributes() & mdStatic) == 0 ? L"(nonstatic)" : L"static", returnType, fi->get_ClassName().c_str(), fi->get_FunctionName().c_str()
		);

	std::vector<std::wstring> arguments;
	for (ULONG i = 0; (signatureBlob != NULL) && (i < argumentCount); ++i) {
		WCHAR parameters[2048];
		parameters[0] = '\0';
		signatureBlob = FunctionInfo::ParseSignature(metaDataImport, signatureBlob, parameters);
		// DebugPrintf(L"arguments %d : %s",i,parameters);
		arguments.push_back(parameters);
	}

	lstrcatW(signatureText, L"(");
	for (ULONG i = 0; i < arguments.size(); i++) {
		if (i != 0) lstrcatW(signatureText, L",");
		lstrcatW(signatureText, arguments[i].c_str());
	}
	lstrcatW(signatureText, L")");
	// DebugPrintf(L"%s",signatureText);
}

static void dump(const void* p, int size) {
	WCHAR b[2000];
	for (int i = 0; i < size; i++) {
		wsprintf(b + 3 * i, L"%02X,", ((const BYTE*)p)[i]);
	}
	DebugPrintf(L"size = %d,body = %s", size, b);
}

std::vector<BYTE> Tranpoline::GetFunctionSignatureBlob() {
	const std::vector<BYTE>& oldSignatureBlob = fi->get_SignatureBlob();
	
	if (IsMdStatic(fi->get_MethodAttributes())) {
		// DebugPrintf(L"static method replacement");
		std::vector<BYTE> newSignatureBlob = oldSignatureBlob;
		newSignatureBlob[0] = IMAGE_CEE_CS_CALLCONV_DEFAULT;
		return newSignatureBlob;
	} else {
		// DebugPrintf(L"non static method remplacement");
		ComPtr<IMetaDataImport> metaDataImport;
		hrCheck(info->GetTokenAndMetaDataFromFunction(fi->get_FunctionID(), IID_IMetaDataImport, (LPUNKNOWN *)&metaDataImport, nullptr));

		std::vector<BYTE> newSignatureBlob(oldSignatureBlob.size() * 2 + 5); // rough estimation
		PCCOR_SIGNATURE pOldData = &oldSignatureBlob[0];
		PCOR_SIGNATURE pNewData = &newSignatureBlob[0];

		ULONG callConvension = IMAGE_CEE_CS_CALLCONV_MAX;
		pOldData += CorSigUncompressData(pOldData, &callConvension);
		pNewData += CorSigCompressData(IMAGE_CEE_CS_CALLCONV_DEFAULT, pNewData);

		ULONG argumentCount;
		pOldData += CorSigUncompressData(pOldData, &argumentCount);
		pNewData += CorSigCompressData(argumentCount + 1, pNewData);

		WCHAR returnType[2048];
		returnType[0] = '\0';
		PCCOR_SIGNATURE retTypeSig = FunctionInfo::ParseSignature(metaDataImport.Get(), pOldData, returnType);
		int retTypeBlobSize = retTypeSig - pOldData;
		memcpy(pNewData, pOldData, retTypeBlobSize);
		pOldData += retTypeBlobSize;
		pNewData += retTypeBlobSize;

		//insert new arguments to newSignature
		*pNewData++ = ELEMENT_TYPE_CLASS;
		ULONG newArgumentTokenSize = CorSigCompressToken(fi->get_ClassTypeDef(), pNewData);
		pNewData += newArgumentTokenSize;

		//add all arguments to newSignature
		int remineSize = pOldData - &oldSignatureBlob[0];
		memcpy(pNewData, pOldData, remineSize);

		newSignatureBlob.resize(oldSignatureBlob.size() + 1 + newArgumentTokenSize);

		debug(metaDataImport.Get(), fi.get(), &newSignatureBlob[0]);

		return newSignatureBlob;
	}
}

mdMemberRef Tranpoline::DefineHakoniwaMethodIntoThisAssembly(const wchar_t* fullyQualifiedClassName, const wchar_t* methodName) {
	static const WCHAR assemblyName[] = L"HakoniwaProfiler.MethodHook";
	static const std::vector<BYTE> publicToken = { 0x41, 0x91, 0x82, 0x76, 0xff, 0x21, 0x51, 0x80 };

	//query interface
	ComPtr<IMetaDataEmit> metaDataEmit;
	hrCheck(info->GetModuleMetaData(fi->get_ModuleID(), ofRead | ofWrite, IID_IMetaDataEmit, (IUnknown**)metaDataEmit.GetAddressOf()));

	//we can not compile """metaDataEmit->As(metaDataAssemblyEmit)""" since `__uuidof(IMetaDataAssemblyEmit)` is undefined... 
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

	std::vector<BYTE> defineSignatureBlob = GetFunctionSignatureBlob();

	mdMemberRef memberRef = mdMemberRefNil;
	hrCheck(metaDataEmit->DefineMemberRef(typeRef, methodName, &(defineSignatureBlob[0]), defineSignatureBlob.size(), &memberRef));

	return memberRef;
}

ULONG Tranpoline::calcNewMethodArgCount() {
	ULONG newArguments = fi->get_ArgumentCount();
	if (!IsMdStatic(fi->get_MethodAttributes())) {
		newArguments++; //push caller object
	}

	if (newArguments > 0xFF) {
		DebugPrintf(L"too many arguments");
		exit(-1);
	}

	return newArguments;
}

std::vector<BYTE> Tranpoline::ConstructTranpolineMethodIL(mdMemberRef mdCallFunctionRef) {
	std::vector<BYTE> newILs;
	ULONG newArguments = calcNewMethodArgCount();

	// ldarg.0, ldarg.1 ... ldarg. newArguments - 1
	for (ULONG i = 0; i < newArguments; i++) {
		if (i < 4) {
			newILs.push_back(0x02 + (BYTE)i); //ldarg.0 ~ ldarg.3
		} else {
			newILs.push_back(0x0E);
			newILs.push_back((BYTE)i); //ldarg.s <index>
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

std::vector<BYTE> Tranpoline::ConstructTranpolineMethodHeader(DWORD codeSize) {
	COR_ILMETHOD_FAT* oldHeader;
	ULONG size;
	info->GetILFunctionBody(fi->get_ModuleID(), fi->get_FunctionToken(), (LPCBYTE *)&oldHeader, &size);
	// dump(oldHeader, size);

	COR_ILMETHOD_FAT fatHeader;
	if (oldHeader->IsFat()) {
		// fatHeader.SetSize(sizeof(COR_ILMETHOD_FAT) / sizeof(DWORD));
		memcpy(&fatHeader, oldHeader, sizeof(COR_ILMETHOD_FAT));
		fatHeader.SetCodeSize(codeSize);
		fatHeader.SetMaxStack(calcNewMethodArgCount() + 1); // (arguments + function return val) size
		
		std::vector<BYTE> ret(sizeof(COR_ILMETHOD_FAT));
		memcpy(&ret[0], &fatHeader, sizeof(COR_ILMETHOD_FAT));
		return ret;
	} else {
		COR_ILMETHOD_TINY tiny;
		tiny.Flags_CodeSize = (BYTE)(CorILMethod_TinyFormat | (codeSize * 4));
		std::vector<BYTE> ret(1);
		ret[0] = *((BYTE*)&tiny);
		return ret;
	}
}

void* Tranpoline::AllocateFuctionBody(DWORD size) {
	ComPtr<IMethodMalloc> methodMalloc;
	hrCheck(info->GetILFunctionBodyAllocator(fi->get_ModuleID(), &methodMalloc));
	void *allocated = methodMalloc->Alloc(size);
	return allocated;
}

void Tranpoline::Update(const wchar_t* className, const wchar_t* methodName) {
	mdMemberRef newMemberRef = DefineHakoniwaMethodIntoThisAssembly(className, methodName);

	std::vector<BYTE> newILs = ConstructTranpolineMethodIL(newMemberRef);
	std::vector<BYTE> newHeader = ConstructTranpolineMethodHeader(newILs.size());

	ULONG newMethodSize = newHeader.size() + newILs.size();
	void *allocated = AllocateFuctionBody(newMethodSize);

	//write new Header
	memcpy(allocated, &newHeader[0], newHeader.size());
	//write new IL
	memcpy((BYTE*)allocated + newHeader.size(), &newILs[0], newILs.size());

	// dump(allocated, newHeader.size() + newILs.size());

	hrCheck(info->SetILFunctionBody(fi->get_ModuleID(), fi->get_FunctionToken(), (LPCBYTE)allocated));
}