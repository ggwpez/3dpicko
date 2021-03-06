/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "GUI/reporter.h"

#include "GUI/database.h"
#include "GUI/types/well.h"
#include "Gcode/gcodeinstruction.h"
#include "Gcode/plateprofile.h"
#include "ImageRecognition/algorithm.h"
#ifndef D3PICKO_NO_QUAZIP
#include <JlCompress.h>
#endif

#include <QPageLayout>
#include <QPdfWriter>
#include <QTextDocument>
#include <QTextTable>
#include <QTextTableFormat>
#include <memory>

namespace d3picko {
Reporter::Reporter(const Job& job, QDateTime creation,
				   const std::map<Well, Colony::ID>& pick_positions,
				   Image const& image, const DetectionResult* result,
				   QSet<Colony::ID> colonies_to_pick, const Profile& plate,
				   const Profile& printer, const Profile& socket,
				   const Profile& octoprint,
				   std::vector<GcodeInstruction> const& gcode)
	: job_(job),
	  creation_(creation),
	  pick_positions_(pick_positions),
	  image_(image),
	  result_(result),
	  colonies_to_pick_(colonies_to_pick),
	  plate_(plate),
	  printer_(printer),
	  socket_(socket),
	  octoprint_(octoprint),
	  gcode_(gcode) {
  if (!job.resultJob()) throw Exception("Job cant have empty result");
}

Reporter Reporter::fromDatabase(
	const Database& db, Job::ID jid,
	const std::map<Well, Colony::ID>& pick_positions,
	std::vector<GcodeInstruction> const& gcode) {
  Job job = db.jobs().get(jid);
  Profile const &plate = db.profiles().get(job.plate()),
				&printer = db.profiles().get(job.printer()),
				&socket = db.profiles().get(job.socket()),
				&octoprint = db.profiles().get(job.octoprint());

  Image const& img = db.images().get(job.imgID());
  DetectionResult* result =
	  static_cast<DetectionResult*>(job.resultJob()->result().get());

  return Reporter(job, QDateTime::currentDateTime(), pick_positions, img,
				  result, job.coloniesToPick(), plate, printer, socket,
				  octoprint, gcode);
}

Report Reporter::createReport() const {
  QString name = "report_" + job_.id() + "-" + job_.name() + "-" +
				 job_.created().toString("dd.MM.yy");
  ResourcePath output = paths::reportFolder() + (name + ".zip");
  ResourcePath htm_path = paths::reportFolder() + (name + ".html");
  ResourcePath txt_path = paths::reportFolder() + (name + "_coords.txt");
  ResourcePath gcode_path = paths::reportFolder() + (name + ".gcode");
  cv::Mat img_data = result_->first();

  // Render the colonies and their ids
  {
	std::vector<Colony> const& colonies(result_->colonies());

	for (auto it = pick_positions_.begin(); it != pick_positions_.end(); ++it) {
	  Colony const& colony =
		  *std::find_if(colonies.begin(), colonies.end(),
						[it](Colony const& c) { return c.id() == it->second; });
	  cv::Point pos(colony.x() * img_data.cols, colony.y() * img_data.rows);

	  cv::Scalar clr(rand() % 255, rand() % 255, rand() % 255);
	  Well const& well(it->first);
	  cv::circle(img_data, pos, colony.major_length(), clr, 1);
	  math::drawText(img_data, pos, well.toString(), clr, 1.5, 2);
	}
  }

  // Create html and write to file
  {
	QFile file(htm_path.toSystemAbsolute());
	QString html;
	writeHtmlReport(img_data, html);

	if (!file.open(QIODevice::WriteOnly))
	  throw Exception("Could not write report to: " +
					  htm_path.toSystemAbsolute());

	file.write(html.toUtf8());
  }

  // Create txt file for Thomas Zolls roboter
  {
	QFile file(txt_path.toSystemAbsolute());
	QTextStream ts(&file);
	std::vector<Colony> const& colonies(result_->colonies());

	if (!file.open(QIODevice::WriteOnly))
	  throw Exception("Could not write coords to: " +
					  txt_path.toSystemAbsolute());

	for (auto it = pick_positions_.begin(); it != pick_positions_.end(); ++it) {
	  Colony const& colony =
		  *std::find_if(colonies.begin(), colonies.end(),
						[it](Colony const& c) { return c.id() == it->second; });

	  cv::Point2f pos(colony.x() * plate_.plate()->redFrameWidth(),
					  colony.y() * plate_.plate()->redFrameHeight());
	  ts << colony.id() << "," << pos.x << "," << pos.y << ",null,null,null\n";
	}
  }

  // Write gcode to file
  {
	QFile file(gcode_path.toSystemAbsolute());

	if (!file.open(QIODevice::WriteOnly))
	  throw Exception("Could not write gcode to: " +
					  gcode_path.toSystemAbsolute());

	QTextStream ts(&file);
	for (GcodeInstruction const& ins : gcode_)
	  ts << QString::fromStdString(ins.ToString()) << "\r\n";
  }

#ifndef D3PICKO_NO_ZLIB
  // Compress the html + gcode + txt_coords
  JlCompress::compressFiles(
	  output.toSystemAbsolute(),
	  {gcode_path.toSystemAbsolute(), htm_path.toSystemAbsolute(),
	   txt_path.toSystemAbsolute()});
#else
  // Let the client only download the html
  output = htm_path;
#endif

  return Report(job_.id(), output);
}

QString Reporter::createLog() const {
  AlgorithmJob* algo = job_.resultJob().get();
  return "<br>Log:<br>" + image_.uploaded().toString(dateTimeFormat()) + ": " +
		 "Image #" + image_.id() + " created<br>" +
		 job_.created().toString(dateTimeFormat()) + ": " + "Job #" +
		 job_.id() + " created<br>" + algo->start().toString(dateTimeFormat()) +
		 ": " + "AlgorithmJob #" + algo->id() + " created<br>" +
		 algo->end().toString(dateTimeFormat()) + ": " + "DetectionResult #" +
		 result_->id() + " created";
}

QString Reporter::createJobInfo() const {
  return "Name: " + job_.name() + createBr() +
		 "Desription: " + job_.description() + createBr() +
		 "Date: " + job_.created().toString(dateTimeFormat()) + createBr();
}

QString Reporter::createImage(QString url) const {
  return "<a href='" + url +
		 "'>"
		 "<img align='middle' style='max-width: 100%; height: auto;' src='" +
		 url +
		 "'>"
		 "</a>";
}

QString Reporter::createImage(QByteArray base64) const {
  return "<img align='middle' style='max-width: 100%; height: auto;' "
		 "src='data:image/png;base64," +
		 base64 + "'>";
}

QString Reporter::createBr(quint32 width) const {
  return QString("<br>").repeated(width);
}

QString Reporter::createTable(QString title, QVector<QString> col_name,
							  QVector<QVector<QString>> row_data) const {
  QString html("<center>" + title + "</center><br>");
  html += "<table style='width:50%' align='center' border='1'>";

  // Name the columns
  html += "<tr>";
  for (QString name : col_name) html += "<th>" + name + "</th>";
  html += "</tr>";

  // Append Row data
  for (QVector<QString> row : row_data) {
	html += "<tr>";
	for (QString entry : row) html += "<td>" + entry + "</td>";
	html += "</tr>";
  }

  html += "</table>";
  return html;
}

void Reporter::writeHtmlReport(const cv::Mat& img_data, QString& html) const {
  QByteArray image_data = math::matToBase64(img_data);

  AlgorithmJob* ajob = job_.resultJob().get();
  std::vector<Colony> const& colonies = result_->colonies();
  html += createProlog() + "<center>" + createTitle() + "</center>" +
		  createBr() + createJobInfo() + createBr() + createImage(image_data) +
		  createBr(2);

  // Colony data table
  {
	static QVector<QString> headers = {"#", "Well", "Area [pixel]"};
	QVector<QVector<QString>> row_data;

	quint32 i = 0;
	for (auto it = pick_positions_.begin(); it != pick_positions_.end(); ++it) {
	  Well const& well = it->first;
	  Colony const& colony =
		  *std::find_if(colonies.begin(), colonies.end(),
						[it](Colony const& c) { return c.id() == it->second; });
	  QVector<QString> data = {QString::number(++i), well.toString(),
							   QString::number(colony.area(), 'f', 0)};

	  row_data.push_back(data);
	}

	html += createTable("Picked Colonies (" + QString::number(i) + ")", headers,
						row_data);
  }
  html += createBr(2);
  // Detection settings
  {
	static QVector<QString> headers = {"Name", "Value"};
	QVector<QVector<QString>> row_data;

	for (AlgoSetting const& setting : ajob->settings())
	  row_data.push_back({setting.name(), setting.value<QString>()});

	html += createTable("Settings for '" + ajob->algo()->name() + "'", headers,
						row_data);
  }
  html += createBr(2) + createLog();
  html += createEpilog();
}

QString Reporter::createProlog() const {
  return "<!doctype html><html><head><title>" + createTitle() +
		 "</title></head><body><tt>";
}

QString Reporter::createTitle() const { return "Report for Job #" + job_.id(); }

QString Reporter::createEpilog() const { return "</tt></body></html>"; }
}  // namespace d3picko
