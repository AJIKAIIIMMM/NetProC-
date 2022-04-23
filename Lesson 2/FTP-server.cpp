#include <winsock2.h>
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Wsock32.lib")
#pragma warning(disable: 4996)


using namespace std;

char addr[256] = "127.0.0.1";

int init_sock() {
    int len;
    sockaddr_in address;
    int result;
    int s;
    s = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = inet_addr("127.0.0.1");   
    address.sin_port = htons(21);    
    len = sizeof(address);
    result = connect(s, (sockaddr*)&address, len);   
    if (result == -1) {
        perror("oops: client");
        return -1;
    }
    return s;
}

int readServ(int s) {
    int rc;
    fd_set fdr;
    FD_ZERO(&fdr);
    FD_SET(s, &fdr);
    timeval timeout;
    timeout.tv_sec = 1;   
    timeout.tv_usec = 0;
    do {
        char buff[512] = { ' ' };
        recv(s, (char*)buff, 512, 0);   
        cout << buff;
        rc = select(s + 1, &fdr, NULL, NULL, &timeout);    
    } while (rc);    
    return 2;
}

int init_data(int s) {
    send(s, "PASV\r\n", strlen("PASV\r\n"), 0);
    char buff[128];
    recv(s, buff, 128, 0);
    cout << buff; 
    int a, b;
    char* tmp_char;
    tmp_char = strtok(buff, "(");
    tmp_char = strtok(NULL, "(");
    tmp_char = strtok(tmp_char, ")");
    int c, d, e, f;
    sscanf(tmp_char, "%d,%d,%d,%d,%d,%d", &c, &d, &e, &f, &a, &b);
    int len;
    sockaddr_in address;
    int result;
    int port = a * 256 + b;
    int ds;
    ds = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(addr);    
    address.sin_port = htons(port);
    len = sizeof(address);
    result = connect(ds, (sockaddr*)&address, len);
    if (result == -1) {
        perror("oops: client");
        return -1;
    }
    return ds;
}

int login(int s) {
    cout << "Введите имя: "; char name[64]; cin >> name;
    char str[512];
    sprintf(str, "USER %s\r\n", name);
    send(s, str, strlen(str), 0);
    readServ(s);
    cout << "Введите пароль: "; char pass[64]; cin >> pass;
    sprintf(str, "PASS %s\r\n", pass);
    send(s, str, strlen(str), 0);
    readServ(s);
    return 0;
}

int get(char* file, int s, int ds) {
    char str[512];
    sprintf(str, "RETR %s\r\n", file);
    send(s, str, strlen(str), 0);

    char size[512];
    recv(s, size, 512, 0);
    cout << size;

    char* tmp_size;
    tmp_size = strtok(size, "(");
    tmp_size = strtok(NULL, "(");
    tmp_size = strtok(tmp_size, ")");
    tmp_size = strtok(tmp_size, " ");

    int file_size;
    sscanf(tmp_size, "%d", &file_size);
    FILE* f;
    f = fopen(file, "wb");   
    int read = 0;  
    do {
        char buff[2048];  
        int readed = recv(ds, buff, sizeof(buff), 0);   
        fwrite(buff, 1, readed, f);   
        read += readed;  
    } while (read < file_size);
    fclose(f);
    cout << "Готово. Ожидание ответа сервера...\n";
    return 0;
}

int main() {
    int s;
    s = init_sock();
    readServ(s);
    int ds = init_data(s);
    login(s);
    char file[256];
    cout << "Please enter path to the file: ";
    cin >> file;
    get(file, s, ds);
    close(s);  
    return 0;
}