#pragma once
#include <winsock2.h>

class thinsock
{
	SOCKET s;

	void check(int e)
	{
		if (e < 0)
			throw error(::WSAGetLastError());
	}
	void checksock(SOCKET sock)
	{
		if (s==INVALID_SOCKET)
			check(SOCKET_ERROR);
	}

public:
	class error
	{
		int c;
	public:
		error(int e) : c(e) {}
		int code() { return c; }
	};

	thinsock() : s(INVALID_SOCKET)
	{
	}

	thinsock(SOCKET sock) : s(sock)
	{
	}

	thinsock(void* pv) : s((SOCKET)pv)
	{
	}

	thinsock(int af, int type, int protocol) : s(INVALID_SOCKET)
	{
		create(af,type,protocol);
	}

	void create(int af, int type, int protocol)
	{
		s = ::socket(af,type,protocol);
		checksock(s);
	}

	virtual ~thinsock()
	{
	}

	void ioctlsocket(long cmd, u_long* argp)
	{
		check(::ioctlsocket(s,cmd,argp));
	}

	int recv(wchar_t* pbuf, int cbuf, int flags)
	{
		char* pb = new char[cbuf];
		int c = ::recv(s,pb,cbuf,flags);
		check(c);

		::MultiByteToWideChar(CP_UTF8,0,pb,c,pbuf,cbuf);

		delete [] pb;
		return c;
	}
	void shutdown(int how = SD_BOTH)
	{
		check(::shutdown(s,how));
	}

	void close()
	{
		if (s != INVALID_SOCKET)
		{
			check(::closesocket(s));
			s = INVALID_SOCKET;
		}
	}

	void bind(const struct sockaddr_in* name, int namelen)
	{
		check(::bind(s,(sockaddr*)name,namelen));
	}

	void listen(int backlog = SOMAXCONN)
	{
		check(::listen(s,backlog));
	}

	int send(const wchar_t* pbuf, int cbuf, int flags)
	{
		int cb = ::WideCharToMultiByte(CP_UTF8,0,pbuf,cbuf,0,0,0,0);
		if (!cb)
			throw error(-1);

		char* pb = new char[cb];
		int ok = ::WideCharToMultiByte(CP_UTF8,0,pbuf,cbuf,pb,cb,0,0);
		if (!ok)
			throw error(-1);

		int c = ::send(s,pb,cb,flags);
		delete [] pb;

		check(c);
		return c;
	}

	int sendbinary(char* pbuf, int cbuf, int flags)
	{
		int c = ::send(s,pbuf,cbuf,flags);
		check(c);
		return c;
	}

	thinsock accept(struct sockaddr_in* addr, int* addrlen)
	{
		SOCKET sock = ::accept(s,(sockaddr*)addr,addrlen);
		checksock(sock);
		return thinsock(sock);
	}
	operator void*()
	{
		return (void*)s;
	}

	operator SOCKET()
	{
		return s;
	}
};
