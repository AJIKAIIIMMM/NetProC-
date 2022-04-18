#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>

extern "C"
{
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
}


const auto buff_size = 1024;


bool print_error(const std::string& msg)
{
    perror(msg.c_str());
    // В OpenSSL есть свои функции для печати ошибок.
    ERR_print_errors_fp(stderr);

    return false;
}


void init_ssl()
{
    // Инициализация библиотеки.
    SSL_load_error_strings();
    SSL_library_init();
}


void cleanup(SSL_CTX* ctx, BIO* bio)
{
    // Уничтожение контекста и контекста ввода/вывода.
    SSL_CTX_free(ctx);
    BIO_free_all(bio);
}


bool secure_connect(const std::string& hostname)
{
    std::stringstream request;
    std::string response;

    auto port_start = hostname.find(':');
    auto host = (port_start == std::string::npos) ? hostname : hostname.substr(0, port_start);

    response.resize(buff_size);

    const SSL_METHOD* method = TLS_server_method();
    if (nullptr == method) return print_error("TLS_client_method...");

    
    SSL_CTX* ctx = SSL_CTX_new(method);
    if (nullptr == ctx) return print_error("SSL_CTX_new...");

    BIO* bio = BIO_new_accept(ctx);
    if (nullptr == bio) return print_error("BIO_new_ssl_connect...");

    SSL* ssl = nullptr;

    if (SSL_CTX_use_certificate_file(ctx, "server.pem", SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }


    if (SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // Подключить канал BIO, SSL сессию, и адрес сервера.
    BIO_get_ssl(bio, &ssl);
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY); /* robustness */
    std::cout << "Connecting to \"" << host << "\"..." << std::endl;
    BIO_set_conn_hostname(bio, host.c_str());
    BIO_set_conn_port(bio, "443");
    // Загрузку хранилища сертификатов надо сделать до соединения.
    if (!SSL_CTX_load_verify_locations(ctx, "/etc/ssl/certs/ca-certificates.crt", "/etc/ssl/certs/"))
        return print_error("SSL_CTX_load_verify_locations...");

    // Подключение...
    if (BIO_do_connect(bio) <= 0)
    {
        cleanup(ctx, bio);
        print_error("BIO_do_connect...");
    }

    long verify_flag = SSL_get_verify_result(ssl);
    if (verify_flag != X509_V_OK)
        std::cerr
        << "##### Certificate verification error ("
        << X509_verify_cert_error_string(verify_flag) << ") but continuing..."
        << std::endl;

    // Загрузить корневую страницу.
    request
        << "GET / HTTP/1.1\x0D\x0A"
        << "Host: " << hostname << "\x0D\x0A"
        << "Connection: Close\x0D\x0A\x0D\x0A";
    BIO_puts(bio, request.str().c_str());

    // Прочитать и вывести HTTP ответ сервера.
    while (true)
    {
        std::fill(response.begin(), response.end(), response.size());
        int n = BIO_read(bio, &response[0], response.size());
        // 0 - конец потока, < 0 - ошибка.
        if (n <= 0) break;
        std::cout << response << std::endl;
    }

    cleanup(ctx, bio);
    return true;
}


int main(int argc, const char* const argv[])
{
    if (argc != 2)
    {
        std::cout
            << "Usage: " << argv[0] << " <host>" << std::endl;
        return EXIT_FAILURE;
    }

    init_ssl();

    std::string hostname = argv[1];
    std::cerr
        << "Trying an HTTPS connection to " << hostname << "..." << std::endl;
    if (!secure_connect(hostname)) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}