#include <signal.h>
#include "HttpServer.h"
#include "Request.h"
#include <string>

static WebCpp::HttpServer server;

void handle_sigint(int)
{
    server.Close();
}

int main()
{
    signal(SIGINT, handle_sigint);

    std::string arr { 'a','b', 'c' };
    std::string del;

    WebCpp::HttpConfig config;
    config.SetRoot("/home/ruslan/source/webcpp/test/public");
    config.SetProtocol("HTTPS");
    config.SetServerPort(8888);
    config.SetSslSertificate("/home/ruslan/source/webcpp/test/ssl/server.cert");
    config.SetSslKey("/home/ruslan/source/webcpp/test/ssl/server.key");

    if(server.Init(config))
    {
        server.Get("/form", [](const WebCpp::Request &, WebCpp::Response &response) -> bool
        {
            bool retval = false;

            retval = response.AddFile("form.html");

            if(retval == false)
            {
                response.SendNotFound();
            }

            return retval;
        });

        server.Post("/form", [](const WebCpp::Request &request, WebCpp::Response &response) -> bool
        {
            bool retval = true;

            auto body = request.GetRequestBody();
            auto file1 = body.GetValue("file1");

            response.SetHeader("Content-Type","text/html;charset=utf-8");
            response.Write("<div>Hello, " + body.GetValue("name").GetDataString() + " " + body.GetValue("surname").GetDataString() + "</div>");
            response.Write("<div>file '" + file1.fileName + "'" + " with length: " + std::to_string(file1.data.size()) + " and mimetype: '" + file1.contentType + "' was successfully uploaded</div>");

            return retval;
        });


        server.Get("/[{file}]", [](const WebCpp::Request &request, WebCpp::Response &response) -> bool
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

        server.Get("/(user|users)/{user:alpha}/[{action:alpha}/]", [](const WebCpp::Request &request, WebCpp::Response &response) -> bool
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

        server.Run();
        return 0;
    }

    return 1;
}
