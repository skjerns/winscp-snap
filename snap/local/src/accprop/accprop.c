/*
 * A stand-in for CLSID_AccPropServices whose methods all succeed.
 *
 * WinSCP 6.7 tags its dialogs for screen readers through
 * IAccPropServices::SetHwndProp and treats a failure as an exception. Wine's
 * oleacc only stubs that method (it returns E_NOTIMPL), so every WinSCP
 * dialog throws while it is being built: the confirmation behind the close
 * button never appears, and an error dialog after a failed connection takes
 * the whole application down. Wine does nothing with these properties anyway,
 * so accepting and dropping them loses nothing.
 *
 * The pre-start hook registers this DLL for the 32-bit registry view only,
 * which is the one WinSCP reads.
 */
#define COBJMACROS
#include <windows.h>
#include <initguid.h>
#include <oleacc.h>

static HRESULT WINAPI aps_QueryInterface(IAccPropServices *iface, REFIID riid, void **ppv)
{
    if (IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid, &IID_IAccPropServices)) {
        *ppv = iface;
        return S_OK;
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}

/* A single static instance, so reference counting has nothing to free. */
static ULONG WINAPI aps_AddRef(IAccPropServices *iface) { return 2; }
static ULONG WINAPI aps_Release(IAccPropServices *iface) { return 1; }

static HRESULT WINAPI aps_SetPropValue(IAccPropServices *iface, const BYTE *id, DWORD idlen,
    MSAAPROPID prop, VARIANT var) { return S_OK; }
static HRESULT WINAPI aps_SetPropServer(IAccPropServices *iface, const BYTE *id, DWORD idlen,
    const MSAAPROPID *props, int count, IAccPropServer *server, AnnoScope scope) { return S_OK; }
static HRESULT WINAPI aps_ClearProps(IAccPropServices *iface, const BYTE *id, DWORD idlen,
    const MSAAPROPID *props, int count) { return S_OK; }
static HRESULT WINAPI aps_SetHwndProp(IAccPropServices *iface, HWND hwnd, DWORD obj, DWORD child,
    MSAAPROPID prop, VARIANT var) { return S_OK; }
static HRESULT WINAPI aps_SetHwndPropStr(IAccPropServices *iface, HWND hwnd, DWORD obj, DWORD child,
    MSAAPROPID prop, LPCWSTR str) { return S_OK; }
static HRESULT WINAPI aps_SetHwndPropServer(IAccPropServices *iface, HWND hwnd, DWORD obj, DWORD child,
    const MSAAPROPID *props, int count, IAccPropServer *server, AnnoScope scope) { return S_OK; }
static HRESULT WINAPI aps_ClearHwndProps(IAccPropServices *iface, HWND hwnd, DWORD obj, DWORD child,
    const MSAAPROPID *props, int count) { return S_OK; }
static HRESULT WINAPI aps_ComposeHwndIdentityString(IAccPropServices *iface, HWND hwnd, DWORD obj,
    DWORD child, BYTE **id, DWORD *idlen) { return E_NOTIMPL; }
static HRESULT WINAPI aps_DecomposeHwndIdentityString(IAccPropServices *iface, const BYTE *id,
    DWORD idlen, HWND *hwnd, DWORD *obj, DWORD *child) { return E_NOTIMPL; }
static HRESULT WINAPI aps_SetHmenuProp(IAccPropServices *iface, HMENU hmenu, DWORD child,
    MSAAPROPID prop, VARIANT var) { return S_OK; }
static HRESULT WINAPI aps_SetHmenuPropStr(IAccPropServices *iface, HMENU hmenu, DWORD child,
    MSAAPROPID prop, LPCWSTR str) { return S_OK; }
static HRESULT WINAPI aps_SetHmenuPropServer(IAccPropServices *iface, HMENU hmenu, DWORD child,
    const MSAAPROPID *props, int count, IAccPropServer *server, AnnoScope scope) { return S_OK; }
static HRESULT WINAPI aps_ClearHmenuProps(IAccPropServices *iface, HMENU hmenu, DWORD child,
    const MSAAPROPID *props, int count) { return S_OK; }
static HRESULT WINAPI aps_ComposeHmenuIdentityString(IAccPropServices *iface, HMENU hmenu,
    DWORD child, BYTE **id, DWORD *idlen) { return E_NOTIMPL; }
static HRESULT WINAPI aps_DecomposeHmenuIdentityString(IAccPropServices *iface, const BYTE *id,
    DWORD idlen, HMENU *hmenu, DWORD *child) { return E_NOTIMPL; }

static IAccPropServicesVtbl aps_vtbl = {
    aps_QueryInterface, aps_AddRef, aps_Release,
    aps_SetPropValue, aps_SetPropServer, aps_ClearProps,
    aps_SetHwndProp, aps_SetHwndPropStr, aps_SetHwndPropServer, aps_ClearHwndProps,
    aps_ComposeHwndIdentityString, aps_DecomposeHwndIdentityString,
    aps_SetHmenuProp, aps_SetHmenuPropStr, aps_SetHmenuPropServer, aps_ClearHmenuProps,
    aps_ComposeHmenuIdentityString, aps_DecomposeHmenuIdentityString,
};
static IAccPropServices aps = { &aps_vtbl };

static HRESULT WINAPI cf_QueryInterface(IClassFactory *iface, REFIID riid, void **ppv)
{
    if (IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid, &IID_IClassFactory)) {
        *ppv = iface;
        return S_OK;
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}
static ULONG WINAPI cf_AddRef(IClassFactory *iface) { return 2; }
static ULONG WINAPI cf_Release(IClassFactory *iface) { return 1; }
static HRESULT WINAPI cf_CreateInstance(IClassFactory *iface, IUnknown *outer, REFIID riid, void **ppv)
{
    if (outer) {
        *ppv = NULL;
        return CLASS_E_NOAGGREGATION;
    }
    return aps_QueryInterface(&aps, riid, ppv);
}
static HRESULT WINAPI cf_LockServer(IClassFactory *iface, BOOL lock) { return S_OK; }

static IClassFactoryVtbl cf_vtbl = {
    cf_QueryInterface, cf_AddRef, cf_Release, cf_CreateInstance, cf_LockServer,
};
static IClassFactory cf = { &cf_vtbl };

HRESULT WINAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv)
{
    if (!IsEqualCLSID(rclsid, &CLSID_AccPropServices)) {
        *ppv = NULL;
        return CLASS_E_CLASSNOTAVAILABLE;
    }
    return cf_QueryInterface(&cf, riid, ppv);
}

HRESULT WINAPI DllCanUnloadNow(void)
{
    return S_FALSE;
}
