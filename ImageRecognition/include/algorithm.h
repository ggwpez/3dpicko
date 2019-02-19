#pragma once

#include "include/algo_setting.h"
#include <QRunnable>
#include <QVector>
#include <functional>
#include <stack>

namespace c3picko
{
class AlgoSetting;
/**
 * @brief Performs calculations stepwise.
 * Operations from other Algorithms can be reused.
 * @tparam Input data type
 * @tparam Output data type
 */
class Algorithm : public QObject, public QRunnable
{
	Q_OBJECT
  public:
	typedef QString ID;
	typedef void (*AlgoStep)(Algorithm*, void*, void**);
	typedef void (*AlgoCleanup)(Algorithm*);

	Algorithm(ID id, QString name, QString description, QList<AlgoStep> steps, AlgoCleanup cleanup, QList<AlgoSetting> settings,
			  bool isThreadable);
	virtual ~Algorithm() override;

	/**
	 * @brief Call with according input data pointer before run()
	 * @param input
	 */
	void setInput(void* input);

	/**
	 * @brief Executes all steps of algorithm one by one.
	 * Emits OnFinished(void*) when done with the last output pointer.
	 */
	virtual void run() override;

	/**
	 * @brief Returns a new instance of the derived class.
	 * Function is similar to common clone() but returns an empty instance, hence
	 * the name.
	 * Used instead of a factory.
	 * @return Pointer to instance.
	 */
	virtual Algorithm* cloneEmpty() const = 0;

  signals:
	void OnFinished(void*);

  public:
	ID				   id() const;
	QString			   name() const;
	QString			   description() const;
	QList<AlgoSetting> settings() const;
	bool			   isThreadable() const;

	AlgoSetting const& settingById(AlgoSetting::ID id) const;
	AlgoSetting const& settingByName(QString name) const;

	void setSettingsValueByID(AlgoSetting::ID id, QVariant value);
	void setSettingsValueByName(QString name, QVariant value);

	void setSettings(QJsonObject const&);

	/**
	 * @brief Abstract classes can not be marshallable.
	 */
	QJsonObject baseToJson() const;

	QVector<void*>& stack();

  protected:
	const ID		   id_;
	const QString	  name_, description_;
	QList<AlgoStep>	steps_;
	AlgoCleanup		   cleanup_;
	QList<AlgoSetting> settings_;
	bool			   is_threadable_;
	// Algorithmic data
	void* input_;
	/**
	 * @brief Agorithms can use this stack for storing data until deleting in in
	 * the cleanup function.
	 */
	QVector<void*> stack_;
};
MAKE_SERIALIZABLE(Algorithm);
}
