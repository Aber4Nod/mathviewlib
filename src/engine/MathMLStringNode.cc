// Copyright (C) 2000-2007, Luca Padovani <padovani@sti.uniurb.it>.
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

#include <cassert>

#include "MathMLElement.hh"
#include "MathMLStringNode.hh"
#include "FormattingContext.hh"
#include "MathGraphicDevice.hh"
#include <iostream>
#include "HorizontalArrayArea.hh"
#include "MathMLTokenElement.hh"

inline bool
isCombining(Char32 ch)
{
  return (ch >= 0x0300 && ch <= 0x0362) || (ch >= 0x20d0 && ch <= 0x20e8);
}

MathMLStringNode::MathMLStringNode(const String& c)
  : content(c)
{
    std::cout << "creating string node: " << this << " with content: "  << c << std::endl;
}

MathMLStringNode::~MathMLStringNode()
{ }

AreaRef
MathMLStringNode::format(FormattingContext& ctxt)
{
    String currentContent = content;
    if (cursorIndex >= 0) {
        if (currentFormattingIndex == 0) {
            currentContent = content.substr(0, cursorIndex + 1);
            currentFormattingIndex = cursorIndex + 1;
        }
        else {
            currentContent = content.substr(currentFormattingIndex);
            // currentFormattingIndex = 0;
        }
    }
    std::cout << "[MathMLStringNode::format]: cursorIndex: " << cursorIndex << "; formatting value: " << currentContent << std::endl;
    // todo go threw area and set this node to them
    AreaRef cont = ctxt.MGD()->string(ctxt, currentContent);
    SmartPtr<const HorizontalArrayArea> harea = smart_cast<const HorizontalArrayArea>(cont);
    if (harea != nullptr)
        harea->setNode(this);
    v_area.push_back(cont);
    std::cout << "[MathMLStringNode::format]: v_area size: " << v_area.size() << std::endl;

    // else
    // {
        // cont = ctxt.MGD()->dummy(ctxt); // make this dumm clever one and know his parent (right now there is arearef that if there and dummy is just what we see)
        // harea->setNode(this);
    // }

    std::cout << "setting mathmlstringnode to this harea: " << harea << std::endl;
    return cont; 
}

unsigned
MathMLStringNode::GetLogicalContentLength() const
{
  UCS4String s = UCS4StringOfString(content);

  unsigned length = 0;
  for (UCS4String::const_iterator i = s.begin(); i != s.end(); i++)
    {
      if (!isCombining(*i) || i == s.begin())
	length++;
    }

  return length;
}

String
MathMLStringNode::GetRawContent() const
{ return content; }

void
MathMLStringNode::LookUpContent() const
{ std::cout << "Content: '" << content << "'" << std::endl; }

void
MathMLStringNode::ClearContent()
{
    // content.push_back('1');
    // content.clear();
    content = "к";
    getParentElement()->setDirtyLayout();
    getParentElement()->setDirtyStructure();
    getParentElement()->setContentSet();
    getParentElement()->setDirtyAttribute();
    // delete getParentElement();
}

void
MathMLStringNode::DeleteContent()
{
    // content = "к";
    // content.clear();
    // std::cout << "setting dirty structure for parent element: " << getParentElement() << std::endl;
    getParentElement()->setDirtyLayout();
    getParentElement()->setDirtyStructure();
    // getParentElement()->setContentSet();
    getParentElement()->setDirtyAttribute();
    // delete getParentElement();
}

void
MathMLStringNode::DeleteGlyph(uint32_t index)
{
    std::cout << "[MathMLStringNode::DeleteGlyph]: got index: " << index << std::endl;
    if (index >= content.length())
        return;
    content.erase(index, 1);
    std::cout << "[MathMLStringNode::DeleteGlyph]: content after erasing: " << content << std::endl;
    getParentElement()->setDirtyLayout();
    getParentElement()->setDirtyStructure();
    getParentElement()->setContentSet();
}

void
MathMLStringNode::InsertGlyphAfter(int32_t index, char glyph)
{
    std::cout << "[MathMLStringNode::InsertGlyphAfter]: got index: " << index << std::endl;
    std::cout << "[MathMLStringNode::InsertGlyphAfter]: current content: " << content << " length: " << content.length() << std::endl;
    if (content.length() == 0)
        content.push_back(glyph);
    else
    if (index >= 0 && index >= content.length())
    {
        std::cout << "[MathMLStringNode::InsertGlyphAfter]: index exceeded number of glyphes, pushing back: " << std::endl;
        content.push_back(glyph);
    }
    else
        content.insert(index + 1, 1, glyph);
    std::cout << "[MathMLStringNode::DeleteGlyph]: content after erasing: " << content << std::endl;
    getParentElement()->setDirtyLayout();
    getParentElement()->setDirtyStructure();
    getParentElement()->setContentSet();
}

void
MathMLStringNode::InsertGlyphBefore(int32_t index, char glyph)
{
    std::cout << "[MathMLStringNode::InsertGlyphBefore]: got index: " << index << std::endl;
    if (content.length() == 0 || index == -1)
        content.push_back(glyph);
    else
    if (index >= content.length())
    {
        std::cout << "[MathMLStringNode::InsertGlyphAfter]: index exceeded number of glyphes, pushing back: " << std::endl;
        content.push_back(glyph);
    }
    else
        content.insert(index, 1, glyph);
    std::cout << "[MathMLStringNode::DeleteGlyph]: content after erasing: " << content << std::endl;
    getParentElement()->setDirtyLayout();
    getParentElement()->setDirtyStructure();
    getParentElement()->setContentSet();
}

void
MathMLStringNode::DeleteParentElement() // deletion also must be supported w/ binding to stringnode
{
    getParentElement()->setDirtyLayout();
    getParentElement()->setDirtyStructure();
    getParentElement()->setDeleteSet();
}

void
MathMLStringNode::InsertElementAfter()
{
    std::cout << "[MathMLStringNode::InsertElementAfter]: inserting element after" << std::endl;
    getParentElement()->setDirtyLayout();
    getParentElement()->setDirtyStructure();
    getParentElement()->setInsertSet();
}

void
MathMLStringNode::insertElementCursor()
{
    std::cout << "[MathMLStringNode::insertElementCursor]: inserting element after" << std::endl;
    getParentElement()->setDirtyLayout();
    getParentElement()->setDirtyStructure();
    getParentElement()->setInsertSetCursor();
}

void
MathMLStringNode::insertInnerElementCursor(uint32_t index)
{
    std::cout << "[MathMLStringNode::insertInnerElementCursor]: setting cursor index: " << index << std::endl;
    // std::cout << "[MathMLStringNode::insertInnerElementCursor]: setting cursor index: " << v_area.size() << std::endl;
    static_cast<MathMLTokenElement *>(getParentElement())->setCursorPosition(this, index);
    getParentElement()->setDirtyLayout();
    getParentElement()->setDirtyStructure();
    getParentElement()->setCursorSet();
}

uint32_t
MathMLStringNode::normalizeGlyphAreaIndex(AreaRef area, uint32_t index)
{
    uint32_t curIndex = 0;
    for (const auto & c_area : v_area)
    {
        if (c_area == area) {
            std::cout << "[MathMLStringNode::normalizeGlyphAreaIndex]: founded area, returning index: " << curIndex + index << std::endl;
            return curIndex + index;
        }
        // TODO: assuming that glyphstringarea\s (whose has link to this node) has only one child element
        curIndex += smart_cast<const LinearContainerArea>(c_area)->getChildren()[0]->size();
        std::cout << "[MathMLStringNode::normalizeGlyphAreaIndex]: incrementing index by size: " << c_area->size() << " current size: " << curIndex << std::endl;
    }
    return index;
}
