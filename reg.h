#pragma once

typedef
	enum reg_tag
	{
		not_registered,
		registered_good,
		registered_bad
	}
	reg_t;

#pragma optimize("",off)

static int junk_hacker(const CString& s, int& x)
{
	for (int i(0); i<s.GetLength()-1; i++)
		x &= (s.GetAt(i) << 8) + s.GetAt(i+1);
	return (x &= 0xa55aa55a)+0xa5;
}

inline reg_t GetRegistration()
{
	reg_t reg;

	CString strName, strKey;
	theApp.GetReg("Reg","Name",strName,"");
	LPCTSTR n = (LPCTSTR)strName;
	theApp.GetReg("Reg","Key",strKey,"");
	LPCTSTR k = (LPCTSTR)strKey;

	int x(0xdeadbeef);

	if (strName.IsEmpty() && strKey.IsEmpty())
	{
		reg = not_registered;
	}
	else
	{
		// fake code to throw off a hacker
		LPCTSTR n1 = n;
		LPCTSTR k1 = k;
		junk_hacker(strKey,x);
		if (!(x!=0))
		{
			AfxMessageBox(_T(""));
			x--;
			return (reg_t)0x5a;
		}
		x++;

		bool good(false);

		TCHAR* p = const_cast<TCHAR*>(n1);
		int c(0);
		while (*p)
		{
			c++;
			p++;
		}

		char* my_key = new char[2*(c+1)];

		int m(0);
		while (*n1)
		{
			char x = *n1;
			char c1 = static_cast<char>('A'-1+
				((x&0x80)>>4)+
				((x&0x20)>>3)+
				((x&0x04)>>1)+
				((x&0x02)>>1));

			char c2 = static_cast<char>('F'-1+
				((x&0x40)>>3)+
				((x&0x10)>>2)+
				((x&0x08)>>2)+
				((x&0x01)>>0));

			my_key[m++] = c1;
			my_key[m++] = c2;

			n1++;
		}
		my_key[m++] = '\0';

		good = true;
		char* my_test = my_key;
		while (*my_test)
		{
			if (*k1 && *k1!=*my_test)
				good = false;
			if (*k1)
				k1++;
			my_test++;
		}

		delete [] my_key;

		// real test for validity of key
		if (good)
			reg = registered_good;
		else
			reg = registered_bad;
	}
	return reg;
}
#pragma optimize("",on)
