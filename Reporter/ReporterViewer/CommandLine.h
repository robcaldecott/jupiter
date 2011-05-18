#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <QStringList>
#include <QString>

/// Class used to parse command-line arguments
class CommandLine
{
public:
    CommandLine(const QStringList& args);

    QString fileName() const;
    bool print() const;
    QString printer() const;
    bool printDefault() const;

private:
    void processArgs(const QStringList& args);

    QString fileName_;
    bool print_;
    QString printer_;
    bool printDefault_;
};

#endif // COMMANDLINE_H
