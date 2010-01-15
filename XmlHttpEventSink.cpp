#include "XmlHttpEventSink.h"
#include "XmlHttpRequestEvents.h"

#include <windows.h>

///////////////////////////////////////////////////////////////////////////////

STDMETHODIMP XmlHttpEventSink::QueryInterface(REFIID riid, void** ppvObject)
{
    if(ppvObject == NULL)
    {
        return E_INVALIDARG;
    }

    *ppvObject = NULL;

    if( riid == IID_IUnknown )
    {
        *ppvObject = static_cast<IUnknown*>(this);
    }
    else if( riid == IID_IDispatch )
    {
        *ppvObject = static_cast<IDispatch*>(this);
    }

    if( *ppvObject == NULL )
    {
        return E_NOINTERFACE;
    }

    AddRef();

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////

STDMETHODIMP_(ULONG) XmlHttpEventSink::AddRef()
{
    return InterlockedIncrement(&_refCount);
}

///////////////////////////////////////////////////////////////////////////////

STDMETHODIMP_(ULONG) XmlHttpEventSink::Release()
{
    LONG refCount = InterlockedDecrement(&_refCount);
    if(refCount == 0)
    {
        delete this;
        return 0;
    }
    else
    {
        return refCount;
    }
}

///////////////////////////////////////////////////////////////////////////////

STDMETHODIMP XmlHttpEventSink::GetTypeInfoCount(UINT* /*pctinfo*/)
{
    return E_NOTIMPL;
}

///////////////////////////////////////////////////////////////////////////////

STDMETHODIMP XmlHttpEventSink::GetTypeInfo(UINT /*iTInfo*/, LCID /*lcid*/, ITypeInfo** /*ppTInfo*/)
{
    return E_NOTIMPL;
}

///////////////////////////////////////////////////////////////////////////////

STDMETHODIMP XmlHttpEventSink::GetIDsOfNames(REFIID /*riid*/, LPOLESTR* /*rgszNames*/, UINT /*cNames*/, LCID /*lcid*/, DISPID* /*rgDispId*/)
{
    return E_NOTIMPL;
}

///////////////////////////////////////////////////////////////////////////////

STDMETHODIMP XmlHttpEventSink::Invoke(DISPID /*dispIdMember*/, REFIID /*riid*/, LCID /*lcid*/, WORD /*wFlags*/, DISPPARAMS* /*pDispParams*/, VARIANT* /*pVarResult*/, EXCEPINFO* /*pExcepInfo*/, UINT* /*puArgErr*/)
{
    long state;
    // Retrieve the state
    _request->get_readyState(&state);

    if(state == 4)
    {
        long status;
        _request->get_status(&status);

        if(status == 200)
        {
            // Get the response body if we were successful.
            _bstr_t responseText;
            _request->get_responseText(responseText.GetAddress());

            _xmlHttpRequestEvents.onHttpRequestResponse(static_cast<const char*>(responseText));
        }
        else if(status >= 400)
        {
            _bstr_t statusText;
            _request->get_statusText(statusText.GetAddress());

            _xmlHttpRequestEvents.onHttpRequestFailed(static_cast<const char*>(statusText));
        }
    }
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
