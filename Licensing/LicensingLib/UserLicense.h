#ifndef USERLICENSE_H
#define USERLICENSE_H

class QString;

// Functions used to generate and validate a user license key
// The license key is tied to the users' name and email address
namespace UserLicense
{
    // Generate an activation key
    QString activationKey(const QString& name, const QString& email);
    // Returns true if an activation key is valid
    bool isValid(const QString& name, const QString& email, const QString& key);
}

#endif // USERLICENSE_H
