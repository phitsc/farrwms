#pragma once

#import <msxml6.dll> named_guids

const CLSID DOM_DOCUMENT_CLASS_ID_60 = MSXML2::CLSID_DOMDocument60;
const CLSID DOM_DOCUMENT_CLASS_ID_40 = MSXML2::CLSID_DOMDocument40;
const CLSID DOM_DOCUMENT_CLASS_ID_ANY = MSXML2::CLSID_DOMDocument;

struct MSXMLHelper
{
    static MSXML2::IXMLDOMDocument2Ptr createDomDocument()
    {
        MSXML2::IXMLDOMDocument2Ptr document;
	    HRESULT hr = document.CreateInstance(DOM_DOCUMENT_CLASS_ID_60);
        if(FAILED(hr))
        {
            hr = document.CreateInstance(DOM_DOCUMENT_CLASS_ID_40);
            if(FAILED(hr))
            {
                document.CreateInstance(DOM_DOCUMENT_CLASS_ID_ANY);
            }
        }

        return document;
    }
};