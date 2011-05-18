#ifndef RC4_H
#define RC4_H

class QByteArray;

/// Class that implements the RC4 cipher
/// http://en.wikipedia.org/wiki/RC4
class Rc4
{
public:
    static QByteArray process(const QByteArray& str, const QByteArray& key);

private:
    Rc4(const QByteArray& key);

    void swap(unsigned char *s, unsigned int i, unsigned int j) const;
    unsigned char output();

    unsigned char S[256];
    unsigned int i;
    unsigned int j;
};

#endif // RC4_H
