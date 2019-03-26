#include "include/types/report.h"
#include "include/database.hpp"

#include <QPageLayout>
#include <QPdfWriter>
#include <QTextDocument>
#include <QTextTable>
#include <QTextTableFormat>

namespace c3picko {
Report::Report(Report::ID id, const Job& job, QDateTime creation,
               const std::map<Colony::ID, Well>& pick_positions,
               Image const& image, const DetectionResult& result,
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

Report Report::fromDatabase(Database& db, ID id, Job::ID jid,
                            const std::map<Colony::ID, Well>& pick_positions) {
  Job job = db.jobs().get(jid);
  Profile const &plate = db.profiles().get(job.plate()),
                &printer = db.profiles().get(job.printer()),
                &socket = db.profiles().get(job.socket()),
                &octoprint = db.profiles().get(job.octoprint());

  Image& img = db.images().get(job.imgID());
  DetectionResult& result = std::ref(db.detectionResults().get(job.resultID()));

  return Report(id, job, QDateTime::currentDateTime(), pick_positions, img,
                result, job.coloniesToPick(), plate, printer, socket,
                octoprint);
}

void Report::writePdfReport(QPdfWriter* pdf) {
  pdf->setPageSize(QPagedPaintDevice::A4);
  pdf->setCreator("3CPickO");
  pdf->setTitle("Report #" + id_ + " for job #" + job_.id() + " (" +
                job_.name().left(20) + ")");
  pdf->setResolution(2400);

  // cv::Mat const& mat = result_.oldMat();
  // QImage		   image(1000, 1000, QImage::Format_RGB888);
  // image.fill(Qt::red);

  AlgorithmJob* algo = job_.resultJob().get();
  // source https://forum.qt.io/topic/78143/export-a-qtablewidget-in-pdf
  const std::size_t columns = 5;
  std::vector<Colony> colonies = result_.colonies();
  const std::size_t rows = colonies.size();
  QTextDocument doc;
  doc.setDefaultFont(QFont("Courier", 8));  // TODO font maybe not available
  QTextCursor cursor(&doc);
  cursor.insertHtml("<center>Report #" + id_ + "</center><br>");
  // cursor.insertImage(image);
  cursor.insertHtml(
      "<br><br>" + image_.uploaded().toString(dateTimeFormat()) + ": " +
      "Image #" + image_.id().left(10) + "... created<br>" +
      job_.created().toString(dateTimeFormat()) + ": " + "Job #" + job_.id() +
      " created<br>" + algo->start().toString(dateTimeFormat()) + ": " +
      "AlgorithmJob #" + algo->id() + " created<br>" +
      algo->end().toString(dateTimeFormat()) + ": " + "DetectionResult #" +
      result_.id() + " created<br><br>");
  /*"<center>Job "
+ job_.id() + " will pick " + QString::number(pick_positions_.size()) + "
colonies from all "
+ QString::number(colonies.size()) + " detected</center><br><br><br>");*/
  QTextTableFormat tableFormat;
  tableFormat.setHeaderRowCount(1);
  tableFormat.setAlignment(Qt::AlignHCenter);
  tableFormat.setCellPadding(0);
  tableFormat.setCellSpacing(0);
  tableFormat.setBorder(1);
  tableFormat.setBorderBrush(QBrush(Qt::SolidPattern));
  tableFormat.clearColumnWidthConstraints();
  QTextTable* textTable = cursor.insertTable(rows + 1, columns, tableFormat);
  QTextCharFormat tableHeaderFormat, tableCellFormat;
  tableHeaderFormat.setBackground(QColor("#DADADA"));
  QTextBlockFormat cellBockFormat;
  cellBockFormat.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  static char const* headers[columns] = {" Colony ", " Picked ", " Well ",
                                         " Excluded by ", " Area [pxl] "};
  for (int i = 0; i < columns; i++) {
    QTextTableCell cell = textTable->cellAt(0, i);
    cell.setFormat(tableHeaderFormat);
    QTextCursor cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(headers[i]);
  }

  for (std::size_t i = 0; i < rows; i++) {
    Colony const& c = colonies[i];
    bool picked = (pick_positions_.find(c.id()) != pick_positions_.end());
    QString comment;
    if (!picked) {
      if (!c.excludedBy().isEmpty())
        comment = c.excludedBy();
      else if (!colonies_to_pick_.contains(c.id()))
        comment = "USER/RANDOM";
    }
    QString well = (picked ? pick_positions_[c.id()].toString() : "");

    QString data[columns] = {QString::number(c.id()), (picked ? "YES" : "NO"),
                             well, comment, QString::number(c.area(), 'e', 2)};

    for (std::size_t j = 0; j < columns; j++) {
      QTextTableCell cell = textTable->cellAt(i + 1, j);
      cell.setFormat(tableCellFormat);
      QTextCursor cellCursor = cell.firstCursorPosition();
      cellCursor.setBlockFormat(cellBockFormat);
      cellCursor.insertText(data[j].toUpper());
    }
  }
  cursor.movePosition(QTextCursor::End);

  cursor.insertHtml("<br><br><br>Detection settings");
  for (AlgoSetting const& setting : algo->settings())
    cursor.insertHtml(setting.name() + " = TODO<br>");

  doc.setDocumentMargin(0);
  // doc.setTextWidth(5);
  doc.print(pdf);
}

QString Well::toString() const { return "A1"; }
}  // namespace c3picko
