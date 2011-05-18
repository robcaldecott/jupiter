#include "SimpleMapi.h"
#include <QString>
#include <QDir>
#include <QScopedPointer>
#include <windows.h>
#include <mapi.h>

namespace
{
	// Simple DLL wrapper class
	class Dll
	{
	public:
		Dll(const wchar_t* fileName)
		{
			dll = ::LoadLibrary(fileName);
		}

		~Dll()
		{
			if (dll)
			{
				::FreeLibrary(dll);
			}
		}

		operator HINSTANCE() const
		{
			return dll;
		}

	private:
		HINSTANCE dll;
	};
}

struct Mapi::Impl
{
	Dll dll;
	LPMAPISENDMAIL mapiSendMail;

	Impl() : dll(L"mapi32.dll"), mapiSendMail(0)
	{
		if (dll)
		{
			mapiSendMail = (LPMAPISENDMAIL)::GetProcAddress(dll, "MAPISendMail");
		}
	}
};

Mapi::Mapi() : impl(new Impl)
{
}

Mapi::~Mapi()
{
}

Mapi& Mapi::instance()
{
	static QScopedPointer<Mapi> mapi(new Mapi());
	return *mapi;
}

bool Mapi::isAvailable()
{
	return impl->mapiSendMail != 0;
}

bool Mapi::send(const QString& fileName, const QString& subject, const QString& name)
{
	if (!isAvailable())
		return false;

	QByteArray asciiName = name.toLocal8Bit();
	QByteArray asciiFileName = QDir::toNativeSeparators(fileName).toLocal8Bit();

	MapiFileDesc file = { 0 };
	file.nPosition = -1;
	file.lpszFileName = asciiName.data();
	file.lpszPathName = asciiFileName.data();

	QByteArray asciiSubject = subject.toLocal8Bit();
	MapiMessage msg = { 0 };
	msg.lpszSubject = asciiSubject.data();
	msg.nFileCount = 1;
	msg.lpFiles = &file;

	return impl->mapiSendMail(0, 0, &msg, MAPI_DIALOG|MAPI_LOGON_UI, 0) == SUCCESS_SUCCESS;
}
