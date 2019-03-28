#include "include/reporter.h"
#include "include/database.hpp"
#include "include/types/well.h"

#include <QPageLayout>
#include <QPdfWriter>
#include <QTextDocument>
#include <QTextTable>
#include <QTextTableFormat>

namespace c3picko {
Reporter::Reporter(Report::ID id, const Job& job, QDateTime creation,
                   const std::map<Well, Colony::ID>& pick_positions,
                   Image const& image, const DetectionResult* result,
                   QSet<Colony::ID> colonies_to_pick, const Profile& plate_,
                   const Profile& printer, const Profile& socket,
                   const Profile& octoprint)
    : id_(id),
      job_(job),
      creation_(creation),
      pick_positions_(pick_positions),
      image_(image),
      result_(result),
      colonies_to_pick_(colonies_to_pick),
      plate_(plate_),
      printer_(printer),
      socket_(socket),
      octoprint_(octoprint) {
  if (!job.resultJob()) throw Exception("Job cant have empty result");
}

Reporter Reporter::fromDatabase(
    Database& db, Report::ID id, Job::ID jid,
    const std::map<Well, Colony::ID>& pick_positions) {
  Job job = db.jobs().get(jid);
  Profile const &plate = db.profiles().get(job.plate()),
                &printer = db.profiles().get(job.printer()),
                &socket = db.profiles().get(job.socket()),
                &octoprint = db.profiles().get(job.octoprint());

  Image& img = db.images().get(job.imgID());
  DetectionResult* result =
      static_cast<DetectionResult*>(job.resultJob()->result().get());

  return Reporter(id, job, QDateTime::currentDateTime(), pick_positions, img,
                  result, job.coloniesToPick(), plate, printer, socket,
                  octoprint);
}

Report Reporter::createReport() const {
  ResourcePath pdf_path = reportFolder() + "report_" + id_ + ".pdf";
  QString img_name = "report_image_" + id_ + ".png";
  ResourcePath img_path = reportFolder() + img_name;
  ResourcePath htm_path = reportFolder() + "report_" + id_ + ".html";

  // Render the colonies and their ids with
  {
    cv::Mat mat = result_->first();
    std::vector<Colony> const& colonies(result_->colonies());

    for (auto it = pick_positions_.begin(); it != pick_positions_.end(); ++it) {
      Colony const& colony =
          *std::find_if(colonies.begin(), colonies.end(),
                        [it](Colony const& c) { return c.id() == it->second; });
      cv::Point pos(colony.x() * mat.cols, colony.y() * mat.rows);

      cv::Scalar clr(rand() % 255, rand() % 255, rand() % 255);
      Well const& well(it->first);
      cv::circle(mat, pos, colony.major_length(), clr, 1);
      math::drawText(mat, pos, well.toString(), clr, 1.5, 2);
    }

    cv::imwrite(img_path.toSystemAbsolute().toStdString(), mat);
  }
  // Create pdf and write to file
  {
    QString html;
    QPdfWriter pdf(pdf_path.toSystemAbsolute());
    writePdfReport(img_name, &pdf, html);
    // Write html
    QFile file(htm_path.toSystemAbsolute());

    if (!file.open(QIODevice::WriteOnly))
      throw Exception("Could not write report to: " +
                      htm_path.toSystemAbsolute());
    file.write(html.toUtf8());
  }

  return Report(job_.id(), pdf_path, img_path);
}

void Reporter::writePdfReport(QString img_name, QPdfWriter* pdf,
                              QString& html) const {
  QString title = "Report #" + id_ + " for job #" + job_.id() + " (" +
                  job_.name().left(20) + ")";
  pdf->setPageSize(QPagedPaintDevice::A4);
  pdf->setCreator("3CPickO");
  pdf->setTitle(title);
  // pdf->setResolution(2400);

  // cv::Mat const& mat = result_.oldMat();
  // QImage		   image(1000, 1000, QImage::Format_RGB888);
  // image.fill(Qt::red);

  std::vector<Colony> const& colonies = result_->colonies();
  AlgorithmJob* algo = job_.resultJob().get();
  // source https://forum.qt.io/topic/78143/export-a-qtablewidget-in-pdf
  const std::size_t columns = 4;
  const std::size_t rows = pick_positions_.size();
  html += "<!doctype html><html><head><title>" + title +
          "</title></head><body><font style=\"font-family:'Courier';\">";
  html += ("<center>Report #" + id_ + "</center><br>");
  html += ("<img style='max-width: 100%; height: auto;' src='" + img_name +
           "' alt='Colonies'>");
  html +=
      ("<br><br>" + image_.uploaded().toString(dateTimeFormat()) + ": " +
       "Image #" + image_.id().left(10) + "... created<br>" +
       job_.created().toString(dateTimeFormat()) + ": " + "Job #" + job_.id() +
       " created<br>" + algo->start().toString(dateTimeFormat()) + ": " +
       "AlgorithmJob #" + algo->id() + " created<br>" +
       algo->end().toString(dateTimeFormat()) + ": " + "DetectionResult #" +
       result_->id() + " created<br><br>");

  static std::vector<QString> headers = {" Well ", " Colony ",
                                         " PositionX [%] ", " PositionY [%] "};
  Q_ASSERT(headers.size() == columns);

  html += "<table style='width:50%' align='center' border='1'><tr>";
  for (int i = 0; i < columns; i++) {
    html += "<th>" + headers[i] + "</th>";
  }
  html += "</tr>";

  int i = 0;
  for (auto it = pick_positions_.begin(); it != pick_positions_.end(); ++it) {
    html += "<tr>";
    Well const& well = it->first;
    Colony const& colony =
        *std::find_if(colonies.begin(), colonies.end(),
                      [it](Colony const& c) { return c.id() == it->second; });

    std::vector<QString> data = {well.toString(), QString::number(it->second),
                                 QString::number(colony.x() * 100, 'f', 1),
                                 QString::number(colony.y() * 100, 'f', 1)};
    Q_ASSERT(data.size() == columns);

    ++i;
    for (std::size_t j = 0; j < columns; j++) {
      html += "<td>" + data[j].toUpper() + "</td>";
    }
    html += "</tr>";
  }
  html += "</table>";

  html += "<br><br><br>Detection settings";

  for (AlgoSetting const& setting : algo->settings())
    html += setting.name() + " = TODO<br>";
  html += "</font></body></html>";

  QTextDocument doc;
  doc.setHtml(html);
  doc.print(pdf);
}
}  // namespace c3picko
