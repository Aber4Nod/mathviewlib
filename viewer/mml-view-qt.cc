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

using namespace std;

QAction *addToolBarActions(Qt_RenderArea* ra, QMenu *alignMenu, std::basic_string<char> s);
QAction *addToolBarActionsExt(Qt_RenderArea* ra, QMenu *alignMenu, std::vector<std::basic_string<char>> actions);
void addSubMenuExt(Qt_RenderArea* ra, QMenu *alignMenu, QString menuStyle, std::vector<std::vector<std::basic_string<char>>> actions);

QAction *addToolBarActionsElements(Qt_RenderArea* ra, QMenu *alignMenu, std::string icon, std::string name, std::map<string, string> opts);
QAction *addToolBarActionsElementsExt(Qt_RenderArea* ra, QMenu *alignMenu, std::map<std::pair<std::string, std::string>, std::map<std::string, std::string>> actions);
void addSubMenuElementsExt(Qt_RenderArea* ra, QMenu *alignMenu, QString menuStyle, std::vector<std::map<std::pair<std::string, std::string>, std::map<std::string, std::string>>> actions);

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

    // Sign actions ------------------------------------------------------------
    QToolBar *firstToolBar = new QToolBar("First Toolbar");
    QMenu *alignMenu = new QMenu("signs");
    alignMenu->setStyleSheet(menuStyle);

    QToolButton *saveButton = new QToolButton();
    saveButton->setMenu(alignMenu);
    saveButton->setPopupMode(QToolButton::InstantPopup);
    saveButton->setToolButtonStyle(Qt::ToolButtonIconOnly);

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

    // Sign actions ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    // Fenced braces -----------------------------------------------------------
    QToolBar *secondToolBar = new QToolBar;
    QMenu *bracesMenu = new QMenu("braces");
    bracesMenu->setStyleSheet(menuStyle);
    QToolButton *secondButton = new QToolButton();

    QAction *fences = new QAction("fences");
    fences->setIcon(QIcon("/Users/n.mikhnenko/mathviewlib/src/backend/qt/mfenced.png"));

    secondButton->setMenu(bracesMenu);
    secondButton->setPopupMode(QToolButton::InstantPopup);
    secondButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    secondButton->setDefaultAction(fences);
    addSubMenuElementsExt(ra, bracesMenu, menuStyle, {
        {
            {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/mfenced5.png", "mfenced"}, {
                {"open", "["},
                {"close", "]"},
            }},
            {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/mfenced6.png", "mfenced"}, {
                {"open", "\u2016"},
                {"close", "\u2016"},
            }},
            {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/mfenced7.png", "mfenced"}, {
                {"open", "("},
                {"close", "]"},
            }},
            {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/mfenced8.png", "mfenced"}, {
                {"open", "]"},
                {"close", "]"},
            }},
        },
        {
            {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/mfenced9.png", "mfenced"}, {
                {"open", "{"},
                {"close", "}"},
            }},
            {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/mfenced10.png", "mfenced"}, {
                {"open", "\u230A"},
                {"close", "\u230B"},
            }},
            {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/mfenced11.png", "mfenced"}, {
                {"open", "|"},
                {"close", ">"},
            }},
            {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/mfenced12.png", "mfenced"}, {
                {"open", "]"},
                {"close", "["},
            }},
        },
        {
            {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/mfenced13.png", "mfenced"}, {
                {"open", "<"},
                {"close", ">"},
            }},
            {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/mfenced14.png", "mfenced"}, {
                {"open", "\u2308"},
                {"close", "\u2309"},
            }},
            {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/mfenced15.png", "mfenced"}, {
                {"open", "<"},
                {"close", "|"},
            }},
            {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/mfenced16.png", "mfenced"}, {
                {"open", "\u301A"},
                {"close", "\u301B"},
            }},
        },
    });
    addToolBarActionsElementsExt(ra, bracesMenu, {
        {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/mfenced1.png", "mfenced"}, {
            {"open", "("},
            {"close", ")"},
        }},
        {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/mfenced2.png", "mfenced"}, {
            {"open", "|"},
            {"close", "|"},
        }},
        {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/mfenced3.png", "mfenced"}, {
            {"open", "["},
            {"close", ")"},
        }},
        {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/mfenced4.png", "mfenced"}, {
            {"open", "["},
            {"close", "["},
        }},
    });
    secondToolBar->addWidget(secondButton);
    // Fenced braces ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    // Operator symbols --------------------------------------------------------
    QMenu *operatorMenu = new QMenu;
    operatorMenu->setStyleSheet(menuStyle);

    QToolButton *thirdButton = new QToolButton;
    thirdButton->setMenu(operatorMenu);
    thirdButton->setPopupMode(QToolButton::InstantPopup);
    thirdButton->setToolButtonStyle(Qt::ToolButtonIconOnly);

    addSubMenuExt(ra, operatorMenu, menuStyle, {
        {"\u2213", "\u2213", "\u2219", "\u203A"},
        {"\u00D7", "\u2297", "\u2022", "\u301A"},
        {"\u002A", "\u2299", "\u2218", "\u301B"},
    });
    addToolBarActionsExt(ra, operatorMenu, {
        "\u00B1", "\u00F7", "\u22C5", "\u2329",
    });

    QAction *operators = new QAction("operators");
    operators->setIcon(QIcon("/Users/n.mikhnenko/mathviewlib/src/backend/qt/operators.png"));
    thirdButton->setDefaultAction(operators);
    firstToolBar->addWidget(thirdButton);

    // Operator symbols ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    // Arrors symbols ----------------------------------------------------------
    QMenu *arrorsMenu = new QMenu;
    arrorsMenu->setStyleSheet(menuStyle);

    QToolButton *fourthButton = new QToolButton;
    fourthButton->setMenu(arrorsMenu);
    fourthButton->setPopupMode(QToolButton::InstantPopup);
    fourthButton->setToolButtonStyle(Qt::ToolButtonIconOnly);

    addSubMenuExt(ra, arrorsMenu, menuStyle, {
        {"\u2192", "\u21D2", "\u2197", "\u2942", "\u21B5"},
        {"\u2190", "\u21D0", "\u2199", "\u2944"},
        {"\u2195", "\u21D5", "\u2921", "\u21CC"},
        {"\u2191", "\u21D1", "\u2198", "\u2962"},
        {"\u2193", "\u21D3", "\u2196", "\u2964"},
    });
    addToolBarActionsExt(ra, arrorsMenu, {
        "\u2194", "\u21D4", "\u2922", "\u21C4", "\u21A6",
    });

    QAction *arrows = new QAction("arrows");
    arrows->setIcon(QIcon("/Users/n.mikhnenko/mathviewlib/src/backend/qt/arrows.png"));
    fourthButton->setDefaultAction(arrows);
    firstToolBar->addWidget(fourthButton);

    // Arrors symbols ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    // Logical symbols ---------------------------------------------------------
    QMenu *logicalMenu = new QMenu;
    logicalMenu->setStyleSheet(menuStyle);

    QToolButton *fifthButton = new QToolButton;
    fifthButton->setMenu(logicalMenu);
    fifthButton->setPopupMode(QToolButton::InstantPopup);
    fifthButton->setToolButtonStyle(Qt::ToolButtonIconOnly);

    addSubMenuExt(ra, logicalMenu, menuStyle, {
        {"\u2235", "\u2203", "\u00AC", "\u2228"},
    });
    addToolBarActionsExt(ra, logicalMenu, {
        "\u2234", "\u220B", "\u2200", "\u2227",
    });

    QAction *logical = new QAction("logical");
    logical->setIcon(QIcon("/Users/n.mikhnenko/mathviewlib/src/backend/qt/logical.png"));
    fifthButton->setDefaultAction(logical);
    firstToolBar->addWidget(fifthButton);

    // Logical symbols ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    // SetTheory symbols -------------------------------------------------------
    QMenu *setTheoryMenu = new QMenu;
    setTheoryMenu->setStyleSheet(menuStyle);

    QToolButton *sixthButton = new QToolButton;
    sixthButton->setMenu(setTheoryMenu);
    sixthButton->setPopupMode(QToolButton::InstantPopup);
    sixthButton->setToolButtonStyle(Qt::ToolButtonIconOnly);

    addSubMenuExt(ra, setTheoryMenu, menuStyle, {
        {"\u2209", "\u2229", "\u2283", "\u2287", "\u2205"},
    });
    addToolBarActionsExt(ra, setTheoryMenu, {
        "\u2208", "\u222A", "\u2282", "\u2286", "\u2284",
    });

    QAction *setTheory = new QAction("setTheory");
    setTheory->setIcon(QIcon("/Users/n.mikhnenko/mathviewlib/src/backend/qt/setTheory.png"));
    sixthButton->setDefaultAction(setTheory);
    firstToolBar->addWidget(sixthButton);

    // SetTheory symbols ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setMargin(0);

    layout->addWidget(firstToolBar);
    layout->addWidget(secondToolBar);
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

QAction *
addToolBarActionsElements(Qt_RenderArea* ra, QMenu *alignMenu, std::string icon, std::string name, std::map<string, string> opts)
{
    QAction *preceds = new QAction(ra);
    preceds->setIcon(QIcon(QString::fromStdString(icon)));
    alignMenu->addAction(preceds);
    ra->connect(preceds, &QAction::triggered, ra, [ra, name, opts]{ ra->insertElementAfterCursor(name, opts); });
    return preceds;
}

QAction *
addToolBarActionsElementsExt(Qt_RenderArea* ra, QMenu *alignMenu, std::map<std::pair<std::string, std::string>, std::map<std::string, std::string>> actions)
{
    QAction *defAct = addToolBarActionsElements(ra, alignMenu, actions.begin()->first.first, actions.begin()->first.second, actions.begin()->second);
    actions.erase(actions.begin());
    for (const auto & i : actions)
        addToolBarActionsElements(ra, alignMenu, i.first.first, i.first.second, i.second);
    return defAct;
}

void
addSubMenuElementsExt(Qt_RenderArea* ra, QMenu *alignMenu, QString menuStyle, std::vector<std::map<std::pair<std::string, std::string>, std::map<std::string, std::string>>> actions)
{
    if (actions.empty())
        return;

    QMenu *alignMenu2 = new QMenu("...");
    std::map<std::pair<std::string, std::string>, std::map<std::string, std::string>> act = actions[0];
    actions.erase(actions.begin());
    addSubMenuElementsExt(ra, alignMenu2, menuStyle, actions);
    addToolBarActionsElementsExt(ra, alignMenu2, act);
    alignMenu2->setStyleSheet(menuStyle);
    alignMenu->addMenu(alignMenu2);
}
