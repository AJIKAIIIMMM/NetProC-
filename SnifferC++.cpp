#pragma comment(lib, "ws2_32.lib")
#include <conio.h>
#include <stdio.h>
#include <winsock2.h>
#include <fstream>

#pragma warning(disable: 4996)

#define MAX_PACKET_SIZE    0x10000
#define SIO_RCVALL         0x98000001

char Buffer[MAX_PACKET_SIZE]; 



typedef struct IPHeader {
	UCHAR   iph_verlen;   
	UCHAR   iph_tos;      
	USHORT  iph_length;   
	USHORT  iph_id;       
	USHORT  iph_offset;   
	UCHAR   iph_ttl;      
	UCHAR   iph_protocol; 
	USHORT  iph_xsum;     
	ULONG   iph_src;      
	ULONG   iph_dest;     
} IPHeader;

char src[10];
char dest[10];
char ds[15];
unsigned short lowbyte;
unsigned short hibyte;

void main()
{
	WSADATA     wsadata;  
	SOCKET      s;        
	char        name[128];
	HOSTENT* phe;       
	SOCKADDR_IN sa;     
	IN_ADDR sa1;        
	unsigned long        flag = 1;  

	
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	s = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
	gethostname(name, sizeof(name));
	phe = gethostbyname(name);
	ZeroMemory(&sa, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = ((struct in_addr*)phe->h_addr_list[0])->s_addr;
	bind(s, (SOCKADDR*)&sa, sizeof(SOCKADDR));

	
	ioctlsocket(s, SIO_RCVALL, &flag);

	
	while (!_kbhit())
	{
		int count;
		count = recv(s, Buffer, sizeof(Buffer), 0);
		std::ofstream out("D:\\LogForSniffer.txt", std::ios::app);
		if (out.is_open()) {
			if (count >= sizeof(IPHeader))
			{
				IPHeader* hdr = (IPHeader*)Buffer;

				strcpy(src, "Пакет: ");
				CharToOem(src, dest);
				out << dest;
				printf(dest);
				printf("From ");
				sa1.s_addr = hdr->iph_src;
				printf(inet_ntoa(sa1));
				out << "From " << inet_ntoa(sa1);
				
				printf(" To ");
				sa1.s_addr = hdr->iph_dest;
				printf(inet_ntoa(sa1));
				out << " To " << inet_ntoa(sa1);
				printf(" Prot: ");
				if (hdr->iph_protocol == IPPROTO_TCP) {
					printf("TCP ");
					out << " Prot: TCP ";
				}
				if (hdr->iph_protocol == IPPROTO_UDP) {
					printf("UDP ");
					out << " Prot: UDP ";
				}
				printf("Size: ");
				lowbyte = hdr->iph_length >> 8;
				hibyte = hdr->iph_length << 8;
				hibyte = hibyte + lowbyte;
				printf("%s", itoa(hibyte, ds, 10));
				out << "Size: " << itoa(hibyte, ds, 10);
				printf(" TTL:%s", itoa(hdr->iph_ttl, ds, 10));
				printf("\n");
				out << "TTL:  " << itoa(hdr->iph_ttl, ds, 10) << std::endl;

			}
		}
	}

	closesocket(s);
	WSACleanup();
}