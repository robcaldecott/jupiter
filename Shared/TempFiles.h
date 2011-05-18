#ifndef TEMPFILES_H
#define TEMPFILES_H

#include <QStringList>

class TempFiles
{
public:
    TempFiles();
    ~TempFiles();

    void add(const QString& fileName);

private:
    QStringList files;
};

#endif // TEMPFILES_H
