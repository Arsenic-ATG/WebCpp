#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>
#include <map>
#include "ICommunication.h"
#include "common.h"


namespace WebCpp
{

class Response
{
public:
    enum class HeaderType
    {
        Undefined = 0,
        AcceptCH,
        AccessControlAllowOrigin,
        AccessControlAllowCredentials,
        AccessControlExposeHeaders,
        AccessControlMaxAge,
        AccessControlAllowMethods,
        AccessControlAllowHeaders,
        AcceptPatch,
        AcceptRanges,
        Age,
        Allow,
        AltSvc,
        CacheControl,
        Connection,
        ContentDisposition,
        ContentEncoding,
        ContentLanguage,
        ContentLength,
        ContentLocation,
        ContentMD5,
        ContentRange,
        ContentType,
        Date,
        DeltaBase,
        ETag,
        Expires,
        IM,
        LastModified,
        Link,
        Location,
        P3P,
        Pragma,
        PreferenceApplied,
        ProxyAuthenticate,
        PublicKeyPins,
        RetryAfter,
        Server,
        SetCookie,
        StrictTransportSecurity,
        Trailer,
        TransferEncoding,
        Tk,
        Upgrade,
        Vary,
        Via,
        Warning,
        WWWAuthenticate,
        XFrameOptions,
    };

    Response(const std::string &version, int connID);
    void SetHeader(Response::HeaderType header, const std::string &value);
    void SetHeader(const std::string &name, const std::string &value);
    void Write(const ByteArray &data);
    void Write(const std::string &data);
    bool SendFile(const std::string &file);

    void SetResponseCode(uint16_t code, const std::string &phrase);
    uint16_t GetResponseCode() const;

    bool Send(ICommunication *communication);
    static std::string HeaderType2String(Response::HeaderType headerType);
    static Response::HeaderType String2HeaderType(const std::string &str);
    static std::string ResponseCode(int code);
    static std::string Extension2MimeType(const std::string &extension);

protected:
    void InitDefault();
    ByteArray BuildStatusLine() const;
    ByteArray BuildHeaders() const;

private:
    int m_connID;
    std::string m_version;
    std::map<std::string, std::string> m_headers;
    ByteArray m_body;
    uint16_t m_responseCode = 200;
    std::string m_responsePhrase = "";
    std::string m_mimeType = "";
    std::string m_file = "";
};

}

#endif // RESPONSE_H
