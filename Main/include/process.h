#pragma once

#include <QObject>
#include "include/global.h"
#include "include/resource_path.h"

class QProcess;
namespace c3picko {
class Process : public QObject {
  Q_OBJECT
 public:
  /**
   * @brief Creates a clone call for git.
   * @param URL of the repository to clone
   * @param Directory to clone into.
   */
  static Process* gitClone(QString repo, const ResourcePath& into,
                           QStringList arguments = {});
  static Process* gitCheckout(QString branch, const ResourcePath& repo);
  /**
   * @brief Git log command.
   * @param Directory of the repository to log
   * @param additional arguments, emtpy means {"--pretty=format:\"%H#%ad\"",
   * "--date=rfc2822", "--max-count=5"}
   */
  static Process* gitLog(const ResourcePath& repo, QStringList arguments = {});

  ~Process();

 public slots:
  void start();
  void kill();

 signals:
  void OnStarted();
  void OnFinished();

  void OnSuccess(QString stdout_);
  void OnFailure(QString stderr_);

 private:
  Process(QStringList args, QObject* _parent = nullptr);
  Process(QStringList args, ResourcePath const& execution_dir,
          QObject* _parent = nullptr);

  QProcess* git_;
  QStringList args_;
  ResourcePath exec_dir_;
};
}  // namespace c3picko
