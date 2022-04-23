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

    const std::string fileName = "some.txt";
    std::ofstream outFile{ fileName };
    outFile
        << std::string(boost::asio::buffers_begin(buf.data()),
            boost::asio::buffers_end(buf.data()));

    return 0;
}