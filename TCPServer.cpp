#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include <fstream>
#include <iterator>
#include <string>

using boost::asio::ip::tcp;

std::vector<char> fileData;

void session(tcp::socket sock)
{
    try
    {
        boost::asio::write(sock,
            boost::asio::buffer(fileData.data(), fileData.size()));
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception in thread: " << e.what();
    }
}

int main()
{
    {
        const std::string fileName = "some.txt";
        std::ifstream inFile{ fileName, std::ios::binary };
        fileData.insert(std::begin(fileData), std::istreambuf_iterator<char> {
            inFile }, std::istreambuf_iterator<char> { });
    }
    try
    {
        boost::asio::io_service io_service;

        tcp::acceptor a{ io_service, tcp::endpoint { tcp::v4(), 9999 } };
        for (;;)
        {
            tcp::socket sock{ io_service };
            a.accept(sock);
            std::thread(session, std::move(sock)).detach();
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}