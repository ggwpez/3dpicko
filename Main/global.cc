#include "include/global.h"
#include <QSslConfiguration>
#include <QSslKey>
#include <QtDebug>

namespace c3picko {
QSslConfiguration* LoadSslConfig(QSettings* settings) {
  QSslConfiguration ssl;

  QString cert(settings->value("certificate").toString());
  QString key(settings->value("key").toString());

  QFile certFile(cert);
  QFile keyFile(key);
  if (!certFile.open(QIODevice::ReadOnly)) {
    qCritical() << "Certificate file:" << cert << ":" << certFile.errorString();
    return nullptr;
  }
  if (!keyFile.open(QIODevice::ReadOnly)) {
    qCritical() << "Key fiel" << cert << ":" << keyFile.errorString();
    return nullptr;
  }

  QSslCertificate certificate(&certFile, QSsl::Pem);
  QSslKey sslKey(&keyFile, QSsl::Rsa, QSsl::Pem);
  certFile.close();
  keyFile.close();
  ssl.setPeerVerifyMode(QSslSocket::VerifyNone);
  ssl.setLocalCertificate(certificate);
  ssl.setPrivateKey(sslKey);
  ssl.setProtocol(QSsl::SecureProtocols);  // TODO

  return new QSslConfiguration(ssl);
}
}  // namespace c3picko
