#pragma once

#include "include/table.hpp"
#include "include/types/image.hpp"
#include "include/types/job.hpp"

namespace c3picko
{
class Database : public QObject, JsonConvertable
{
	Q_OBJECT
  public:
	typedef Table<Job>   JobTable;
	typedef Table<Image> ImageTable;

	Database(QString file_path, QObject* parent);
	~Database() override;

	void saveToFile();

	JobTable&   jobTable();
	JobTable&   deletedJobTable(); // Tracks deleted jobs TODO neeeded?
	ImageTable& imageTable();
	ImageTable& deletedImageTable(); // "

  public:
	void read(const QJsonObject&) override;
	void write(QJsonObject&) const override;

  private:
	QString file_path_;

	JobTable   job_table_;
	JobTable   deleted_job_table_;
	ImageTable image_table_;
	ImageTable deleted_image_table_;
};
}
