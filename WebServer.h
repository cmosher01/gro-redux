#pragma once
#include "thinsock.h"
#include <string>
using namespace std;

class WebServer
{
public:
	class ContentHandler
	{
	public:
		ContentHandler() { }
		virtual ~ContentHandler() { }
		virtual wstring getHTML(const wstring& path, wstring& mime, thinsock& sock, wstring& lastmod, const wstring& ifmod) = 0;
	};

	WebServer(int port, ContentHandler& ch);
	virtual ~WebServer();

	int getPort() { return mPort; }
	void quit();

private:
	ContentHandler& mCH;
	int mPort;
	thinsock mSocket;
	bool mQuit;
	HANDLE mThread;

	static DWORD WINAPI th(LPVOID s);
	void run();
	void SetUpListener();
	void AcceptConnections();
};
