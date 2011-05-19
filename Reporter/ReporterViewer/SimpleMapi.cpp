#include "SimpleMapi.h"
#include <QString>
#include <QDir>
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

/// Returns true if MAPI support is available
bool Mapi::isAvailable()
{
	Dll dll(L"mapi32.dll");
	return dll != NULL;
}

/// Send a file using MAPI
bool Mapi::send(const QString& fileName)
{
	QByteArray ascii = QDir::toNativeSeparators(fileName).toLocal8Bit();

	MapiFileDesc file = { 0 };
	file.nPosition = -1;
	file.lpszFileName = (char*)"report.pdf";
	file.lpszPathName = ascii.data();

	MapiMessage msg = { 0 };
	msg.lpszSubject = (char*)"Jupiter Reporter";
	msg.nFileCount = 1;
	msg.lpFiles = &file;

	return ::MAPISendMail(0, 0, &msg, MAPI_DIALOG|MAPI_LOGON_UI, 0) == SUCCESS_SUCCESS;
}
