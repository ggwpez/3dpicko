#ifndef COMMAND_H_
#define COMMAND_H_

#include "response.h"
#include <QHttpMultiPart>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QSet>
#include <QUrlQuery>

namespace c3picko {
namespace pi {
class Command : public QObject {
  Q_OBJECT

public:
  enum class HTTPType { GET, POST, PUT, PATCH, DELETE };
  typedef responses::Response Response;

  Command(QString api_url, QSet<int> status_ok, HTTPType type);
  Command(QString api_url, QByteArray data, QSet<int> status_ok, HTTPType type_,
          QString content_type = "application/json; charset=utf-8");
  Command(QString api_url, QJsonObject data, QSet<int> status_ok,
          HTTPType type_,
          QString content_type = "application/json; charset=utf-8");
  Command(QString api_url, QHttpMultiPart *query, QSet<int> status_ok,
          HTTPType type_,
          QString content_type = "application/json; charset=utf-8");
  ~Command();

  /**
   * @brief Returns the API-URL for the Post request
   *
   * Eg: http://localhost/api/start_print
   * Here start_print would be returned
   */
  QString GetApiUrl() const;
  HTTPType type() const;

  QByteArray GetPostData() const;
  QHttpMultiPart *GetPostQuery() const;
  virtual QString GetContentType() const;
  bool IsQuery() const;

public slots:
  virtual void OnReplyFinished(QNetworkReply *);
  virtual void OnReplyError(QNetworkReply::NetworkError);

protected slots:
  /**
   * @brief Checks whether the status code from the reply is in the
   * Command::status_ok_ set. Emits #OnStatusOk or @p reply
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
  inline void CheckStatusCodeAndResponse(QNetworkReply *reply);

signals:
  void OnStatusOk(int status, Response *);
  void OnStatusErr(QVariant status, Response *);
  void OnNetworkErr(QString error);
  /**
   * @brief This will be always emitted after one of the above was raised.
   * Use it for cleanup.
   */
  void OnFinished();

private:
  void SetupSlots();

protected:
  QString const api_url_;
  // TODO bad style
  QByteArray data_;
  QHttpMultiPart *query_ = nullptr;
  QSet<int> const status_ok_;
  HTTPType const type_;
  QString content_type_;
};
} // namespace pi
} // namespace c3picko
#include "command.inc.h"
#endif // COMMAND_H_
