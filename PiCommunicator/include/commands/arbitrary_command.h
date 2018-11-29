#ifndef ARBITRARY_COMMAND_H_
#define ARBITRARY_COMMAND_H_

#include "command.h"

namespace c3picko {
namespace commands {
class ArbitraryCommand : public Command {
  Q_OBJECT

 public:
  ArbitraryCommand(QString command);
  ArbitraryCommand(QStringList commands);

  struct Reply : Command::Response {
    QString data1;
    int data2;
  };

 public slots:
  virtual void OnReplyFinished(QNetworkReply* reply) override;
};
}
}

#endif  // ARBITRARY_COMMAND_H_
