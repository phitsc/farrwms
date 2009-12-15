#pragma once

#include "msxml.h"

///////////////////////////////////////////////////////////////////////////////

struct XmlHttpRequestEvents;

///////////////////////////////////////////////////////////////////////////////

class XmlHttpEventSink : public IDispatch
{
public:
    XmlHttpEventSink(MSXML2::IXMLHTTPRequest* request, XmlHttpRequestEvents& xmlHttpRequestEvents) : 
      _refCount(1), 
      _request(request) ,
      _xmlHttpRequestEvents(xmlHttpRequestEvents)
    {}

    // IUnknown 
    STDMETHOD(QueryInterface)(REFIID riid, void **ppvObject);
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();

    // IDispatch
    STDMETHOD(GetTypeInfoCount)(UINT* pctinfo);
    STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo);
    STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId);
    STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);

private:
    LONG _refCount;
    MSXML2::IXMLHTTPRequest* _request;
    XmlHttpRequestEvents& _xmlHttpRequestEvents;

    void operator=(const XmlHttpEventSink&);
};

///////////////////////////////////////////////////////////////////////////////
