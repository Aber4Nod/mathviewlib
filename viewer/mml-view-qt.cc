// Copyright (C) 2000-2007, Luca Padovani <padovani@sti.uniurb.it>.
// Copyright (C) 2013, Khaled Hosny <khaledhosny@eglug.org>.
// Copyright (C) 2014, Yue Liu <yue.liu@mail.com>.
//
// This file is part of GtkMathView, a flexible, high-quality rendering
// engine for MathML documents.
//
// GtkMathView is free software; you can redistribute it and/or modify it
// either under the terms of the GNU Lesser General Public License version
// 3 as published by the Free Software Foundation (the "LGPL") or, at your
// option, under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation (the "GPL").  If you do not
// alter this notice, a recipient may use your version of this file under
// either the GPL or the LGPL.
//
// GtkMathView is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the LGPL or
// the GPL for more details.
//
// You should have received a copy of the LGPL and of the GPL along with
// this program in the files COPYING-LGPL-3 and COPYING-GPL-2; if not, see
// <http://www.gnu.org/licenses/>.

#include <config.h>

#include "Qt_RenderArea.hh"
#include "Logger.hh"

#include <QApplication>
#include <QCommandLineParser>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QPalette>
#include <QMenuBar>
#include <QToolBar>
#include <QObject>
#include <QToolButton>
#include <QPixmap>

QAction *addToolBarActions(Qt_RenderArea* ra, QMenu *alignMenu, std::basic_string<char> s);
QAction *addToolBarActionsExt(Qt_RenderArea* ra, QMenu *alignMenu, std::vector<std::basic_string<char>> actions);
void addSubMenuExt(Qt_RenderArea* ra, QMenu *alignMenu, QString menuStyle, std::vector<std::vector<std::basic_string<char>>> actions);

int
main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("mml-view-qt");
    QApplication::setApplicationVersion(PACKAGE_VERSION);

    a.setAttribute(Qt::AA_DontUseNativeMenuBar);
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("input", "MathML file to view.");
    parser.process(a);

    const QStringList args = parser.positionalArguments();

    const QString input_file = args.at(0);

    SmartPtr<AbstractLogger> logger = Logger::create();

    QMainWindow w;
    Qt_RenderArea* ra = new Qt_RenderArea(logger);
    // ra->loadURI(input_file.toUtf8());
    QVBoxLayout* layout = new QVBoxLayout;
    // layout->addWidget(ra);
    QWidget* window = new QWidget();

    QMenu *pmnuFile = new QMenu("&File");
    QAction* newFile = pmnuFile->addAction("&New",
                        ra,
                        SLOT(slotNew()),
                        QKeySequence("CTRL+N")
                        );
    pmnuFile->addSeparator();

    QAction *openFolder = pmnuFile->addAction("&Open",
                        ra,
                        SLOT(slotLoad()),
                        QKeySequence("CTRL+O")
                        );
   
    QAction *saveFile = pmnuFile->addAction("&Save",
                        ra,
                        SLOT(slotSave()),
                        QKeySequence("CTRL+S")
                        );
   
   pmnuFile->addAction("S&ave As",
                       ra,
                       SLOT(slotSaveAs()),
                       QKeySequence("CTRL+Shift+S")
                       );
   pmnuFile->addSeparator();
   QAction *closeFile = pmnuFile->addAction("&Close",
                       ra,
                       SLOT(close()),
                       QKeySequence("CTRL+W")
                       );

    QMenu *pmnuInsertion = new QMenu("&Insert");
    ra->connect(pmnuInsertion->addAction("&underover"),  &QAction::triggered, ra, [ra]{ ra->insert("munderover"); });
    ra->connect(pmnuInsertion->addAction("&fraction"),  &QAction::triggered, ra, [ra]{ ra->insert("mfrac"); });
    ra->connect(pmnuInsertion->addAction("&over"),  &QAction::triggered, ra, [ra]{ ra->insert("mover"); });
    ra->connect(pmnuInsertion->addAction("&under"),  &QAction::triggered, ra, [ra]{ ra->insert("munder"); });
    ra->connect(pmnuInsertion->addAction("&mi"),  &QAction::triggered, ra, [ra]{ ra->insert("mi"); }); // dont work inserting 
    ra->connect(pmnuInsertion->addAction("&msqrt"),  &QAction::triggered, ra, [ra]{ ra->insert("msqrt"); });
    ra->connect(pmnuInsertion->addAction("&mroot"),  &QAction::triggered, ra, [ra]{ ra->insert("mroot"); });
    ra->connect(pmnuInsertion->addAction("&msub"),  &QAction::triggered, ra, [ra]{ ra->insert("msub"); });
    ra->connect(pmnuInsertion->addAction("&msup"),  &QAction::triggered, ra, [ra]{ ra->insert("msup"); });
    ra->connect(pmnuInsertion->addAction("&msubsup"),  &QAction::triggered, ra, [ra]{ ra->insert("msubsup"); });

    QMenu *pmnuDeletion = new QMenu("&Delete");
    ra->connect(pmnuDeletion->addAction("&DeleteSelected"),  &QAction::triggered, ra, [ra]{ ra->deleteSelectedElements(); });

    QMenu *pmnuCopy = new QMenu("&Copy");
    ra->connect(pmnuCopy->addAction("&CopySelected"),  &QAction::triggered, ra, [ra]{ ra->copyElement(); });
    ra->connect(pmnuCopy->addAction("&InsertSelected"),  &QAction::triggered, ra, [ra]{ ra->insertCopiedElement(); });
    ra->connect(pmnuCopy->addAction("&UnselectAllElements"),  &QAction::triggered, ra, [ra]{ ra->unselectAllElements(); });

    QMenuBar* menuBar = new QMenuBar();

    menuBar->addMenu(pmnuFile);
    menuBar->addMenu(pmnuInsertion);
    menuBar->addMenu(pmnuDeletion);
    menuBar->addMenu(pmnuCopy);
    layout->setMenuBar(menuBar);

    QToolBar *firstToolBar = new QToolBar("First Toolbar");
    QMenu *alignMenu = new QMenu("signs");
    QString  menuStyle(
        "QMenu {"
        "background-color: rgba(255,255,255, 0.7);"
            "margin: 2px; /* some spacing around the menu */"
        "}"
        "QMenu::item {"
            "color: black;"
            "padding: 2px 25px 2px 20px;"
            "border: 1px solid transparent; /* reserve space for selection border */"
        "}"
        "QMenu::item:selected {"
            "border-color: darkblue;"
            "background: rgba(100, 100, 100, 150);"
        "}"
        "QMenu::icon:checked { /* appearance of a 'checked' icon */"
            "background: gray;"
            "border: 1px inset gray;"
            "position: absolute;"
            "top: 1px;"
            "right: 1px;"
            "bottom: 1px;"
            "left: 1px;"
        "}"
        "QMenu::separator {"
            "height: 2px;"
            "background: lightblue;"
            "margin-left: 10px;"
            "margin-right: 5px;"
        "}"
        "QMenu::indicator {"
            "width: 13px;"
            "height: 13px;"
        "}"
      );
    alignMenu->setStyleSheet(menuStyle);

    QToolButton *saveButton = new QToolButton();
    saveButton->setMenu(alignMenu);
    saveButton->setPopupMode(QToolButton::InstantPopup);
    saveButton->setToolButtonStyle(Qt::ToolButtonIconOnly);

    // Sign actions
    addSubMenuExt(ra, alignMenu, menuStyle, {
        {"\u227A", "\u227B", "\u22B2", "\u22B3"},
        {"\u007E", "\u2248", "\u2243", "\u2245"},
        {"\u2260", "\u2261", "\u225C", "\u2259"},
        {"\u2250", "\u221D"},
    });
    addToolBarActionsExt(ra, alignMenu, {
        "\u2265", "\u2264", "\u226A", "\u226B",
    });

    QAction *insertSign = new QAction("Signs");
    insertSign->setIcon(QIcon("/Users/n.mikhnenko/mathviewlib/src/backend/qt/signs.png"));
    saveButton->setDefaultAction(insertSign);

    firstToolBar->addWidget(saveButton);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(firstToolBar);
    layout->addWidget(ra);

    QPalette Pal;
    Pal.setColor(QPalette::Background, Qt::white);
    window->setAutoFillBackground(true);
    window->setPalette(Pal);

    window->setLayout(layout);
    w.setCentralWidget(window);
    w.show();
    return a.exec();
}

QAction *
addToolBarActions(Qt_RenderArea* ra, QMenu *alignMenu, std::basic_string<char> s)
{
    QAction *preceds = new QAction(QString::fromStdString(s), ra);
    alignMenu->addAction(preceds);
    ra->connect(preceds, &QAction::triggered, ra, [ra, s]{ ra->insertGlyphAfter(s); });
    return preceds;
}

QAction *
addToolBarActionsExt(Qt_RenderArea* ra, QMenu *alignMenu, std::vector<std::basic_string<char>> actions)
{
    QAction *defAct = addToolBarActions(ra, alignMenu, actions[0]);
    actions.erase(actions.begin());
    for (const auto & i : actions)
        addToolBarActions(ra, alignMenu, i);
    return defAct;
}

void
addSubMenuExt(Qt_RenderArea* ra, QMenu *alignMenu, QString menuStyle, std::vector<std::vector<std::basic_string<char>>> actions)
{
    if (actions.empty())
        return;

    QMenu *alignMenu2 = new QMenu("...");
    std::vector<std::basic_string<char>> act = actions[0];
    actions.erase(actions.begin());
    addSubMenuExt(ra, alignMenu2, menuStyle, actions);
    addToolBarActionsExt(ra, alignMenu2, act);
    alignMenu2->setStyleSheet(menuStyle);
    alignMenu->addMenu(alignMenu2);
}
