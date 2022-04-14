#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <fstream>

int main()
{
    boost::asio::streambuf buf;

    try
    {
        boost::asio::io_service io_service;

        boost::asio::ip::tcp::socket s{ io_service };
        boost::asio::ip::tcp::resolver resolver{ io_service };
        boost::asio::connect(s, resolver.resolve({ "localhost", "9999" }));
        boost::asio::read(s, buf);
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    CURL* curl;
    CURLcode res;

    static const char* postthis = "moo mooo moo moo";

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis);

        /* if we do not provide POSTFIELDSIZE, libcurl will strlen() by
           itself */
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(postthis));

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    CURL* curl1;
    CURLcode res1;

    curl1 = curl_easy_init();
    if (curl1) {
        curl_easy_setopt(curl1, CURLOPT_URL, "https://example.com");
        /* example.com is redirected, so we tell libcurl to follow redirection */
        curl_easy_setopt(curl1, CURLOPT_FOLLOWLOCATION, 1L);

        /* Perform the request, res will get the return code */
        res1 = curl_easy_perform(curl1);
        /* Check for errors */
        if (res1 != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res1));

        /* always cleanup */
        curl_easy_cleanup(curl1);
    }
    const std::string fileName = "some.txt";
    std::ofstream outFile{ fileName };
    outFile
        << std::string(boost::asio::buffers_begin(buf.data()),
            boost::asio::buffers_end(buf.data()));

    return 0;
}