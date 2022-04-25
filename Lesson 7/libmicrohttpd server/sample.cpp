#include "WebServer.hpp"
#include <time.h>

class MyController:public DynamicController{
public:
    virtual bool validPath(const char* path, const char* method){
        if(strcmp(path, "/") == 0 && strcmp("GET", method) == 0){
            return true;
        }
        return false;
    }

    virtual void createResponse(struct MHD_Connection * connection,
        const char * url, const char * method, const char * upload_data,
        size_t * upload_data_size, std::stringstream& response){
        
        time_t time_cur;
        time(&time_cur);
        struct tm* time_now = localtime(&time_cur);
        response << "<html><head><title>Hello World from cpp</title></head><body>Hello World at "
            << time_now->tm_hour << ":" << time_now->tm_min << ":" << time_now->tm_sec << "!</body></html>";
    }

};


int main(int argc, char** argv){
    
    MyController myPage;

    WebServer server(8080);
    server.addController(&myPage);
    server.start();
}

