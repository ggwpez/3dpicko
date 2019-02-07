#pragma once

#include "include/table.hpp"
#include "include/types/image.hpp"
#include "include/types/job.hpp"

// Cant forward declare bc type traits check in class Table<typename T>
#include "include/types/profile.hpp"

namespace c3picko
{
class Database : public QObject, JsonConvertable
{
	Q_OBJECT
  public:
	typedef Table<Job>   JobTable;
	typedef Table<Image> ImageTable;
	typedef Table<ProfileWrapper> ProfileTable;

	Database(QString file_path, QObject* parent);
	~Database() override;

	void saveToFile();

	JobTable&   jobs();
	JobTable&   deletedJobs(); // Tracks deleted jobs TODO neeeded?
	ImageTable& images();
	ImageTable& deletedImages(); // "
	ProfileTable& profiles();

	Image::ID newImageId();
	Job::ID newJobId();
	ProfileWrapper::ID newProfileId();

  public:
	void read(const QJsonObject&) override;
	void write(QJsonObject&) const override;

  private:
	QString file_path_;

	JobTable   jobs_;
	JobTable   deleted_jobs_;
	ImageTable images_;
	ImageTable deleted_images_;
	ProfileTable profiles_;

	qint32 image_id_;
	qint32 job_id_;
	qint32 profile_id_;
};
}
