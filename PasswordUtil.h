#ifndef PASSWORDUTIL_H
#define PASSWORDUTIL_H

#include <QString>

class PasswordUtil {
public:
    // Returns "saltHex:hashHex" using SHA-256(salt || password) with per-user random salt.
    static QString hashPassword(const QString& plainPassword);
    // Accepts hashed form or legacy plaintext (for one-time migration of old data.json).
    static bool verifyPassword(const QString& plainPassword, const QString& stored);
    static bool isHashed(const QString& stored);
};

#endif // PASSWORDUTIL_H
