#include "include/types/image.hpp"

namespace c3picko {

	Image::Image(const QJsonObject& obj)
	 : JsonConstructable(obj), id_(obj["id"].toString()),
	   original_name_(obj["original_name"].toString()),
	  description_(obj["description"].toString()),
	  path_(obj["path"].toString()),
	  uploaded_(parseDateTime(obj["uploaded"]))
	{

	}

	Image::Image(ID id, QString original_name, QString description, QString path, QDateTime uploaded)
		: JsonConstructable(QJsonObject()), id_(id), original_name_(original_name), description_(description), path_(path), uploaded_(uploaded)
	{

	}

	Image::ID Image::id() const
	{
		return id_;
	}

	QString Image::path() const
	{
		return path_;
	}

	void Image::write(QJsonObject& obj) const
	{
		obj["id"] = id_;
		obj["original_name"] = original_name_;
		obj["description"] = description_;
		obj["path"] = path_;
		obj["uploaded"] = uploaded_.toMSecsSinceEpoch();
	}
}
