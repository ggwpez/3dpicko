#pragma once

#include "include/types/job.hpp"
#include "include/types/image.hpp"
#include "include/table.hpp"

namespace c3picko {
	class Database : public QObject, JsonConvertable
	{
		Q_OBJECT
	public:
		typedef Table<Job> JobTable;
		typedef Table<Image> ImageTable;

		Database(QString file_path, QObject* parent);
		~Database() override;

		void saveToFile();

		JobTable& jobTable();
		ImageTable& imageTable();

	public:
		void read(const QJsonObject&) override;
		void write(QJsonObject&) const override;

	private:
		QString file_path_;

		JobTable job_table_;
		ImageTable image_table_;
	};
}
