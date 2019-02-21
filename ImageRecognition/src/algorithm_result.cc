#include "include/algorithm_result.h"
#include "include/colony.hpp"
#include "include/colony.hpp"
#include <QJsonArray>
#include <opencv2/core/mat.hpp>

namespace c3picko
{
AlgorithmResult::AlgorithmResult(ID id) : id_(id), stages_succeeded_(false), cleanup_succseeded_(false) {}

AlgorithmResult::~AlgorithmResult() { cleanup(); }

cv::Mat& AlgorithmResult::newMat(cv::Mat const& copy_from)
{
	cv::Mat* new_mat = new cv::Mat();

	copy_from.copyTo(*new_mat);

	stack_.push_back(new_mat);
	return *new_mat;
}

cv::Mat& AlgorithmResult::oldMat()
{
	Q_ASSERT(stack_.back());
	return *stack_.back();
}

AlgorithmResult::ID AlgorithmResult::id() const { return id_; }

cv::Mat& AlgorithmResult::newMat()
{
	cv::Mat* new_mat = new cv::Mat();

	stack_.push_back(new_mat);

	return *new_mat;
}

void AlgorithmResult::cleanup()
{
	// Dont delete them right now, otherwise we cant user them
	// colonies_.~vector();
	qDeleteAll(stack_);
}

QString AlgorithmResult::stageError() const { return stage_error_; }

QString AlgorithmResult::cleanupError() const { return cleanup_error_; }

std::vector<Colony> AlgorithmResult::colonies() const { return colonies_; }

quint64 AlgorithmResult::tookNs() const { return took_ns_; }

int AlgorithmResult::lastStage() const { return last_stage_; }

bool AlgorithmResult::cleanupSuccseeded() const { return cleanup_succseeded_; }

bool AlgorithmResult::stagesSucceeded() const { return stages_succeeded_; }

template <> QJsonObject Marshalling::toJson(const AlgorithmResult& value)
{
	throw std::runtime_error("Not tested");
	QJsonObject obj;

	obj["id"]				  = value.id();
	obj["took_ns"]			  = qint64(value.tookNs());
	obj["stages_succeeded"]   = value.stagesSucceeded();
	obj["cleanup_succseeded"] = value.cleanupSuccseeded();
	obj["last_stage"]		  = value.lastStage();
	obj["stage_error"]		  = value.stageError();
	obj["cleanup_error"]	  = value.cleanupError();

	QJsonArray colonies;

	for (Colony const& colony : value.colonies())
		colonies.push_back(Marshalling::toJson(colony));

	return obj;
}

template <> AlgorithmResult Marshalling::fromJson(const QJsonObject& obj) { throw std::runtime_error("Not implemented"); }
}
