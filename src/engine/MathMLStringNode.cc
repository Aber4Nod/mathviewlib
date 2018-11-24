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
    // todo go threw area and set this node to them
    AreaRef cont = ctxt.MGD()->string(ctxt, content);
    SmartPtr<const HorizontalArrayArea> harea = smart_cast<const HorizontalArrayArea>(cont);
    std::cout << "this harea: " << harea << std::endl;
    harea->setNode(this);
    return harea; 
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
    content.push_back('7');
    // content.clear();
    getParentElement()->setDirtyLayout();
}
