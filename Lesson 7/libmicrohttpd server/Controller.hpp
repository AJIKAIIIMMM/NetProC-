#pragma once
#include <sstream>

/**
 * Base controller for handling http requests.
 */
class Controller{

public:
    /**
     * Check if given path and method are handled by this controller.
     */
    virtual bool validPath(const char* path, const char* method) = 0;

    /**
     * Handles given request.
     */
    virtual int handleRequest(struct MHD_Connection* connection,
        const char* url, const char* method, const char* upload_data,
        size_t* upload_data_size) = 0;

};

/**
 * The dynamic controller is a controller for creating user defined pages.
 */
class DynamicController:public Controller{
public:
    virtual bool validPath(const char* path, const char* method) = 0;

    /**
     * User defined http response.
     */
    virtual void createResponse(struct MHD_Connection* connection,
        const char* url, const char* method, const char* upload_data,
        size_t* upload_data_size, std::stringstream& response) = 0;

    virtual int handleRequest(struct MHD_Connection* connection,
        const char* url, const char* method, const char* upload_data,
        size_t* upload_data_size){

        std::stringstream response_string;
        createResponse(connection, url, method, upload_data, upload_data_size, response_string);

        //Send response.
        struct MHD_Response * response = MHD_create_response_from_buffer(strlen(response_string.str().c_str()),
            (void*)response_string.str().c_str(), MHD_RESPMEM_MUST_COPY);
        int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);
        
        return ret;
    }
};

