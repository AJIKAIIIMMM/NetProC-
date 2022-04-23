#pragma once
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsock2.h>
#include <mstcpip.h>
#include <lm.h>

#include "resource.h"

#pragma comment(lib, "ws2_32.lib")

#pragma comment(lib, "netapi32.lib")

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
}IPHeader;

BOOL AreWeAlone(LPSTR szName);
BOOL IsCurrentUserAdmin();
INT_PTR CALLBACK AboutDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

#define WM_RECV (WM_USER + 1)

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
