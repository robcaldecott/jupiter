#include "CommandLine.h"

CommandLine::CommandLine(const QStringList& args) :
    print_(false),
    printDefault_(false)
{
    processArgs(args);
}

QString CommandLine::fileName() const
{
    return fileName_;
}

bool CommandLine::print() const
{
    return print_;
}

QString CommandLine::printer() const
{
    return printer_;
}

bool CommandLine::printDefault() const
{
    return printDefault_;
}

void CommandLine::processArgs(const QStringList& args)
{
    if (args.count() > 1)
    {
	// Filename must be the second parameter
	fileName_ = args.value(1);
	// Process remaining params
	for (int i = 2; i < args.count(); ++i)
	{
	    const QString arg = args.at(i).toLower();
	    if (arg == "-print" || arg == "/print" || arg.startsWith("/print=") || arg.startsWith("-print="))
	    {
		print_ = true;

		// Extract printer name
		printer_ = args.at(i).mid(arg.indexOf('=') + 1);
		printer_.remove('"');
	    }
	    else if (arg == "-printdefault" || arg == "/printdefault")
	    {
		printDefault_ = true;
	    }
	}
    }
}
