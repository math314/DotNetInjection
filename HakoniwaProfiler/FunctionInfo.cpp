// FunctionInfo class is originally from https://www.codeproject.com/articles/17275/really-easy-logging-using-il-rewriting-and-the-net
// modified by https://github.com/math314/

#define _CRT_SECURE_NO_WARNINGS
#include "FunctionInfo.h"
#include "ComUtil.h"
#include "Debugger.h"

#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

const int MAX_LENGTH = 2048;
FunctionInfo *FunctionInfo::CreateFunctionInfo(ICorProfilerInfo *profilerInfo, FunctionID functionID)
{
	ClassID classID = 0;
	ModuleID moduleID = 0;
	mdToken tkMethod = 0;
	hrCheck(profilerInfo->GetFunctionInfo(functionID, &classID, &moduleID, &tkMethod));

	WCHAR moduleName[MAX_LENGTH];
	AssemblyID assemblyID;
	hrCheck(profilerInfo->GetModuleInfo(moduleID, NULL, MAX_LENGTH, 0, moduleName, &assemblyID));

	WCHAR assemblyName[MAX_LENGTH];
	hrCheck(profilerInfo->GetAssemblyInfo(assemblyID, MAX_LENGTH, 0, assemblyName, NULL, NULL));

	ComPtr<IMetaDataImport> metaDataImport;
	mdToken token = 0;
	hrCheck(profilerInfo->GetTokenAndMetaDataFromFunction(functionID, IID_IMetaDataImport, (LPUNKNOWN *)&metaDataImport, &token));

	mdTypeDef classTypeDef;
	WCHAR functionName[MAX_LENGTH];
	WCHAR className[MAX_LENGTH];
	PCCOR_SIGNATURE signatureBlob;
	ULONG signatureBlobLength;
	DWORD methodAttributes = 0;
	hrCheck(metaDataImport->GetMethodProps(token, &classTypeDef, functionName, MAX_LENGTH, 0, &methodAttributes, &signatureBlob, &signatureBlobLength, NULL, NULL));
	hrCheck(metaDataImport->GetTypeDefProps(classTypeDef, className, MAX_LENGTH, 0, NULL, NULL));

	PCCOR_SIGNATURE signatureBlobOrigin = signatureBlob;

	ULONG callConvension = IMAGE_CEE_CS_CALLCONV_MAX;
	signatureBlob += CorSigUncompressData(signatureBlob, &callConvension);

	ULONG argumentCount;
	signatureBlob += CorSigUncompressData(signatureBlob, &argumentCount);

	WCHAR returnType[MAX_LENGTH];
	returnType[0] = '\0';
	signatureBlob = ParseSignature(metaDataImport.Get(), signatureBlob, returnType);

	WCHAR signatureText[MAX_LENGTH] = L"";
	wsprintf(signatureText, L"fid=%08X|%s %s %s::%s",
		functionID,
		(methodAttributes & mdStatic) == 0 ? L"(nonstatic)" : L"static", returnType, className, functionName
		);

	std::vector<std::wstring> arguments;
	for (ULONG i = 0; (signatureBlob != NULL) && (i < argumentCount); ++i) {
		WCHAR parameters[MAX_LENGTH];
		parameters[0] = '\0';
		signatureBlob = ParseSignature(metaDataImport.Get(), signatureBlob, parameters);
		arguments.push_back(parameters);
	}

	lstrcatW(signatureText, L"(");
	for (ULONG i = 0; i < arguments.size(); i++) {
		if(i != 0) lstrcatW(signatureText, L",");
		lstrcatW(signatureText, arguments[i].c_str());
	}
	lstrcatW(signatureText, L")");

	FunctionInfo* result = new FunctionInfo();
	
	result->mFunctionID = functionID;
	result->mClassID = classID;
	result->mClassTypeDef = classTypeDef;
	result->mModuleID = moduleID;
	result->mFunctionToken = tkMethod;
	result->mFunctionName = functionName;
	result->mClassName = className;
	result->mAssemblyName = assemblyName;
	result->mSignatureText = signatureText;
	result->mSignatureBlob = std::vector<BYTE>(signatureBlobOrigin, signatureBlob);
	result->mMethodAttributes = methodAttributes;
	result->mRetType = returnType;
	result->mArguments = arguments;

	return result;
}

PCCOR_SIGNATURE FunctionInfo::ParseSignature(IMetaDataImport *metaDataImport, PCCOR_SIGNATURE signature, LPWSTR signatureText)
{
	COR_SIGNATURE corSignature = *signature++;

	switch (corSignature) {
	case ELEMENT_TYPE_VOID:
		wcscat(signatureText, L"void");
		break;
	case ELEMENT_TYPE_BOOLEAN:
		wcscat(signatureText, L"bool");
		break;
	case ELEMENT_TYPE_CHAR:
		wcscat(signatureText, L"wchar");
		break;
	case ELEMENT_TYPE_I1:
		wcscat(signatureText, L"int8");
		break;
	case ELEMENT_TYPE_U1:
		wcscat(signatureText, L"unsigned int8");
		break;
	case ELEMENT_TYPE_I2:
		wcscat(signatureText, L"int16");
		break;
	case ELEMENT_TYPE_U2:
		wcscat(signatureText, L"unsigned int16");
		break;
	case ELEMENT_TYPE_I4:
		wcscat(signatureText, L"int32");
		break;
	case ELEMENT_TYPE_U4:
		wcscat(signatureText, L"unsigned int32");
		break;
	case ELEMENT_TYPE_I8:
		wcscat(signatureText, L"int64");
		break;
	case ELEMENT_TYPE_U8:
		wcscat(signatureText, L"unsigned int64");
		break;
	case ELEMENT_TYPE_R4:
		wcscat(signatureText, L"float32");
		break;
	case ELEMENT_TYPE_R8:
		wcscat(signatureText, L"float64");
		break;
	case ELEMENT_TYPE_STRING:
		wcscat(signatureText, L"string");
		break;
	case ELEMENT_TYPE_VAR:
		wcscat(signatureText, L"class variable(unsigned int8)");
		break;
	case ELEMENT_TYPE_MVAR:
		wcscat(signatureText, L"method variable(unsigned int8)");
		break;
	case ELEMENT_TYPE_TYPEDBYREF:
		wcscat(signatureText, L"refany");
		break;
	case ELEMENT_TYPE_I:
		wcscat(signatureText, L"int");
		break;
	case ELEMENT_TYPE_U:
		wcscat(signatureText, L"unsigned int");
		break;
	case ELEMENT_TYPE_OBJECT:
		wcscat(signatureText, L"object");
		break;
	case ELEMENT_TYPE_SZARRAY:
		signature = ParseSignature(metaDataImport, signature, signatureText);
		wcscat(signatureText, L"[]");
		break;
	case ELEMENT_TYPE_PINNED:
		signature = ParseSignature(metaDataImport, signature, signatureText);
		wcscat(signatureText, L"pinned");
		break;
	case ELEMENT_TYPE_PTR:
		signature = ParseSignature(metaDataImport, signature, signatureText);
		wcscat(signatureText, L"*");
		break;
	case ELEMENT_TYPE_BYREF:
		signature = ParseSignature(metaDataImport, signature, signatureText);
		wcscat(signatureText, L"&");
		break;
	case ELEMENT_TYPE_VALUETYPE:
	case ELEMENT_TYPE_CLASS:
	case ELEMENT_TYPE_CMOD_REQD:
	case ELEMENT_TYPE_CMOD_OPT:
	{
		mdToken	token;
		signature += CorSigUncompressToken(signature, &token);

		WCHAR className[MAX_LENGTH];
		if (TypeFromToken(token) == mdtTypeRef) {
			hrCheck(metaDataImport->GetTypeRefProps(token, NULL, className, MAX_LENGTH, NULL));
		} else {
			hrCheck(metaDataImport->GetTypeDefProps(token, className, MAX_LENGTH, NULL, NULL, NULL));
		}

		wcscat(signatureText, className);
	}
	break;
	case ELEMENT_TYPE_GENERICINST:
	{
		signature = ParseSignature(metaDataImport, signature, signatureText);

		wcscat(signatureText, L"<");
		ULONG arguments = CorSigUncompressData(signature);
		for (ULONG i = 0; i < arguments; ++i) {
			if (i != 0) {
				wcscat(signatureText, L", ");
			}

			signature = ParseSignature(metaDataImport, signature, signatureText);
		}
		wcscat(signatureText, L">");
	}
	break;
	case ELEMENT_TYPE_ARRAY:
	{
		signature = ParseSignature(metaDataImport, signature, signatureText);
		ULONG rank = CorSigUncompressData(signature);
		if (rank == 0) {
			wcscat(signatureText, L"[?]");
		} else {
			ULONG arraysize = (sizeof(ULONG) * 2 * rank);
			ULONG *lower = (ULONG *)_alloca(arraysize);
			memset(lower, 0, arraysize);
			ULONG *sizes = &lower[rank];

			ULONG numsizes = CorSigUncompressData(signature);
			for (ULONG i = 0; i < numsizes && i < rank; i++) {
				sizes[i] = CorSigUncompressData(signature);
			}

			ULONG numlower = CorSigUncompressData(signature);
			for (ULONG i = 0; i < numlower && i < rank; i++) {
				lower[i] = CorSigUncompressData(signature);
			}

			wcscat(signatureText, L"[");
			for (ULONG i = 0; i < rank; ++i) {
				if (i > 0) {
					wcscat(signatureText, L",");
				}

				if (lower[i] == 0) {
					if (sizes[i] != 0) {
						WCHAR size[50];
						wsprintf(size, L"%d", sizes[i]);
						wcscat(signatureText, size);
					}
				} else {
					WCHAR low[50];
					wsprintf(low, L"%d", lower[i]);
					wcscat(signatureText, low);
					wcscat(signatureText, L"...");

					if (sizes[i] != 0) {
						WCHAR size[50];
						wsprintf(size, L"%d", (lower[i] + sizes[i] + 1));
						wcscat(signatureText, size);
					}
				}
			}
			wcscat(signatureText, L"]");
		}
	}
	break;
	default:
	case ELEMENT_TYPE_END:
	case ELEMENT_TYPE_SENTINEL:
		WCHAR *elementType = new WCHAR[MAX_LENGTH];
		elementType[0] = '\0';
		wsprintf(elementType, L"<UNKNOWN:0x%X>", corSignature);
		wcscat(signatureText, elementType);
		break;
	}

	return signature;
}
