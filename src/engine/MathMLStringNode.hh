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

#ifndef MathMLStringNode_hh
#define MathMLStringNode_hh

#include "Area.hh"
#include "MathMLTextNode.hh"
#include <vector>

class MathMLStringNode: public MathMLTextNode
{
protected:
  MathMLStringNode(const String&);
  virtual ~MathMLStringNode();

public:
  static SmartPtr<MathMLStringNode> create(const String& s)
  { return new MathMLStringNode(s); }

  virtual AreaRef  format(class FormattingContext&);

  virtual unsigned GetLogicalContentLength(void) const;
  virtual String   GetRawContent(void) const;
  virtual void     LookUpContent() const;
  virtual void     ClearContent();
  virtual void     DeleteContent();
  
  void DeleteGlyph(uint32_t index);
  void InsertGlyphAfter(int32_t index, std::basic_string<char> glyph);
  void InsertGlyphBefore(int32_t index, char glyph);
  void DeleteParentElement(void);
  void InsertElementAfter(void);
  // todo remove this logic to base element class (must be supported by all elements)
  void insertElementCursor(void);
  void insertInnerElementCursor(int32_t index);
  void setSelected(void);
  void clearV_Area(void) { v_area.clear(); };
  int32_t normalizeGlyphAreaIndex(AreaRef area, int32_t index);
  
private:
    // setDirtyLayout
  std::vector<AreaRef> v_area;
  String content;
};

#endif // MathMLStringNode_hh
