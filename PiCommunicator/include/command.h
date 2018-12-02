#ifndef COMMAND_H_
#define COMMAND_H_

#include "commands/responses/response.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QSet>
#include <QUrlQuery>

namespace c3picko {
class Command : public QObject {
  Q_OBJECT

public:
  Command(QString api_url, QByteArray data, QSet<int> status_ok);
  Command(QString api_url, QJsonObject data, QSet<int> status_ok);
  ~Command();

  /**
   * @brief Returns the API-URL for the Post request
   *
   * Eg: http://localhost/api/start_print
   * Here start_print would be returned
   */
  QString GetApiUrl() const;

  typedef commands::responses::Response Response;

public slots:
  QByteArray GetPostData() const;

  virtual void OnReplyFinished(QNetworkReply *);
  virtual void OnReplyError(QNetworkReply::NetworkError);

protected slots:
  void CheckStatusCode(QNetworkReply *reply, Response *answer = nullptr);

signals:
  void OnStatusOk(int status, Response *);
  void OnStatusErr(QVariant status, Response *);
  void OnNetworkErr(QString error);

protected:
  QString api_url_;
  QByteArray data_;
  QSet<int> status_ok_;
  Response *answer_ = nullptr;
};
} // namespace c3picko

#endif // COMMAND_H_
