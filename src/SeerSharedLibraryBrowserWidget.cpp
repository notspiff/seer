#include "SeerSharedLibraryBrowserWidget.h"
#include "SeerUtl.h"
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItemIterator>
#include <QtWidgets/QLabel>
#include <QtWidgets/QApplication>
#include <QtCore/QFileInfo>
#include <QtCore/Qt>
#include <QtCore/QDebug>

SeerSharedLibraryBrowserWidget::SeerSharedLibraryBrowserWidget (QWidget* parent) : QWidget(parent) {

    // Construct the UI.
    setupUi(this);

    // Setup the widgets
    sharedLibrarySearchLineEdit->setPlaceholderText("Search...");
    sharedLibrarySearchLineEdit->setClearButtonEnabled(true);
    sharedLibraryTreeWidget->resizeColumnToContents(0);
    sharedLibraryTreeWidget->resizeColumnToContents(1);
    sharedLibraryTreeWidget->resizeColumnToContents(2);
    sharedLibraryTreeWidget->resizeColumnToContents(3);
    sharedLibraryTreeWidget->resizeColumnToContents(4);
    sharedLibraryTreeWidget->resizeColumnToContents(5);
    sharedLibraryTreeWidget->clear();
    sharedLibraryTreeWidget->setSortingEnabled(false);

    // Connect things.
    QObject::connect(sharedLibrarySearchLineEdit,        &QLineEdit::textChanged,            this,  &SeerSharedLibraryBrowserWidget::handleSearchLineEdit);
}

SeerSharedLibraryBrowserWidget::~SeerSharedLibraryBrowserWidget () {
}

void SeerSharedLibraryBrowserWidget::handleText (const QString& text) {

    // Don't do any work if the widget is hidden.
    if (isHidden()) {
        return;
    }

    QApplication::setOverrideCursor(Qt::BusyCursor);

    if (text.startsWith("^done,shared-libraries=[") && text.endsWith("]")) {

        sharedLibraryTreeWidget->clear();
        sharedLibraryTreeWidget->setSortingEnabled(false);
        sharedLibraryTreeWidget->sortByColumn(-1, Qt::AscendingOrder);

        // -file-list-shared-libraries
        // ^done,shared-libraries=[
        //     {id="/lib/libfoo.so",target-name="/lib/libfoo.so",host-name="/lib/libfoo.so",symbols-loaded="1",thread-group="i1",ranges=[{from="0x72815989",to="0x728162c0"}]},
        //     {id="/lib/libbar.so",target-name="/lib/libbar.so",host-name="/lib/libbar.so",symbols-loaded="1",thread-group="i1",ranges=[{from="0x76ee48c0",to="0x76ee9160"}]}
        // ]

        QString libraries_text = Seer::parseFirst(text, "shared-libraries=", '[', ']', false);

        //qDebug() << libraries_text;

        QStringList libraries_list = Seer::parse(libraries_text, "", '{', '}', false);

        for ( const auto& entry_text : libraries_list  ) {

            QString id_text             = Seer::parseFirst(entry_text, "id=",             '"', '"', false);
            QString target_name_text    = Seer::parseFirst(entry_text, "target-name=",    '"', '"', false);
            QString host_name_text      = Seer::parseFirst(entry_text, "host-name=",      '"', '"', false);
            QString symbols_loaded_text = Seer::parseFirst(entry_text, "symbols-loaded=", '"', '"', false);
            QString thread_group_text   = Seer::parseFirst(entry_text, "thread-group=",   '"', '"', false);
            QString ranges_text         = Seer::parseFirst(entry_text, "ranges=",         '[', ']', false);

            // Add the file to the tree.
            QTreeWidgetItem* item = new QTreeWidgetItem;
            item->setText(0, id_text);
            item->setText(1, target_name_text);
            item->setText(2, host_name_text);
            item->setText(3, symbols_loaded_text);
            item->setText(4, thread_group_text);
            item->setText(5, ranges_text);

            sharedLibraryTreeWidget->addTopLevelItem(item);
        }

    }else{
        // Ignore others.
    }

    sharedLibraryTreeWidget->resizeColumnToContents(0);
    sharedLibraryTreeWidget->resizeColumnToContents(1);
    sharedLibraryTreeWidget->resizeColumnToContents(2);
    sharedLibraryTreeWidget->resizeColumnToContents(3);
    sharedLibraryTreeWidget->resizeColumnToContents(4);
    sharedLibraryTreeWidget->resizeColumnToContents(5);
    sharedLibraryTreeWidget->sortByColumn(0, Qt::AscendingOrder);
    sharedLibraryTreeWidget->setSortingEnabled(true);

    sharedLibrarySearchLineEdit->clear();

    QApplication::restoreOverrideCursor();
}

void SeerSharedLibraryBrowserWidget::handleSearchLineEdit (const QString& text) {

    // Set everything to a normal font. If there is no search text, unhide everything.
    // If there is search text, hide everything so the matching ones can be unhidden later on.
    QTreeWidgetItemIterator it(sharedLibraryTreeWidget);

    if (*it) {

        QFont f0 = (*it)->font(0);

        f0.setBold(false);

        if (text == "") {
            while (*it) {
                (*it)->setHidden(false); // No search text, unhide everything.
                (*it)->setFont(0,f0);
                ++it;
            }
        }else{
            while (*it) {
                (*it)->setHidden(true); // Has serach text, hide everything. Matching items to be unhidden below.
                (*it)->setFont(0,f0);
                ++it;
            }
        }
    }

    // Set selected items to a bold font and unhidden. Move to the first match.
    if (text != "") {

        QList<QTreeWidgetItem*> matches;

        if (text.contains('*')) {
            matches = sharedLibraryTreeWidget->findItems(text, Qt::MatchWildcard | Qt::MatchRecursive, 0);
        }else{
            matches = sharedLibraryTreeWidget->findItems(text, Qt::MatchStartsWith | Qt::MatchRecursive, 0);
        }

        QList<QTreeWidgetItem*>::const_iterator it = matches.begin();
        QList<QTreeWidgetItem*>::const_iterator e  = matches.end();

        if (it != e) {

            sharedLibraryTreeWidget->setCurrentItem(*it);

            QFont f0 = (*it)->font(0);

            f0.setBold(true);

            while (it != e) {
                (*it)->setHidden(false);
                (*it)->setFont(0,f0);
                it++;
            }
        }

        //qDebug() << text << matches.size();
    }

    sharedLibraryTreeWidget->resizeColumnToContents(0);
    sharedLibraryTreeWidget->resizeColumnToContents(1);
    sharedLibraryTreeWidget->resizeColumnToContents(2);
    sharedLibraryTreeWidget->resizeColumnToContents(3);
    sharedLibraryTreeWidget->resizeColumnToContents(4);
    sharedLibraryTreeWidget->resizeColumnToContents(5);
}

void SeerSharedLibraryBrowserWidget::refresh () {
    emit refreshSharedLibraryList();
}

void SeerSharedLibraryBrowserWidget::showEvent (QShowEvent* event) {

    QWidget::showEvent(event);

    refresh();
}

