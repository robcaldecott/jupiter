#include "Rc4.h"
#include <QByteArray>

QByteArray Rc4::process(const QByteArray& bytes, const QByteArray& key)
{
	Rc4 rc(key);

	QByteArray ret;
	for (int i = 0, len = bytes.length(); i < len; ++i)
	{
		ret += bytes[i] ^ rc.output();
	}

	return ret;
}

Rc4::Rc4(const QByteArray& key)
{
	for (i = 0; i < 256; i++)
	{
		S[i] = i;
	}

	const int keyLength = key.length();
	for (i = j = 0; i < 256; i++)
	{
		j = (j + key[i % keyLength] + S[i]) & 255;
		swap(S, i, j);
	}

	i = j = 0;
}

unsigned char Rc4::output()
{
	i = (i + 1) & 255;
	j = (j + S[i]) & 255;

	swap(S, i, j);

	return S[(S[i] + S[j]) & 255];
}

void Rc4::swap(unsigned char *s, unsigned int i, unsigned int j) const
{
	unsigned char temp = s[i];
	s[i] = s[j];
	s[j] = temp;
}
