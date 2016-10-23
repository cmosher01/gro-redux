#include "stdafx.h"
#include "WebServer.h"

#include "WebClient.h"
#include "thinsock.h"

#include <string>
using namespace std;



WebServer::WebServer(int port, ContentHandler& ch) : mPort(port), mCH(ch), mQuit(true), mSocket(INVALID_SOCKET)
{
	DWORD thid;
	mThread = ::CreateThread(0,0,th,this,0,&thid);
}

WebServer::~WebServer()
{
}

DWORD WINAPI WebServer::th(LPVOID s)
{
	WebServer* p = (WebServer*)s;
	p->run();
	return ERROR_SUCCESS;
}

void WebServer::run()
{
	try
	{
		WSAData wsaData;
		int nCode = ::WSAStartup(MAKEWORD(2,0),&wsaData);
		if (nCode != 0)
			throw nCode;

		mPort = ::htons(mPort);

		SetUpListener();
		AcceptConnections();
	}
	catch (...)
	{
	}

	try
	{
		mSocket.shutdown();
	}
	catch (...)
	{
	}

	try
	{
		mSocket.close();
	}
	catch (...)
	{
	}

	(void)::WSACleanup();
}

void WebServer::SetUpListener()
{
	u_long nInterfaceAddr = ::inet_addr("0.0.0.0");
	if (nInterfaceAddr == INADDR_NONE)
		throw ::WSAGetLastError();

	mSocket.create(AF_INET,SOCK_STREAM,0);

	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = nInterfaceAddr;
	sin.sin_port = mPort;

	mSocket.bind(&sin,sizeof(sockaddr_in));

	mSocket.listen();
}

void WebServer::AcceptConnections()
{
	sockaddr_in sinRemote;
	int nAddrSize = sizeof(sinRemote);

	mQuit = false;
	while (!mQuit)
	{
		thinsock s = mSocket.accept(&sinRemote,&nAddrSize);
		new WebClient(mCH,s);
	}
}

void WebServer::quit()
{
	if (mQuit)
		return;

	mQuit = true;

	try
	{
		mSocket.close();
	}
	catch (...)
	{
	}

	::WaitForSingleObject(mThread,10000);
}
