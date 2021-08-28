#ifndef FCGICLIENT_H
#define FCGICLIENT_H

#include <string>
#include <map>
#include "ComminucationUnixClient.h"
#include "IError.h"
#include "Request.h"
#include "Response.h"
#include "HttpConfig.h"


namespace WebCpp
{

class FcgiClient : public IError
{
public:
    enum class FcgiParam
    {
        QUERY_STRING,
        REQUEST_METHOD,
        PATH_INFO,
        CONTENT_TYPE,
        CONTENT_LENGTH,
        SCRIPT_FILENAME,
        SCRIPT_NAME,
        REQUEST_URI,
        DOCUMENT_URI,
        DOCUMENT_ROOT,
        SERVER_PROTOCOL,
        GATEWAY_INTERFACE,
        REMOTE_ADDR,
        REMOTE_PORT,
        SERVER_ADDR,
        SERVER_PORT,
        SERVER_NAME,
    };

    FcgiClient(const std::string &address, const HttpConfig& config);
    bool Init();
    bool Connect();
    void SetKeepConnection(bool keepConnection);
    bool GetKeepConnection();
    void SetParam(FcgiParam param, std::string name);
    bool SendRequest(const Request& request);
    std::string GetParam(FcgiParam param, const HttpHeader &header, const HttpConfig &config) const;

protected:
#pragma pack(push, 1)

    enum class RequestType
    {
        FCGI_BEGIN_REQUEST     = 1,
        FCGI_ABORT_REQUEST     = 2,
        FCGI_END_REQUEST       = 3,
        FCGI_PARAMS            = 4,
        FCGI_STDIN             = 5,
        FCGI_STDOUT            = 6,
        FCGI_STDERR            = 7,
        FCGI_DATA              = 8,
        FCGI_GET_VALUES        = 9,
        FCGI_GET_VALUES_RESULT = 10,
        FCGI_UNKNOWN_TYPE      = 11,
    };

    enum class RequestRole
    {
        FCGI_RESPONDER         = 1,
        FCGI_AUTHORIZER        = 2,
        FCGI_FILTER            = 3,
    };

    enum class ProtocolStatus
    {
        FCGI_REQUEST_COMPLETE   = 0,
        FCGI_CANT_MPX_CONN      = 1,
        FCGI_OVERLOADED         = 2,
        FCGI_UNKNOWN_ROLE       = 3,
    };

    typedef struct {
        uint8_t version;
        uint8_t type;
        uint8_t requestIdB1;
        uint8_t requestIdB0;
        uint8_t contentLengthB1;
        uint8_t contentLengthB0;
        uint8_t paddingLength;
        uint8_t reserved;
    } FCGI_Header;

    typedef struct {
        uint8_t roleB1;
        uint8_t roleB0;
        uint8_t flags;
        uint8_t reserved[5];
    } FCGI_BeginRequestBody;

    typedef struct {
        FCGI_Header header;
        FCGI_BeginRequestBody body;
    } FCGI_BeginRequestRecord;

    typedef struct {
        uint8_t appStatusB3;
        uint8_t appStatusB2;
        uint8_t appStatusB1;
        uint8_t appStatusB0;
        uint8_t protocolStatus;
        uint8_t reserved[3];
    } FCGI_EndRequestBody;

    typedef struct {
        uint8_t nameLengthB0;
    } FCGI_Name1;

    typedef struct {
        uint8_t nameLengthB3;
        uint8_t nameLengthB2;
        uint8_t nameLengthB1;
        uint8_t nameLengthB0;
    } FCGI_Name2;

    typedef struct {
        uint8_t valueLengthB0;
    } FCGI_Value1;

    typedef struct {
        uint8_t valueLengthB3;
        uint8_t valueLengthB2;
        uint8_t valueLengthB1;
        uint8_t valueLengthB0;
    } FCGI_Value2;
#pragma pack(pop)

    struct ResponseData
    {
        ResponseData(int ID, int connID)
        {
            this->ID = ID;
            this->connID = connID;
        }
        bool IsEmpty() { return ID == (-1) || connID == (-1); }
        int ID;
        int connID;
        ByteArray data;
        static ResponseData DefaultResponseData;
    };

    ByteArray BuildBeginRequestPacket(uint16_t ID) const;
    ByteArray BuildParamPacket(const std::string &name, const std::string &value) const;
    ByteArray BuildParamsPacket(uint16_t ID, const ByteArray &params) const;
    ByteArray BuildStdinPacket(uint16_t ID, const ByteArray &stdinData) const;
    void OnDataReady(ByteArray &data);
    void OnConnectionClosed();
    ByteArray ReadData();
    ResponseData& GetResponseData(int ID);
    void ProcessResponse(int ID);

private:
    std::string m_address;
    HttpConfig m_config;
    ComminucationUnixClient m_connection;
    bool m_keepConnection = true;
    static uint16_t RequestID;
    std::map<FcgiParam, std::string> m_fcgiParams;
    std::vector<ResponseData> m_responseQueue;
};

}

#endif // FCGICLIENT_H
