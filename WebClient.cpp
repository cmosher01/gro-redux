#include "stdafx.h"
#include "WebClient.h"

#include <sstream>
#include <string>
using namespace std;

WebClient::WebClient(WebServer::ContentHandler& ch, thinsock s) : mCH(ch), mSocket(s)
{
	DWORD thid;
	::CreateThread(0,0,th,this,0,&thid);
}

WebClient::~WebClient()
{
}

DWORD WINAPI WebClient::th(LPVOID s)
{
	WebClient* p = (WebClient*)s;
	p->run();
	delete p;
	return ERROR_SUCCESS;
}

static wstring getIfMod(const wstring& req)
{
	wstring s;

	wstring::size_type i = req.find(L"If-Modified-Since:");
	if (i != wstring::npos)
	{
		i += 18;
		wstring::size_type j = req.find(L"GMT",i);
		s = req.substr(i,j-i);
	}

	return s;
}

void WebClient::run()
{
	wstring s, lastmod;
	int c = 200;
	wstring mime;
	try
	{
		wstring req = getRequest();
		wstring ifmod = getIfMod(req);
		s = getPage(req,mime,lastmod,ifmod);
	}
	catch (int code)
	{
		s.erase();
		c = code;
	}
	catch (...)
	{
		s.erase();
		c = 404;
	}

	try
	{
		if (!s.empty()||c>=300)
			sendResponse(s,c,mime,lastmod);
	}
	catch (...)
	{
	}

	try
	{
		close();
	}
	catch (...)
	{
	}
}

wstring WebClient::getRequest()
{
	wstring str;

	::Sleep(10);

	int sane(64);
	while (ready() && sane--)
		str += read();
//	::OutputDebugString(L"-------------------------------------------------------------\n");
//	::OutputDebugString(str.c_str());
//	::OutputDebugString(L"-------------------------------------------------------------\n");
	return str;
}

wstring WebClient::read()
{
	TCHAR acReadBuffer[1024];
	int n = mSocket.recv(acReadBuffer,1024,0);
	return wstring(acReadBuffer,n);
}

bool WebClient::ready()
{
	u_long n(0);
	mSocket.ioctlsocket(FIONREAD,&n);
	return !!n;
}

wstring WebClient::getPage(const wstring& request, wstring& mime, wstring& lastmod, const wstring& ifmod)
{
	wstring localifmod(ifmod);
	wstring s;
	{
		wistringstream iss(request);

		//make sure it is a GET
		iss >> s;
		if (s!=L"GET")
		{
			::OutputDebugString(CString(L"received ")+s.c_str());
			throw 404;
		}

		iss >> s; // this is the URL of the page that the user requested

/*
		wstring ua;
		wstring ref;
		wstring slin;
		bool eof(false);
		while (!eof)
		{
			TCHAR buf[1024];
			iss.getline(buf,1024);
			slin = buf;
			eof = slin.size()==0;
			if (!eof)
			{
				if (slin.substr(0,11).compare(L"User-Agent:")==0)
					ua = slin;
				else if (slin.substr(0,8).compare(L"Referer:")==0)
					ref = slin;			}
		}
		if (!ua.empty())
		{
			ua = ua.substr(11);
			ua = ua.erase(0,ua.find_first_not_of(L" "));
			if (ua.substr(0,7).compare(L"contype")==0)
				return mCH.getContype(s,mime,mSocket,lastmod,localifmod);
		}
		if (!ref.empty())
		{
			if (localifmod.empty() && ua.find(L"Gecko")==wstring::npos)
				//kludge to catch double requests from Adobe SVG in IE
				localifmod = L"ignore this request";
		}
*/
	}
	return mCH.getHTML(s,mime,mSocket,lastmod,localifmod);
}

//same as endl, but puts cr-lf, instead of just lf
static inline basic_ostream<wchar_t, char_traits<wchar_t> >& __cdecl endl2(basic_ostream<wchar_t, char_traits<wchar_t> >& _O)
{
	_O.put(L'\r');
	_O.put(L'\n');
	_O.flush();
	return (_O);
}

void WebClient::sendResponse(const wstring& page, int code, const wstring& mime, const wstring& lastmod)
{
	wostringstream os;

	if (code==200)
	{
		os << L"HTTP/1.0 200 OK" << endl2;
		os << L"Content-Length: " << page.length() << endl2;
		if (!mime.empty())
			os << L"Content-Type: " << mime << endl2;
		if (!lastmod.empty())
			os << L"Last-Modified: " << lastmod << endl2;
	}
	else if (code==404)
		os << L"HTTP/1.0 404 Not Found" << endl2;
	else if (code==304)
		os << L"HTTP/1.0 304 Not Modified" << endl2;

	os << L"Connection: close" << endl2;
	os << endl2;

	os << page;

	wstring& resp = os.str();

	mSocket.send(resp.c_str(),resp.length(),0);
}

void WebClient::close()
{
	mSocket.shutdown();
	getRequest(); // throw away any unread data
	mSocket.close();
}
