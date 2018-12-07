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
	enum class Type
	{
		GET,
		POST
	};

  Command(QString api_url, QByteArray data, QSet<int> status_ok, Type type_,
		  QString content_type = "application/json; charset=utf-8");
  Command(QString api_url, QJsonObject data, QSet<int> status_ok, Type type_,
		  QString content_type = "application/json; charset=utf-8");

  /**
   * @brief Returns the API-URL for the Post request
   *
   * Eg: http://localhost/api/start_print
   * Here start_print would be returned
   */
  QString GetApiUrl() const;

  typedef commands::responses::Response Response;

  Type type() const;

public slots:
  QByteArray GetPostData() const;
  virtual QString GetContentType() const;

  virtual void OnReplyFinished(QNetworkReply *);
  virtual void OnReplyError(QNetworkReply::NetworkError);

protected slots:
  /**
   * @brief Checks whether the status code from the reply is in the
   * Command::status_ok_ set Emits #OnStatusOk or @p reply
   * @param reply Network reply received by Command::OnReplyFinished()
   * @param response Response or nothing if the command has no response
   */
  void CheckStatusCode(QNetworkReply *reply, Response *response = nullptr);

protected:
  /**
   * @brief Reads all data from @p reply, parses it as JSON and construct a new
   * Object of Type @p T from it. Then passes the Object to Check status code.
   * @tparam T Type of the Response, is then passed to #CheckStatusCode
   * @param reply Network reply received by #OnReplyFinished
   *
   */
  template <typename T>
  inline void OnReplyFinishedDefault(QNetworkReply *reply);

signals:
  void OnStatusOk(int status, Response *);
  void OnStatusErr(QVariant status, Response *);
  void OnNetworkErr(QString error);

protected:
  QString const api_url_;
  QByteArray data_;
  QSet<int> const status_ok_;
  QString const content_type_;
  Type const type_;
};
} // namespace c3picko

#include "command.inc.h"
#endif // COMMAND_H_
