#pragma once
#include "WebServer.h"
#include "thinsock.h"

#include <string>
using namespace std;

class WebClient
{
	thinsock mSocket;
	WebServer::ContentHandler& mCH;

public:
	WebClient(WebServer::ContentHandler& ch, thinsock s);
	virtual ~WebClient();

private:
	static DWORD WINAPI th(LPVOID s);
	void run();
	void close();
	wstring getRequest();
	wstring getPage(const wstring& request, wstring& mime, wstring& lastmod, const wstring& ifmod);
	void sendResponse(const wstring& page, int code, const wstring& mime = L"text/html", const wstring& lastmod = L"");
	bool ready();
	wstring read();
};
