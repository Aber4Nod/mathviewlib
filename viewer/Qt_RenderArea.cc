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

#include "Qt_RenderArea.hh"
#include "Qt_GlyphArea.hh"
#include "defs.h"
#include <unistd.h>
#include <QPainter>
#include <QRawFont>
#include <QDebug>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QFileDialog>

Qt_RenderArea::Qt_RenderArea(SmartPtr<AbstractLogger> logger,
                       QWidget* parent)
    : QWidget(parent)
{
    m_rawFont = QRawFont::fromFont(QFont(DEFAULT_FONT_FAMILY, 32));
    m_backend = Qt_Backend::create(m_rawFont);
    m_device = m_backend->getMathGraphicDevice();
    m_dictionary = MathMLOperatorDictionary::create();
    m_view = MathView::create(logger);
    m_view->setOperatorDictionary(m_dictionary);
    m_view->setMathMLNamespaceContext(MathMLNamespaceContext::create(m_view, m_device));
    m_view->setDefaultFontSize(32);
    setFocusPolicy(Qt::StrongFocus);
}

Qt_RenderArea::~Qt_RenderArea()
{
    m_view->resetRootElement();
}

void Qt_RenderArea::loadURI(const char* mml_file) {
    m_view->loadURI(mml_file);
    qDebug() << "Getting bounding box!";
    const BoundingBox box = m_view->getBoundingBox();
    qDebug() << "Got bounding box123!";
    qreal width = Qt_RenderingContext::toQtPixels(box.horizontalExtent());
    qreal height = Qt_RenderingContext::toQtPixels(box.verticalExtent());
    //qDebug() << width << height;
    setMinimumSize(QSizeF(width, height).toSize());
    repaint();
}

void Qt_RenderArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    m_rc.setPainter(&painter);
    m_view->render(m_rc, scaled::zero(), -m_view->getBoundingBox().height);
}

void Qt_RenderArea::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QPointF pos = event->pos();
        qDebug() << "[Qt_RenderArea::mousePressEvent]: pressed on pos: " << pos;

        qDebug() << "[Qt_RenderArea::mousePressEvent]: widget pos: " << this->pos();
        
        // -- deletion of glyph
        // if (m_view->deleteGlyph(pos.x(), pos.y()));
            // repaint();

        // -- insertion of glyph after
        // if (m_view->insertGlyphAfter(pos.x(), pos.y(), 'q'))
            // repaint();

        // -- insertion of glyph before
        // if (m_view->insertGlyphBefore(pos.x(), pos.y(), 'q'))
            // repaint();

        // -- lookup content of stringnode
        // m_view->lookUpContent(pos.x(), pos.y());

        // -- deletion of element here
        // if (m_view->deleteElement(pos.x(), pos.y()))
            // repaint();

        // -- addition of element after here
        // if (m_view->insertElementAfter(pos.x(), pos.y()))
            // repaint();

        // -- addition of cursor after here
        // if (m_view->insertElementCursor(pos.x() - this->pos().x(), pos.y() - this->pos().y()))
            // repaint();

        if (m_view->insertElementCursor(pos.x(), pos.y())) {
            repaint();
        }

        // Functionalty for dumping current snapshot of mathml
        // QFile qf("TestFileDump.xml");
        // qf.open(QIODevice::WriteOnly);
        // int fd = qf.handle();
        // FILE* f = fdopen(dup(fd), "wb");
        // m_view->dumpDocument(f);
        // fclose(f);
    }
}

void Qt_RenderArea::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    qDebug() << "[Qt_RenderArea::keyPressEvent]: pressed on key: " << key;
    if (key == Qt::Key_Q && (event->modifiers() & Qt::ShiftModifier))
    {
        if (m_view->insertElementAfterCursor("munderover"))
            repaint();
    }
    else
    if (key >= Qt::Key_Space && key <= Qt::Key_AsciiTilde && !event->modifiers())
    {
        // if (m_view->insertElementAfterCursor(std::tolower(key)))
            // repaint();
        m_view->insertGlyphAfterCursor(std::tolower(key));
        repaint();
    }
    else
    if (key == Qt::Key_Backspace)
    {
        m_view->deleteGLyphBeforeCursor();
        repaint();
    }
    else
    if (key == Qt::Key_Left)
    {
        if (event->modifiers() & Qt::ShiftModifier)
            m_view->stepCursorLeft();
        else
            m_view->moveCursorLeft();
        repaint();
        m_view->dumpDocument(stdout);
    }
    else
    if (key == Qt::Key_Right)
    {
        if (event->modifiers() & Qt::ShiftModifier)
            m_view->stepCursorRight();
        else
            m_view->moveCursorRight();
        repaint();
        m_view->dumpDocument(stdout);
    }
}

void
Qt_RenderArea::slotNew()
{
    m_view->loadNewFormula();
    const BoundingBox box = m_view->getBoundingBox();
    qDebug() << "Got bounding box123!";
    qreal width = Qt_RenderingContext::toQtPixels(box.horizontalExtent());
    qreal height = Qt_RenderingContext::toQtPixels(box.verticalExtent());
    //qDebug() << width << height;
    setMinimumSize(QSizeF(width, height).toSize());
    repaint();
}

void
Qt_RenderArea::slotLoad()
{
    m_strFileName=QFileDialog::getOpenFileName();
    if (m_strFileName.isEmpty()) {
        return;
    }

    loadURI(m_strFileName.toUtf8());
}

void
Qt_RenderArea::slotSaveAs()
{
    QString str = QFileDialog::getSaveFileName(0, m_strFileName);
    if (!str.isEmpty()) {
        m_strFileName = str;
        slotSave();
    }
}

void
Qt_RenderArea::slotSave()
{
    if (m_strFileName.isEmpty()) {
        slotSaveAs();
       // return;
    }

    QFile file(m_strFileName);
    if (file.open(QIODevice::WriteOnly)) {
        int fd = file.handle();
        FILE* f = fdopen(dup(fd), "wb");
        m_view->dumpDocument(f);
        fclose(f);
    }
}

void
Qt_RenderArea::close()
{
    m_view->unload();
}

void
Qt_RenderArea::insert(std::string name)
{
    if (m_view->insertElementAfterCursor(name))
        repaint();
}
