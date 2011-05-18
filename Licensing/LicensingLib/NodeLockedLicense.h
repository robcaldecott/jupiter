#ifndef NODELOCKEDLICENSE_H
#define NODELOCKEDLICENSE_H

class QString;
class QDate;

// Functions used to generate and validate a node-locked
// license, using the MAC address(es) of the machine.
namespace NodeLockedLicense
{
    // Generate a license file that can be turned into an activation key
    QString licenseFile(const QString& name, const QString& email, const QDate& expiryDate);
    // Generate an activation key from a license file
    QString activationKey(const QString& license);
    // Returns true if the license is valid
    bool isValid(const QString& name, const QString& email, const QDate& expiryDate, const QString& license);
}

#endif // NODELOCKEDLICENSE_H
