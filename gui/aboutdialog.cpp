#include "aboutdialog.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QString title = QString(tr("About %1")).arg(QApplication::applicationName());
    setWindowTitle(title);

    QString about_text;
    about_text = "<div>" + QApplication::applicationName() +
            tr(" is a video application that let's you sort, annotate and organize your video clips into bins.<br>");
    about_text += tr("Clips can then be transcoded and exported to different file format.</div>");
    about_text += tr("<div>This software is licensed under the <a href=http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html style=\"color: #0078D7;\">LGPLv2.1</a>"
                     " and its sources can be downloaded <a href=adresse_github style=\"color: #0078D7;\">here</a>");
    about_text += "<br>";
    about_text += tr("<div>This software uses code of <a href=http://ffmpeg.org style=\"color: #0078D7;\">FFmpeg</a> licensed"
                     " under the <a href=http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html style=\"color: #0078D7;\">LGPLv2.1</a>");
    about_text += "<br>";
    about_text += tr("<div>This software uses libraries from the <a href=http://www.qtav.org/ style=\"color: #0078D7;\">QtAV project</a> which is free software"
                     " licensed under the term of <a href=http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html style=\"color: #0078D7;\">LGPLv2.1</a></div>");
    about_text += "<br>";

    QLabel *label = new QLabel(this);
    label->setTextFormat(Qt::RichText);
    label->setText(about_text);
    label->openExternalLinks();

    QPushButton *close_btn = new QPushButton(" Close ", this);

    layout->addWidget(label);
    layout->addWidget(close_btn, 0, Qt::AlignRight);

    connect(close_btn, SIGNAL(clicked(bool)), this, SLOT(close()));

}
