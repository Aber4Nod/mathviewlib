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

QToolButton *makeSumsMenu(Qt_RenderArea* ra, QString menuStyle);
QToolButton *makeIntegralsMenu(Qt_RenderArea* ra, QString menuStyle);
QToolButton *makeBarsMenu(Qt_RenderArea* ra, QString menuStyle);
QToolButton *makeSetsMenu(Qt_RenderArea* ra, QString menuStyle);
QToolButton *makeTablesMenu(Qt_RenderArea* ra, QString menuStyle);

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

    // Miscellanious symbols ---------------------------------------------------
    QMenu *miscellaniousMenu = new QMenu;
    miscellaniousMenu->setStyleSheet(menuStyle);

    QToolButton *seventhButton = new QToolButton;
    seventhButton->setMenu(miscellaniousMenu);
    seventhButton->setPopupMode(QToolButton::InstantPopup);
    seventhButton->setToolButtonStyle(Qt::ToolButtonIconOnly);

    addSubMenuExt(ra, miscellaniousMenu, menuStyle, {
        {"\u2118", "\u2124", "\u210F", "\u2207", "\u220F", "\u2221", "\u25A1"},
        {"\u2135", "\u2115", "\u2020", "\u22C4", "\u00B0", "\u2225", "\u25CB"},
        {"\u2102", "\u019B", "\u03A9", "\u2210", "\u2222", "\u25AD"},
        {"\u2113", "\u2127", "\u222B", "\u27C2", "\u25B1"},
    });
    addToolBarActionsExt(ra, miscellaniousMenu, {
        "\u2202", "\u211D", "\u221E", "\u0394", "\u2211", "\u2220", "\u25B3",
    });

    QAction *miscellanious = new QAction("setTheory");
    miscellanious->setIcon(QIcon("/Users/n.mikhnenko/mathviewlib/src/backend/qt/miscellanious.png"));
    seventhButton->setDefaultAction(miscellanious);
    firstToolBar->addWidget(seventhButton);

    // Miscellanious symbols ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    // GreekSmall symbols ------------------------------------------------------
    QMenu *greekSmallMenu = new QMenu;
    greekSmallMenu->setStyleSheet(menuStyle);

    QToolButton *eightButton = new QToolButton;
    eightButton->setMenu(greekSmallMenu);
    eightButton->setPopupMode(QToolButton::InstantPopup);
    eightButton->setToolButtonStyle(Qt::ToolButtonIconOnly);

    addSubMenuExt(ra, greekSmallMenu, menuStyle, {
        {"\u03B2", "\u03D5", "\u03B9", "\u03BD", "\u03B8", "\u03C2", "\u03BE"},
        {"\u03C7", "\u03C6", "\u03BA", "\u03BF", "\u03D1", "\u03C4", "\u03C8"},
        {"\u03B4", "\u03B3", "\u03BB", "\u03C0", "\u03C1", "\u03C5", "\u03B6"},
    });
    addToolBarActionsExt(ra, greekSmallMenu, {
        "\u03B1", "\u03B5", "\u03B7", "\u03BC", "\u03D6", "\u03C3", "\u03C9",
    });

    QAction *greekSmall = new QAction("setTheory");
    greekSmall->setIcon(QIcon("/Users/n.mikhnenko/mathviewlib/src/backend/qt/greekSmall.png"));
    eightButton->setDefaultAction(greekSmall);
    firstToolBar->addWidget(eightButton);

    // GreekSmall symbols ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    // GreekCapital symbols ----------------------------------------------------
    QMenu *greekCapitalMenu = new QMenu;
    greekCapitalMenu->setStyleSheet(menuStyle);

    QToolButton *ninthButton = new QToolButton;
    ninthButton->setMenu(greekCapitalMenu);
    ninthButton->setPopupMode(QToolButton::InstantPopup);
    ninthButton->setToolButtonStyle(Qt::ToolButtonIconOnly);

    addSubMenuExt(ra, greekCapitalMenu, menuStyle, {
        {"\u0392", "\u03A6", "\u039A", "\u039F", "\u03A3", "\u039E"},
        {"\u03A7", "\u0393", "\u039B", "\u03A0", "\u03A4", "\u03A8"},
        {"\u0394", "\u0397", "\u039C", "\u0398", "\u03D2", "\u0396"},
    });
    addToolBarActionsExt(ra, greekCapitalMenu, {
        "\u0391", "\u0395", "\u0399", "\u039D", "\u03A1", "\u03A9",
    });

    QAction *greekCapital = new QAction("setTheory");
    greekCapital->setIcon(QIcon("/Users/n.mikhnenko/mathviewlib/src/backend/qt/greekCapital.png"));
    ninthButton->setDefaultAction(greekCapital);
    firstToolBar->addWidget(ninthButton);

    // GreekCapital symbols ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    // Fracts  -----------------------------------------------------------------
    QMenu *fractsMenu = new QMenu("fracts");
    fractsMenu->setStyleSheet(menuStyle);
    QToolButton *fractsButton = new QToolButton();

    QAction *fracts = new QAction("fracts");
    fracts->setIcon(QIcon("/Users/n.mikhnenko/mathviewlib/src/backend/qt/fracts.png"));

    fractsButton->setMenu(fractsMenu);
    fractsButton->setPopupMode(QToolButton::InstantPopup);
    fractsButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    fractsButton->setDefaultAction(fracts);
    addSubMenuElementsExt(ra, fractsMenu, menuStyle, {
        {
            {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/fracts2.png", "mfrac"}, {
                {"bevelled", "true"},
            }},
            {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/fracts4.png", "mroot"}, {}
            },
        },
    });
    addToolBarActionsElementsExt(ra, fractsMenu, {
        {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/fracts1.png", "mfrac"}, {}
        },
        {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/fracts3.png", "msqrt"}, {}
        },
    });
    secondToolBar->addWidget(fractsButton);
    // Fracts ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    // UnderOvers --------------------------------------------------------------
    QMenu *underoverMenu = new QMenu("underovers");
    underoverMenu->setStyleSheet(menuStyle);
    QToolButton *underoverButton = new QToolButton();

    QAction *underovers = new QAction("underovers");
    underovers->setIcon(QIcon("/Users/n.mikhnenko/mathviewlib/src/backend/qt/underovers.png"));

    underoverButton->setMenu(underoverMenu);
    underoverButton->setPopupMode(QToolButton::InstantPopup);
    underoverButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    underoverButton->setDefaultAction(underovers);
    addSubMenuElementsExt(ra, underoverMenu, menuStyle, {
        {
            {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/underovers2.png", "munder"}, {}
            },
            {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/underovers5.png", "msub"}, {}
            },
        },
        {
            {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/underovers3.png", "munderover"}, {}
            },
            {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/underovers6.png", "msubsup"}, {}
            },
        },
    });
    addToolBarActionsElementsExt(ra, underoverMenu, {
        {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/underovers1.png", "mover"}, {}
        },
        {{"/Users/n.mikhnenko/mathviewlib/src/backend/qt/underovers4.png", "msup"}, {}
        },
    });
    secondToolBar->addWidget(underoverButton);
    // UnderOvers ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    secondToolBar->addWidget(makeSumsMenu(ra, menuStyle));
    secondToolBar->addWidget(makeIntegralsMenu(ra, menuStyle));
    secondToolBar->addWidget(makeBarsMenu(ra, menuStyle));
    secondToolBar->addWidget(makeSetsMenu(ra, menuStyle));
    secondToolBar->addWidget(makeTablesMenu(ra, menuStyle));

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

// Sums ------------------------------------------------------------------------
QToolButton *makeSumsMenu(Qt_RenderArea* ra, QString menuStyle)
{
    QMenu *sumsMenu = new QMenu("sums");
    sumsMenu->setStyleSheet(menuStyle);
    QToolButton *sumsButton = new QToolButton();

    QAction *sums = new QAction("sums");
    sums->setIcon(QIcon("/Users/n.mikhnenko/mathviewlib/src/backend/qt/sums.png"));

    sumsButton->setMenu(sumsMenu);
    sumsButton->setPopupMode(QToolButton::InstantPopup);
    sumsButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    sumsButton->setDefaultAction(sums);
    addSubMenuElementsExt(ra, sumsMenu, menuStyle, {
        {
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/sums3.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<mrow>"
                                "<munder>"
                                    "<mo movablelimits=\"false\">&Sum;</mo>"
                                    "<mi/>"
                                "</munder>"
                                "<mi/>"
                            "</mrow>"
                        "</math>"
                }, {}
            },
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/sums4.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<mrow>"
                                "<msubsup>"
                                    "<mo movablelimits=\"false\">&Sum;</mo>"
                                    "<mi/>"
                                    "<mi/>"
                                "</msubsup>"
                                "<mi/>"
                            "</mrow>"
                        "</math>"
                }, {}
            },
        },
        {
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/sums5.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<mrow>"
                                "<munderover>"
                                    "<mo movablelimits=\"false\">&Sum;</mo>"
                                    "<mi/>"
                                    "<mi/>"
                                "</munderover>"
                                "<mi/>"
                            "</mrow>"
                        "</math>"
                }, {}
            },
        },
    });
    addToolBarActionsElementsExt(ra, sumsMenu, {
        {
            {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/sums1.png",
                    "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                        "<mrow>"
                            "<mo movablelimits=\"false\">&Sum;</mo>"
                            "<mi></mi>"
                        "</mrow>"
                    "</math>"
            }, {}
        },
        {
            {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/sums2.png",
                    "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                        "<mrow>"
                            "<msub>"
                                "<mo movablelimits=\"false\">&Sum;</mo>"
                                "<mi/>"
                            "</msub>"
                            "<mi/>"
                        "</mrow>"
                    "</math>"
            }, {}
        },
    });

    return sumsButton;
}
// Sums ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// Integrals -------------------------------------------------------------------
QToolButton *makeIntegralsMenu(Qt_RenderArea* ra, QString menuStyle)
{
    QMenu *integralsMenu = new QMenu("integrals");
    integralsMenu->setStyleSheet(menuStyle);
    QToolButton *integralsButton = new QToolButton();

    QAction *integrals = new QAction("integrals");
    integrals->setIcon(QIcon("/Users/n.mikhnenko/mathviewlib/src/backend/qt/integrals.png"));

    integralsButton->setMenu(integralsMenu);
    integralsButton->setPopupMode(QToolButton::InstantPopup);
    integralsButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    integralsButton->setDefaultAction(integrals);
    addSubMenuElementsExt(ra, integralsMenu, menuStyle, {
        {
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/integrals6.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<mrow>"
                                "<munderover>"
                                    "<mo movablelimits=\"false\">&#x222B;</mo>"
                                    "<mi/>"
                                    "<mi/>"
                                "</munderover>"
                                "<mi/>"
                            "</mrow>"
                        "</math>"
                }, {}
            },
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/integrals7.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<mrow>"
                                "<munder>"
                                    "<mo movablelimits=\"false\">&#x222B;</mo>"
                                    "<mi/>"
                                "</munder>"
                                "<mi/>"
                            "</mrow>"
                        "</math>"
                }, {}
            },
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/integrals8.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<mrow>"
                                "<munder>"
                                    "<mo movablelimits=\"false\">&#x222C;</mo>"
                                    "<mi/>"
                                "</munder>"
                                "<mi/>"
                            "</mrow>"
                        "</math>"
                }, {}
            },
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/integrals9.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<mrow>"
                                "<munder>"
                                    "<mo movablelimits=\"false\">&#x222D;</mo>"
                                    "<mi/>"
                                "</munder>"
                                "<mi/>"
                            "</mrow>"
                        "</math>"
                }, {}
            },
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/integrals10.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<mrow>"
                                "<munder>"
                                    "<mo movablelimits=\"false\">&#x222E;</mo>"
                                    "<mi/>"
                                "</munder>"
                                "<mi/>"
                            "</mrow>"
                        "</math>"
                }, {}
            },
        },

        {
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/integrals11.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<mrow>"
                                "<msupsup>"
                                    "<mo movablelimits=\"false\">&#x222B;</mo>"
                                    "<mi/>"
                                    "<mi/>"
                                "</msupsup>"
                                "<mi/>"
                            "</mrow>"
                        "</math>"
                }, {}
            },
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/integrals12.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<mrow>"
                                "<msub>"
                                    "<mo movablelimits=\"false\">&#x222B;</mo>"
                                    "<mi/>"
                                "</msub>"
                                "<mi/>"
                            "</mrow>"
                        "</math>"
                }, {}
            },
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/integrals13.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<mrow>"
                                "<msub>"
                                    "<mo movablelimits=\"false\">&#x222C;</mo>"
                                    "<mi/>"
                                "</msub>"
                                "<mi/>"
                            "</mrow>"
                        "</math>"
                }, {}
            },
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/integrals14.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<mrow>"
                                "<msub>"
                                    "<mo movablelimits=\"false\">&#x222D;</mo>"
                                    "<mi/>"
                                "</msub>"
                                "<mi/>"
                            "</mrow>"
                        "</math>"
                }, {}
            },
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/integrals15.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<mrow>"
                                "<msub>"
                                    "<mo movablelimits=\"false\">&#x222E;</mo>"
                                    "<mi/>"
                                "</msub>"
                                "<mi/>"
                            "</mrow>"
                        "</math>"
                }, {}
            },
        },
    });
    addToolBarActionsElementsExt(ra, integralsMenu, {
        {
            {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/integrals1.png",
                    "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                        "<mrow>"
                            "<mo movablelimits=\"false\">&#x222B;</mo>"
                            "<mi/>"
                        "</mrow>"
                    "</math>"
            }, {}
        },
        {
            {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/integrals3.png",
                    "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                        "<mrow>"
                            "<mo movablelimits=\"false\">&#x222C;</mo>"
                            "<mi/>"
                        "</mrow>"
                    "</math>"
            }, {}
        },
        {
            {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/integrals4.png",
                    "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                        "<mrow>"
                            "<mo movablelimits=\"false\">&#x222D;</mo>"
                            "<mi/>"
                        "</mrow>"
                    "</math>"
            }, {}
        },
        {
            {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/integrals5.png",
                    "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                        "<mrow>"
                            "<mo movablelimits=\"false\">&#x222E;</mo>"
                            "<mi/>"
                        "</mrow>"
                    "</math>"
            }, {}
        },
    });

    return integralsButton;
}
// Integrals ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// Bars ------------------------------------------------------------------------
QToolButton
*makeBarsMenu(Qt_RenderArea* ra, QString menuStyle)
{
    QMenu *barsMenu = new QMenu("bars");
    barsMenu->setStyleSheet(menuStyle);
    QToolButton *barsButton = new QToolButton();

    QAction *bars = new QAction("bars");
    bars->setIcon(QIcon("/Users/n.mikhnenko/mathviewlib/src/backend/qt/bars.png"));

    barsButton->setMenu(barsMenu);
    barsButton->setPopupMode(QToolButton::InstantPopup);
    barsButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    barsButton->setDefaultAction(bars);
    addSubMenuElementsExt(ra, barsMenu, menuStyle, {
        {
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/bars5.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<mover>"
                                "<mi/>"
                                "<mo>\u2227</mo>"
                            "</mover>"
                        "</math>"
                }, {}
            },
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/bars7.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<mover>"
                                "<mi/>"
                                "<mo>&#8592;</mo>"
                            "</mover>"
                        "</math>"
                }, {}
            },
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/bars8.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<munder>"
                                "<mi/>"
                                "<mo>&#8592;</mo>"
                            "</munder>"
                        "</math>"
                }, {}
            },
        },
    });
    addToolBarActionsElementsExt(ra, barsMenu, {
        {
            {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/bars1.png",
                    "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                        "<mover>"
                            "<mi/>"
                            "<mo>&#x223C;<mo>"
                        "</mover>"
                    "</math>"
            }, {}
        },
        {
            {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/bars2.png",
                    "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                        "<mover>"
                            "<mi/>"
                            "<mo>&#x2015;<mo>"
                        "</mover>"
                    "</math>"
            }, {}
        },
        {
            {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/bars3.png",
                    "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                        "<mover>"
                            "<mi/>"
                            "<mo>&#x2192;<mo>"
                        "</mover>"
                    "</math>"
            }, {}
        },
        {
            {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/bars4.png",
                    "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                        "<munder>"
                            "<mi/>"
                            "<mo>&#x2192;<mo>"
                        "</munder>"
                    "</math>"
            }, {}
        },
    });

    return barsButton;
}
// Bars ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// Sets ------------------------------------------------------------------------
QToolButton
*makeSetsMenu(Qt_RenderArea* ra, QString menuStyle)
{
    QMenu *setsMenu = new QMenu("bars");
    setsMenu->setStyleSheet(menuStyle);
    QToolButton *setsButton = new QToolButton();

    QAction *sets = new QAction("sets");
    sets->setIcon(QIcon("/Users/n.mikhnenko/mathviewlib/src/backend/qt/sets1.png"));

    setsButton->setMenu(setsMenu);
    setsButton->setPopupMode(QToolButton::InstantPopup);
    setsButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    setsButton->setDefaultAction(sets);
    addSubMenuElementsExt(ra, setsMenu, menuStyle, {
        {
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/sets5.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<mrow>"
                                "<munder>"
                                    "<mo movablelimits=\"false\">&prod;</mo>"
                                    "<mi/>"
                                "</munder>"
                                "<mi/>"
                            "</mrow>"
                        "</math>"
                }, {}
            },
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/sets6.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<mrow>"
                                "<munder>"
                                    "<mo movablelimits=\"false\">&#x22c2;</mo>"
                                    "<mi/>"
                                "</munder>"
                                "<mi/>"
                            "</mrow>"
                        "</math>"
                }, {}
            },
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/sets7.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<mrow>"
                                "<munder>"
                                    "<mo movablelimits=\"false\">&#x22c3;</mo>"
                                    "<mi/>"
                                "</munder>"
                                "<mi/>"
                            "</mrow>"
                        "</math>"
                }, {}
            },
        },

        {
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/sets8.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<mrow>"
                                "<munderover>"
                                    "<mo movablelimits=\"false\">&prod;</mo>"
                                    "<mi/>"
                                    "<mi/>"
                                "</munderover>"
                                "<mi/>"
                            "</mrow>"
                        "</math>"
                }, {}
            },
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/sets9.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<mrow>"
                                "<munderover>"
                                    "<mo movablelimits=\"false\">&#x22c2;</mo>"
                                    "<mi/>"
                                    "<mi/>"
                                "</munderover>"
                                "<mi/>"
                            "</mrow>"
                        "</math>"
                }, {}
            },
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/sets10.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<mrow>"
                                "<munderover>"
                                    "<mo movablelimits=\"false\">&#x22c3;</mo>"
                                    "<mi/>"
                                    "<mi/>"
                                "</munderover>"
                                "<mi/>"
                            "</mrow>"
                        "</math>"
                }, {}
            },
        },
    });
    addToolBarActionsElementsExt(ra, setsMenu, {
        {
            {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/sets2.png",
                    "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                        "<mrow>"
                            "<mo movablelimits=\"false\">&prod;</mo>"
                            "<mi></mi>"
                        "</mrow>"
                    "</math>"
            }, {}
        },
        {
            {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/sets3.png",
                    "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                        "<mrow>"
                            "<mo movablelimits=\"false\">&#x22c2;</mo>"
                            "<mi/>"
                        "</mrow>"
                    "</math>"
            }, {}
        },
        {
            {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/sets4.png",
                    "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                        "<mrow>"
                            "<mo movablelimits=\"false\">&#x22c3;</mo>"
                            "<mi/>"
                        "</mrow>"
                    "</math>"
            }, {}
        },
    });

    return setsButton;
}
// Sets ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// Tables ----------------------------------------------------------------------
QToolButton
*makeTablesMenu(Qt_RenderArea* ra, QString menuStyle)
{
    QMenu *tablesMenu = new QMenu("tables");
    tablesMenu->setStyleSheet(menuStyle);
    QToolButton *tablesButton = new QToolButton();

    QAction *tables = new QAction("tables");
    tables->setIcon(QIcon("/Users/n.mikhnenko/mathviewlib/src/backend/qt/tables.png"));

    tablesButton->setMenu(tablesMenu);
    tablesButton->setPopupMode(QToolButton::InstantPopup);
    tablesButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    tablesButton->setDefaultAction(tables);
    addSubMenuElementsExt(ra, tablesMenu, menuStyle, {
        {
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/tables3.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<mtable>"
                            "<mtr>"
                                  "<mtd><mi/></mtd>"
                             "</mtr>"
                             "<mtr>"
                                   "<mtd><mi/></mtd>"
                              "</mtr>"
                             "</table>"
                        "</math>"
                }, {}
            },
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/tables4.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<mtable>"
                            "<mtr>"
                                  "<mtd><mi/></mtd>"
                             "</mtr>"
                             "<mtr>"
                                   "<mtd><mi/></mtd>"
                              "</mtr>"
                              "<mtr>"
                                    "<mtd><mi/></mtd>"
                               "</mtr>"
                             "</table>"
                        "</math>"
                }, {}
            },
        },
    
        {
            {
                {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/tables5.png",
                        "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                            "<mtable>"
                            "<mtr>"
                                  "<mtd><mi/></mtd>"
                                  "<mtd><mi/></mtd>"
                             "</mtr>"
                             "<mtr>"
                                   "<mtd><mi/></mtd>"
                                  "<mtd><mi/></mtd>"
                              "</mtr>"
                             "</table>"
                        "</math>"
                }, {}
            },
        },
    });
    addToolBarActionsElementsExt(ra, tablesMenu, {
        {
            {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/tables1.png",
                    "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                        "<mtable>"
                        "<mtr>"
                              "<mtd><mi/></mtd>"
                              "<mtd><mi/></mtd>"
                         "</mtr>"
                         "</table>"
                    "</math>"
            }, {}
        },
        {
            {"/Users/n.mikhnenko/mathviewlib/src/backend/qt/tables2.png",
                    "<math display=\"inline\" xmlns=\"http://www.w3.org/1998/Math/MathML\">"
                        "<mtable>"
                        "<mtr>"
                              "<mtd><mi/></mtd>"
                              "<mtd><mi/></mtd>"
                              "<mtd><mi/></mtd>"
                         "</mtr>"
                         "</table>"
                    "</math>"
            }, {}
        },
    });

    return tablesButton;
}
// Tables ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
