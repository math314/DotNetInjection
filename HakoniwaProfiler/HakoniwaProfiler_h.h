

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Mar 07 14:29:50 2015
 */
/* Compiler settings for HakoniwaProfiler.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __HakoniwaProfiler_h_h__
#define __HakoniwaProfiler_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __HakoniwaProfiler_FWD_DEFINED__
#define __HakoniwaProfiler_FWD_DEFINED__
typedef interface HakoniwaProfiler HakoniwaProfiler;

#endif 	/* __HakoniwaProfiler_FWD_DEFINED__ */


#ifndef __HakoniwaProfilerImpl_FWD_DEFINED__
#define __HakoniwaProfilerImpl_FWD_DEFINED__

#ifdef __cplusplus
typedef class HakoniwaProfilerImpl HakoniwaProfilerImpl;
#else
typedef struct HakoniwaProfilerImpl HakoniwaProfilerImpl;
#endif /* __cplusplus */

#endif 	/* __HakoniwaProfilerImpl_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __HakoniwaProfiler_INTERFACE_DEFINED__
#define __HakoniwaProfiler_INTERFACE_DEFINED__

/* interface HakoniwaProfiler */
/* [oleautomation][uuid][object] */ 


EXTERN_C const IID IID_HakoniwaProfiler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DBFCC66B-281D-41C7-A2E0-355580315B43")
    HakoniwaProfiler : public IUnknown
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct HakoniwaProfilerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            HakoniwaProfiler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            HakoniwaProfiler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            HakoniwaProfiler * This);
        
        END_INTERFACE
    } HakoniwaProfilerVtbl;

    interface HakoniwaProfiler
    {
        CONST_VTBL struct HakoniwaProfilerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define HakoniwaProfiler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define HakoniwaProfiler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define HakoniwaProfiler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __HakoniwaProfiler_INTERFACE_DEFINED__ */



#ifndef __HakoniwaProfilerLib_LIBRARY_DEFINED__
#define __HakoniwaProfilerLib_LIBRARY_DEFINED__

/* library HakoniwaProfilerLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_HakoniwaProfilerLib;

EXTERN_C const CLSID CLSID_HakoniwaProfilerImpl;

#ifdef __cplusplus

class DECLSPEC_UUID("9992F2A6-DF35-472B-AD3E-317F85D958D7")
HakoniwaProfilerImpl;
#endif
#endif /* __HakoniwaProfilerLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


