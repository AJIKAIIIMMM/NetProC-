#ifndef _WEBSERVER_
#define _WEBSERVER_

#include <microhttpd.h>
#include <iostream>
#include <string.h>
#include <vector>

#include "Controller.hpp"

class WebServer{
private:
    int port;
    struct MHD_Daemon* daemon;

    /** List of controllers this server has. */
    std::vector<Controller*> controllers;

    static int request_handler(void * cls, struct MHD_Connection * connection,
        const char * url, const char * method, const char * version,
        const char * upload_data, size_t * upload_data_size, void ** ptr);
public:
    WebServer(int p);

    void addController(Controller* controller);
    int start();
};
#endif
