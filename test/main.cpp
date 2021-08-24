#include <signal.h>
#include "common.h"
#include "HttpServer.h"
#include "WebSocketServer.h"
#include "Request.h"
#include <string>
#include <iostream>
#include "StringUtil.h"
#include "Data.h"
#include "ResponseWebSocket.h"


static WebCpp::HttpServer httpServer;
static WebCpp::WebSocketServer wsServer;

void handle_sigint(int)
{
    httpServer.Close(false);
    wsServer.Close(false);
}

int main()
{
    signal(SIGINT, handle_sigint);        

    WebCpp::HttpConfig config;
    config.SetRoot(PUBLIC_DIR);
    config.SetHttpProtocol("HTTP");
    config.SetHttpServerPort(8080);
    config.SetSslSertificate("/home/ruslan/source/webcpp/test/ssl/server.cert");
    config.SetSslKey("/home/ruslan/source/webcpp/test/ssl/server.key");
    config.SetTempFile(true);

    if(httpServer.Init(config))
    {
        httpServer.Get("/ws", [](const WebCpp::Request &, WebCpp::Response &response) -> bool
        {
            bool retval = false;

            retval = response.AddFile("ws.html");

            if(retval == false)
            {
                response.SendNotFound();
            }

            return retval;
        });

        httpServer.Get("/form", [](const WebCpp::Request &, WebCpp::Response &response) -> bool
        {
            bool retval = false;

            retval = response.AddFile("form.html");

            if(retval == false)
            {
                response.SendNotFound();
            }

            return retval;
        });

        httpServer.Post("/form", [](const WebCpp::Request &request, WebCpp::Response &response) -> bool
        {
            bool retval = true;

            auto &body = request.GetRequestBody();
            auto file1 = body.GetValue("file1");
            auto file2 = body.GetValue("file2");

            response.SetHeader("Content-Type","text/html;charset=utf-8");
            response.Write("<div>Hello, " + body.GetValue("name").GetDataString() + " " + body.GetValue("surname").GetDataString() + "</div>");
            response.Write("<div>file 1 '" + file1.fileName + "'" + " with length: " + std::to_string(file1.data.size()) + " and mimetype: '" + file1.contentType + "' was successfully uploaded</div>");
            response.Write("<div>file 2 '" + file2.fileName + "'" + " with length: " + std::to_string(file2.data.size()) + " and mimetype: '" + file2.contentType + "' was successfully uploaded</div>");

            return retval;
        });

        httpServer.Get("/[{file}]", [](const WebCpp::Request &request, WebCpp::Response &response) -> bool
        {
            bool retval = false;
            std::string file = request.GetArg("file");
            if(!file.empty())
            {
                retval = response.AddFile(file);
            }
            else
            {
                retval = response.AddFile("index.html");
            }

            if(retval == false)
            {
                response.SendNotFound();
            }

            return retval;
        });        

        httpServer.Get("/(user|users)/{user:alpha}/[{action:string}/]", [](const WebCpp::Request &request, WebCpp::Response &response) -> bool
        {
            std::string user = request.GetArg("user");
            if(user.empty())
            {
                user = "Unknown";
            }
            std::string action = request.GetArg("action");
            if(action.empty())
            {
                action = "Hello!";
            }

            response.SetHeader("Content-Type","text/html;charset=utf-8");
            response.Write(std::string("<h2>") + user + ", " + action + "</h2>");
            return true;
        });

        if(wsServer.Init(config))
        {
            wsServer.Data([](const WebCpp::HttpHeader &, WebCpp::ResponseWebSocket &response, const ByteArray &data) -> bool {

                std::cout << "received from client: " << StringUtil::ByteArray2String(data) << std::endl;
                response.WriteText("Hello from server!");
                return true;                
            });
        }

        httpServer.Run();
        wsServer.Run();

        httpServer.WaitFor();
        wsServer.WaitFor();

        return 0;
    }

    return 1;
}
