#include <QtTest/QtTest>
#include "../PasswordUtil.h"

class PasswordUtilTest : public QObject {
    Q_OBJECT
private slots:
    void hashIsNotPlaintext();
    void verifyAcceptsCorrectPassword();
    void verifyRejectsWrongPassword();
    void legacyPlaintextStillWorks();
};

void PasswordUtilTest::hashIsNotPlaintext()
{
    const QString hashed = PasswordUtil::hashPassword("admin123");
    QVERIFY(PasswordUtil::isHashed(hashed));
    QVERIFY(hashed != "admin123");
}

void PasswordUtilTest::verifyAcceptsCorrectPassword()
{
    const QString hashed = PasswordUtil::hashPassword("secretpass");
    QVERIFY(PasswordUtil::verifyPassword("secretpass", hashed));
}

void PasswordUtilTest::verifyRejectsWrongPassword()
{
    const QString hashed = PasswordUtil::hashPassword("secretpass");
    QVERIFY(!PasswordUtil::verifyPassword("otherpass1", hashed));
}

void PasswordUtilTest::legacyPlaintextStillWorks()
{
    QVERIFY(PasswordUtil::verifyPassword("admin123", "admin123"));
}

QTEST_APPLESS_MAIN(PasswordUtilTest)
#include "PasswordUtilTest.moc"
