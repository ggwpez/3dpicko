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
  Command(QString api_url, QByteArray data, QSet<int> status_ok,
          QString content_type = "application/json; charset=utf-8");
  Command(QString api_url, QJsonObject data, QSet<int> status_ok,
          QString content_type = "application/json; charset=utf-8");
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
  virtual QString GetContentType() const;

  virtual void OnReplyFinished(QNetworkReply *);
  virtual void OnReplyError(QNetworkReply::NetworkError);

protected slots:
  ///
  /// \brief Checks whether the status code from the reply is in the #status_ok_
  /// set Emits #OnStatusOk or #OnStatusErr \param reply Network reply received
  /// by OnReplyFinished \param response Response or nothing if the command has
  /// no response
  ///
  void CheckStatusCode(QNetworkReply *reply, Response *response = nullptr);

protected:
  ///
  /// \brief Reads all data from \a reply, parses it as JSON and construct a new
  /// Object of Type T from it. Then passes the Object to Check status code.
  /// \tparam T Type of the Response, is then passed to #CheckStatusCode
  /// \param reply Network reply received by OnReplyFinished
  ///
  template <typename T>
  inline void OnReplyFinishedDefault(QNetworkReply *reply);

signals:
  void OnStatusOk(int status, Response *);
  void OnStatusErr(QVariant status, Response *);
  void OnNetworkErr(QString error);

protected:
  QString api_url_;
  QByteArray data_;
  QSet<int> status_ok_;
  Response *answer_ = nullptr;
  QString content_type_;
};
} // namespace c3picko

#include "command.inc.h"
#endif // COMMAND_H_
