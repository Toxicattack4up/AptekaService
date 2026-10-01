#include "PasswordUtil.h"
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QByteArray>

namespace {
QByteArray randomSalt(int bytes = 16)
{
    QByteArray salt;
    salt.resize(bytes);
    for (int i = 0; i < bytes; ++i) {
        salt[i] = static_cast<char>(QRandomGenerator::global()->generate() & 0xFF);
    }
    return salt;
}

QByteArray sha256(const QByteArray& data)
{
    return QCryptographicHash::hash(data, QCryptographicHash::Sha256);
}
} // namespace

bool PasswordUtil::isHashed(const QString& stored)
{
    const int sep = stored.indexOf(':');
    if (sep <= 0) {
        return false;
    }
    const QString saltHex = stored.left(sep);
    const QString hashHex = stored.mid(sep + 1);
    return saltHex.size() >= 16 && hashHex.size() == 64;
}

QString PasswordUtil::hashPassword(const QString& plainPassword)
{
    const QByteArray salt = randomSalt();
    const QByteArray digest = sha256(salt + plainPassword.toUtf8());
    return QString::fromLatin1(salt.toHex()) + QLatin1Char(':') + QString::fromLatin1(digest.toHex());
}

bool PasswordUtil::verifyPassword(const QString& plainPassword, const QString& stored)
{
    if (isHashed(stored)) {
        const int sep = stored.indexOf(':');
        const QByteArray salt = QByteArray::fromHex(stored.left(sep).toLatin1());
        const QByteArray expected = QByteArray::fromHex(stored.mid(sep + 1).toLatin1());
        const QByteArray actual = sha256(salt + plainPassword.toUtf8());
        return actual == expected;
    }
    // Legacy plaintext demo credentials.
    return plainPassword == stored;
}
