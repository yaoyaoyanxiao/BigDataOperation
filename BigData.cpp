#include "BigData.h"
#include <cassert>

BigData::BigData(INT64 data)
	: m_llValue(data)
	, m_strData("")
{
	INT64ToString();
}

int idx = 0;

BigData::BigData(const char *_pdata)
{
	assert(_pdata);
	char csymbol = _pdata[0];
	char* pdata = (char*)_pdata;

	if (csymbol == '+' || csymbol == '-')
	{
		pdata++;
	}
	else if (*pdata >= '0'&& *pdata <= '9')
	{
		csymbol = '+';
	}
	else
	{
		m_llValue = 0;
		m_strData = '0';
		return;
	}

	while (*pdata == '0')
	{
		pdata++;
	}

	m_strData.resize(strlen(pdata) + 1);
	m_llValue = 0;
	m_strData[0] = csymbol;
	int count = 1;

	while (pdata)
	{
		if (*pdata >= '0' && *pdata <= '9')
		{
			m_llValue = m_llValue * 10 + *pdata - '0';
			m_strData[count++] = *pdata++;
		}
		else
		{
			break;
		}
	}
	
	if ('-' == csymbol)
	{
		m_llValue = 0 - m_llValue;
	}
}

BigData BigData::operator+(BigData& bigData)
{
	if (!IsINT64Overflow() && !bigData.IsINT64Overflow())
	{
		if (m_strData[0] != bigData.m_strData[0])//两个数异号
		{
			return BigData(m_llValue + bigData.m_llValue);
		}
		else
		{
			if ((m_strData[0] == '+' && MAX_INT64 - m_llValue >= bigData.m_llValue
				|| (m_strData[0]) == '-' && MIN_INT64 - m_llValue <= bigData.m_llValue))
			{
				return BigData(m_llValue + bigData.m_llValue);
			}
		}
	}

	std::string strret;
	if (m_strData[0] == bigData.m_strData[0])
	{
		strret = Add(m_strData, bigData.m_strData);
	}
	else
	{
		strret = Sub(m_strData, bigData.m_strData);
	}

	return BigData(strret.c_str());
}

BigData BigData::operator-(const BigData& bigData)
{
	if (!IsINT64Overflow() && !bigData.IsINT64Overflow())
	{
		if (m_strData[0] == bigData.m_strData[0])
		{
			return BigData(m_llValue - bigData.m_llValue);
		}
		else
		{
			if ((m_strData[0] == '+' && MAX_INT64 + bigData.m_llValue >= m_llValue)
				|| (m_strData[0] == '-' && MIN_INT64 + bigData.m_llValue <= m_llValue))
			{
				return BigData(m_llValue - bigData.m_llValue);
			}
		}
	}

	std::string strret;
	if (m_strData[0] != bigData.m_strData[0])
	{
		strret = Add(m_strData, bigData.m_strData);
	}
	else
	{
		strret = Sub(m_strData, bigData.m_strData);
	}

	return BigData(strret.c_str());
}

BigData BigData::operator*(const BigData& bigData)
{
	if (m_llValue == 0 || bigData.m_llValue == 0)
	{
		return BigData(INT64(0));
	}
	if (!IsINT64Overflow() && !bigData.IsINT64Overflow())
	{
		if (m_strData[0] == bigData.m_strData[0])
		{
			if ((m_strData[0] == '+' && MAX_INT64 / m_llValue >= bigData.m_llValue)
				|| (m_strData[0] == '-' && MAX_INT64 / m_llValue <= bigData.m_llValue))
			{
				return BigData(m_llValue * bigData.m_llValue);
			}
		}
		else
		{
			if ((m_strData[0] == '+' && MIN_INT64 / m_llValue <= bigData.m_llValue)
				|| (m_strData[0] == '-' && MIN_INT64 / m_llValue >= bigData.m_llValue))
			{
				return BigData(m_llValue * bigData.m_llValue);
			}
		}
	}

	return BigData(Mul(m_strData, bigData.m_strData).c_str());
}

BigData BigData::operator/(const BigData& bigData)
{
	if (bigData.m_llValue == 0)
	{
		assert("除数不能为0!");
		return BigData(INT64(0));
	}

	if (!IsINT64Overflow() && bigData.IsINT64Overflow())
	{
		return BigData(m_llValue / bigData.m_llValue);
	}

	return BigData(Div(m_strData, bigData.m_strData).c_str());
}

std::string BigData::Add(std::string left, std::string right)
{
	int lsize = left.size();
	int rsize = right.size();
	char ch = left[0];

	if (lsize < rsize)
	{
		std::swap(left, right);
		std::swap(lsize, rsize);
	}

	std::string strret;
	strret.resize(lsize + 1);
	strret[0] = ch;
	char cstep = 0;
	int n = 1;

	while (n < lsize)
	{
		char cret = left[lsize - n] - '0' + cstep;//cstep进位
		if (n < rsize)
		{
			cret += right[rsize - n] - '0';
		}
		strret[lsize - n + 1] = (cret % 10 + '0');
		cstep = cret / 10;
		n++;
	}

	strret[1] = cstep + '0';

	return strret;
}

std::string BigData::Sub(std::string left, std::string right)
{
	int lsize = left.size();
	int rsize = right.size();
	char csymbol = left[0];

	if ((lsize < rsize) ||(lsize == rsize && left < right))
	{
		std::swap(left, right);
		std::swap(lsize, rsize);

		if (csymbol == '+')
		{
			csymbol = '-';
		}
		else
		{
			csymbol = '+';
		}
	}

	std::string strret;
	strret.resize(lsize);
	strret[0] = csymbol;
	int n = 1;

	while (n < lsize)
	{
		char cret = left[lsize - n] - '0';
		if (n < rsize)
		{
			cret -= right[rsize - n] - '0';
		}
		if (cret < 0)
		{
			left[lsize - n - 1] -= 1;
			cret += 10;
		}
		strret[lsize - n] = (cret + '0');
		n++;
	}

	return strret;
}

std::string BigData::Mul(std::string left, std::string right)
{
	int lsize = left.size();
	int rsize = right.size();

	if (lsize > rsize)
	{
		swap(left, right);
		swap(lsize, rsize);
	}

	char csymbol = '+';
	if (left[0] != right[0])
	{
		csymbol = '-';
	}

	string strret;
	strret.assign(lsize + rsize - 1, '0');
	strret[0] = csymbol;
	int datalen = strret.size();
	int ioffset = 0;

	for (int lidx = 1; lidx < lsize; lidx++)
	{
		char cleft = left[lsize - lidx] - '0';
		char cstep = 0;

		if (cleft == 0)
		{
			ioffset++;
			continue;
		}

		for (int ridx = 1; ridx < rsize; ridx++)
		{
			char cret = cleft*(right[rsize - ridx] - '0');
			cret += cstep;
			cret += (strret[datalen - ioffset - ridx] - '0');
			strret[datalen - ioffset - ridx] = (cret % 10 + '0');
			cstep = cret / 10;
		}

		strret[datalen - ioffset - rsize] += cstep;
		ioffset++;
	}

	return strret;
}

std::string BigData::Div(std::string left, std::string right)
{
	char lsize = left.size();
	char rsize = right.size();
	char csymbol = '+';

	if (left[0] != right[0])
	{
		csymbol = '-';
	}

	if ((lsize < rsize) ||(lsize == rsize && strcmp(left.c_str() + 1, right.c_str() + 1) < 0))
	{
		return "0";
	}
	else
	{
		if (right == "-1" || right == "+1")
		{
			left[0] = csymbol;
			return left;
		}
	}

	string strret;
	strret.append(1, csymbol);
	char *pleft = (char*)(left.c_str() + 1);
	char *pright = (char*)(right.c_str() + 1);
	int datalen = 1;
	lsize -= 1;

	for (idx = 0; idx < lsize;)
	{
		if (*pleft == '0')
		{
			strret.append(1, '0');
			pleft++;
			idx++;
			continue;
		}

		if (!IsLeftStrBig(pleft, datalen, pright, rsize - 1))
		{
			strret.append(1, '0');
			datalen++;
			if (idx + datalen > lsize)
			{
				break;
			}
			continue;
		}
		else
		{
			strret.append(1, SubLoop(pleft, datalen, pright, rsize - 1));
			datalen++;
		}
	}

	return strret;
}

void BigData::INT64ToString()
{
	char csymbol = '+';
	INT64 tmp = m_llValue;

	if (tmp < 0)
	{
		csymbol = '-';
		tmp = 0 - tmp;
	}
	m_strData.append(1, csymbol);

	while (tmp)
	{
		m_strData.append(1, tmp % 10 + '0');
		tmp /= 10;
	}

	char *left = (char*)(m_strData.c_str() + 1);
	char *right = (char*)(m_strData.c_str() + m_strData.size() - 1);

	while (left < right)
	{
		char ctmp = *left;
		*left++ = *right;
		*right-- = ctmp;
	}
}

bool BigData::IsINT64Overflow()const
{
	std::string tmp;

	if ('+' == m_strData[0])
	{
		tmp = "+9223372036854775807";
	}
	else
	{
		tmp = "-9223372036854775808";
	}
	if ((m_strData.size() > tmp.size()) ||
		((m_strData.size() == tmp.size()) && (m_strData > tmp)))
	{
		return true;
	}

	return false;
}

bool BigData::IsLeftStrBig(char *pLeft, size_t LSize, char *pRight, size_t RSize)
{
	assert(pLeft != NULL && pRight != NULL);

	if ((LSize > RSize) || (LSize == RSize && strncmp(pLeft, pRight, LSize) >= 0))
	{
		return true;
	}

	return false;
}

char BigData::SubLoop(char *&pLeft, int& LSize, char *pRight, size_t RSize)
{
	assert(pLeft != NULL && pRight != NULL);
	char cret = '0';

	while (true)
	{
		if (!IsLeftStrBig(pLeft, LSize, pRight, RSize))
		{
			break;
		}

		int ldatalen = LSize - 1;
		int rdatalen = RSize - 1;

		while (ldatalen >= 0 && rdatalen >= 0)
		{
			if (pLeft[ldatalen] < pRight[rdatalen])
			{
				pLeft[ldatalen - 1] -= 1;
				pLeft[ldatalen] += 10;
			}
			pLeft[ldatalen] = pLeft[ldatalen] - pRight[rdatalen] + '0';
			ldatalen--;
			rdatalen--;
		}

		while ((*pLeft == '0') && (LSize > 0))
		{
			pLeft++;
			LSize--;
			idx++;
		}
		cret++;
	}

	return cret;
}

std::ostream& operator<<(std::ostream& _cout, const BigData& bigData)
{
	if (!bigData.IsINT64Overflow()) // 没有溢出
	{
		_cout << bigData.m_llValue;
	}
	else
	{
		char* pData = (char*)bigData.m_strData.c_str();
		if (pData[0] == '+')
		{
			pData++;
		}
		_cout << pData;
	}

	return _cout;
}

void Test()
{
	BigData bigdata1("222222222222222222222222222222222222222222222");
	BigData bigdata2("1111");

	//测试加法
	cout << "测试加法：";
	cout << bigdata1 + bigdata2<<endl;

	//测试减法
	cout << "测试减法：";
	cout << bigdata1 - bigdata2<<endl;

	//测试乘法
	cout << "测试乘法：";
	cout << bigdata1 * bigdata2<<endl;

	//测试除法
	cout << "测试除法：";
	cout << bigdata1 / bigdata2<<endl;
}

int main()
{
	Test();

	system("pause");
	return 0;
}
