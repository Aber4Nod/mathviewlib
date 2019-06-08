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

#include "MathML.hh"
#include "AreaFactory.hh"
#include "BoundingBoxAux.hh"
#include "FormattingContext.hh"
#include "MathGraphicDevice.hh"
#include "MathMLGlyphNode.hh"
#include "MathMLIdentifierElement.hh"
#include "MathMLMarkNode.hh"
#include "MathMLNumberElement.hh"
#include "MathMLOperatorElement.hh"
#include "MathMLStringNode.hh"
#include "MathMLTextElement.hh"
#include "MathMLTokenElement.hh"
#include "MathMLValueConversion.hh"
#include "mathVariantAux.hh"
#include "traverseAux.hh"
#include "MathMLAttributeSignatures.hh"
#include <iostream>

MathMLTokenElement::MathMLTokenElement(const SmartPtr<class MathMLNamespaceContext>& context)
  : MathMLElement(context)
{ }

MathMLTokenElement::~MathMLTokenElement()
{ }

void
MathMLTokenElement::append(const String& s)
{
  std::cout << "[MathMLTokenElement::append]: creating mathmlstringnode w/ text: " << s << std::endl;
  content.appendChild(this, MathMLStringNode::create(s));
}

AreaRef
MathMLTokenElement::formatAux(FormattingContext& ctxt)
{
  RGBColor oldColor = ctxt.getColor();
  RGBColor oldBackground = ctxt.getBackground();

  if (SmartPtr<Value> value = GET_ATTRIBUTE_VALUE(MathML, Token, mathsize))
    {
      if (IsTokenId(value))
	switch (ToTokenId(value))
	  {
	  case T_SMALL: ctxt.addScriptLevel(1); break;
	  case T_BIG: ctxt.addScriptLevel(-1); break;
	  case T_NORMAL: break; // noop
	  default: assert(false); break;
	  }
      else
	ctxt.setSize(ctxt.MGD()->evaluate(ctxt, ToLength(value), ctxt.getSize()));
    } 

  const unsigned logicalContentLength = GetLogicalContentLength();
  if (SmartPtr<Value> value = GET_ATTRIBUTE_VALUE(MathML, Token, mathvariant))
    ctxt.setVariant(toMathVariant(value));
  else if (is_a<MathMLIdentifierElement>(SmartPtr<MathMLTokenElement>(this)) && logicalContentLength == 1)
    ctxt.setVariant(ITALIC_VARIANT); // turned off temporary for making qt appearance ok (while not fixed)
    // ctxt.setMathMode(false);

  if (is_a<MathMLTextElement>(SmartPtr<MathMLTokenElement>(this)) ||
      (is_a<MathMLIdentifierElement>(SmartPtr<MathMLTokenElement>(this)) && logicalContentLength > 1))
    ctxt.setMathMode(false);

  if (SmartPtr<Value> value = GET_ATTRIBUTE_VALUE(MathML, Token, mathcolor))
    ctxt.setColor(ToRGB(value));

  if (SmartPtr<Value> value = GET_ATTRIBUTE_VALUE(MathML, Token, mathbackground))
    ctxt.setBackground(ToRGB(value));

  RGBColor newColor = ctxt.getColor();
  RGBColor newBackground = ctxt.getBackground();

  std::vector<AreaRef> c;
  c.reserve(getSize());
#if 0
  std::transform(content.begin(), content.end(), std::back_inserter(c),
		 std::bind2nd(FormatAdapter<FormattingContext,MathMLTextNode,AreaRef>(), &ctxt));
#else
  uint32_t curIndex = 0;
  std::cout << "current node index: " << cursorNodeIndex << std::endl;
  for (const auto & elem : content) {
      if (curIndex++ == cursorNodeIndex) {
          elem->setCursorIndex(cursorNodeContentIndex);
          if (cursorNodeContentIndex == -1) {
              c.push_back(ctxt.MGD()->cursor(ctxt)); 
          }
          else {
              c.push_back(elem->format(ctxt));
              c.push_back(ctxt.MGD()->cursor(ctxt)); 
          }
      }

      if (elem->GetLogicalContentLength() == 0 || elem->GetLogicalContentLength() > elem->getCursorIndex() + 1) {
          std::cout << "GetLogicalContentLength: " << elem->GetLogicalContentLength() << " [MathMLTokenElement]: getCursorIndex: " << elem->getCursorIndex() << std::endl;
          c.push_back(elem->format(ctxt));
      }
      elem->resetFormattingIndex();
  }
#endif

  AreaRef res;
  std::cout << "current size: [MathMLTokenElement]: " << c.size() << " for MathMLTokenElement: " << this << std::endl;
  if (c.size() == 0) res = ctxt.MGD()->dummy(ctxt);
  else if (c.size() == 1) res = c[0];
  else res = ctxt.MGD()->getFactory()->horizontalArray(c);

  if (oldColor != newColor)
    res = ctxt.MGD()->getFactory()->color(res, newColor);

  if (!newBackground.transparent() && newBackground != oldBackground)
    res = ctxt.MGD()->getFactory()->background(res, newBackground);

  return res;
}

// todo make special formatting for cursor (temporary solution? -> come up w/ better solution)
AreaRef
MathMLTokenElement::format(FormattingContext& ctxt)
{
  if (dirtyLayout())
    {
      ctxt.push(this);
      printf("[MathMLTokenElement::format]: beginning to format dirty \n");
      // if (cursorSet()) {
      //   // must be binded to prev or next element (to be fitted) -> prevential formatting size (i.e. simple text); 
      //   // also there is an actual formatting size - after selecting which element to insert.
      //   std::vector<AreaRef> c;
      //   if (!getContentLength())
      //   {
      //       printf("[MathMLTokenElement::format]: no content length \n");
      //       // c.push_back(ctxt.MGD()->cursor(ctxt));
      //       c.push_back(ctxt.MGD()->wrapper(ctxt, formatAux(ctxt)));
      //   } 
      //   else
      //   {
      //       printf("[MathMLTokenElement::format]: content length = %d | data = %.*s \n", 
      //               getContentLength(), getContentLength(), GetRawContent().c_str());
      //       // ctxt.addScriptLevel(-1);
      //       // c.push_back(ctxt.MGD()->cursor(ctxt));
      //       c.push_back(ctxt.MGD()->wrapper(ctxt, formatAux(ctxt)));
      //   }
      //   // c.push_back(ctxt.MGD()->dummy(ctxt));
      //   setArea(ctxt.MGD()->getFactory()->horizontalArray(c));
      //   // setArea(ctxt.MGD()->wrapper(ctxt, ctxt.MGD()->cursor(ctxt)));
      // }
      // else
        setArea(ctxt.MGD()->wrapper(ctxt, formatAux(ctxt)));
      ctxt.pop();
      resetDirtyLayout();
    }

  return getArea();
}

bool
MathMLTokenElement::IsNonMarking() const
{
#if 0
  for (std::vector< SmartPtr<MathMLTextNode> >::const_iterator text = content.begin();
       text != content.end();
       text++)
    {
      assert(*text);
      if (!is_a<MathMLSpaceNode>(*text)) return false;
    }

  return true;
#endif
  return false;
}

#if 0
void
MathMLTokenElement::AddItalicCorrection()
{
  if (!is_a<MathMLIdentifierElement>(SmartPtr<MathMLElement>(this)) &&
      !is_a<MathMLNumberElement>(SmartPtr<MathMLElement>(this)) &&
      !is_a<MathMLTextElement>(SmartPtr<MathMLElement>(this))) return;
  
  if (getSize() == 0) return;

  SmartPtr<MathMLTextNode> lastNode = getChild(getSize() - 1);
  assert(lastNode);

  SmartPtr<MathMLElement> next = findRightSibling(this);
  if (!next) return;

  SmartPtr<MathMLOperatorElement> coreOp = next->getCoreOperatorTop();
  if (!coreOp) return;
  bool isFence = coreOp->IsFence();
  if (!isFence) return;
}
#endif

String
MathMLTokenElement::GetRawContent() const
{
  String res;
  for (const auto & elem : content)
    {
      assert(elem);
      res += elem->GetRawContent();
    }

  return res;
}

String
MathMLTokenElement::GetRawContentBeforeCursor()
{
    String res;
    uint32_t curIndex = 0;
    for (const auto & elem : content)
    {
        if (curIndex < cursorNodeIndex)
            res += elem->GetRawContent();
        else
        if (curIndex == cursorNodeIndex)
        {
            String elem_content = elem->GetRawContent();
            res += StringOfUCS4String(UCS4StringOfString(elem_content).substr(0, cursorNodeContentIndex + 1));
            return res;
        }
        else
            assert(curIndex <= cursorNodeIndex);
        ++curIndex;
    }

    return res;
}

String
MathMLTokenElement::GetRawContentAfterCursor()
{
    String res;
    uint32_t curIndex = 0;
    for (const auto & elem : content)
    {
        if (curIndex == cursorNodeIndex)
        {
            String elem_content = elem->GetRawContent();
            res += StringOfUCS4String(UCS4StringOfString(elem_content).substr(cursorNodeContentIndex + 1));
        }
        else
        if (curIndex > cursorNodeIndex)
            res += elem->GetRawContent();
        ++curIndex;
    }
    
    std::cout << "[MathMLTokenElement::GetRawContentAfterCursor]: value: " << res << std::endl;

    return res;
}

unsigned
MathMLTokenElement::GetLogicalContentLength() const
{
  unsigned len = 0;

  for (const auto & elem : content)
    {
      assert(elem);
      len += elem->GetLogicalContentLength();
    }

  return len;
}

unsigned
MathMLTokenElement::getContentLength() const
{ 
  return UCS4StringOfString(GetRawContent()).length();
}

uint32_t
MathMLTokenElement::getContentSize() const
{
    // todo must be accumulate!
    uint32_t size = 0;
    for (const auto & elem : content)
    {
        assert(elem);
        std::cout << "[MathMLTokenElement::getContentSize]: child: " << elem << std::endl;
        size += 1;
    }
    
    return size;
}

void
MathMLTokenElement::setCursorPosition(const SmartPtr<class MathMLTextNode>& node, int32_t index)
{
    uint32_t curIndex = 0;
    for (const auto & elem : content) {
        if (node == elem) {
            // if (cursorNodeIndex == curIndex)
                
            cursorNodeIndex        = curIndex;
            cursorNodeContentIndex = index;
            std::cout << "[MathMLTokenElement::setCursorPosition]: found element " << elem << " with total node index: " << cursorNodeIndex << std::endl;
        }
        ++curIndex;
    }
}

void
MathMLTokenElement::insertGlyphAfterCursor(std::basic_string<char> glyph)
{
    uint32_t curIndex = 0;
    for (const auto & elem : content) {
        if (curIndex == cursorNodeIndex) {
            smart_cast<MathMLStringNode>(elem)->InsertGlyphAfter(cursorNodeContentIndex, glyph);
            cursorNodeContentIndex++;
            return;
        }
        ++curIndex;
    }
}

void
MathMLTokenElement::deleteGLyphBeforeCursor()
{
    uint32_t curIndex = 0;
    for (const auto & elem : content) {
        if (curIndex == cursorNodeIndex) {
            smart_cast<MathMLStringNode>(elem)->DeleteGlyph(cursorNodeContentIndex);
            cursorNodeContentIndex--;
            return;
        }
        ++curIndex;
    }
}

void
MathMLTokenElement::setLastCursorPostition()
{
    cursorNodeIndex = content.getContent().size() - 1;
    cursorNodeContentIndex = content.getContent().back()->GetLogicalContentLength() - 1;

    setCursorSet();
    setDirtyStructure();
    setDirtyLayout();
}

void
MathMLTokenElement::setFirstCursorPostition()
{
    cursorNodeIndex = 0;
    cursorNodeContentIndex = -1;

    setCursorSet();
    setDirtyStructure();
    setDirtyLayout();
}

int32_t
MathMLTokenElement::decreaseCursorPosition()
{
    if (!cursorSet() || (cursorNodeIndex == 0 && cursorNodeContentIndex == -1))
        return -1;

    if (cursorNodeContentIndex == 0 && cursorNodeIndex != 0)
    {
        cursorNodeIndex--;
        cursorNodeContentIndex = content.getContent()[cursorNodeIndex]->GetLogicalContentLength();
    }
    else
        cursorNodeContentIndex--;

    setDirtyStructure();
    setDirtyLayout();
    return 0;
}

int32_t
MathMLTokenElement::increaseCursorPosition()
{
    if (!cursorSet() || (cursorNodeIndex == content.getContent().size() - 1
               && cursorNodeContentIndex == content.getContent().back()->GetLogicalContentLength() - 1))
        return -1;

    if (cursorNodeContentIndex == content.getContent()[cursorNodeIndex]->GetLogicalContentLength() - 1
            && cursorNodeIndex != content.getContent().size() - 1)
    {
        cursorNodeIndex++;
        cursorNodeContentIndex = 0;
    }
    else
        cursorNodeContentIndex++;

    setDirtyStructure();
    setDirtyLayout();
    return 0;
}

void 
MathMLTokenElement::resetCursor()
{
    if (!cursorSet())
        return;

    resetFlag(FCursorSet);
    setDirtyStructure();
    setDirtyLayout();
    setNodeIndex(-1);
    setNodeContentIndex(-1);
}

void
MathMLTokenElement::setNodeIndex(int32_t index)
{
    if (index == -1)
        std::for_each(content.begin(), content.end(), [](SmartPtr<MathMLTextNode> elem) {
            elem->setCursorIndex(-1);
            // smart_cast<MathMLStringNode>(elem)->clearV_Area();
        });
    cursorNodeIndex = index;
}

void
MathMLTokenElement::setNodeContentIndex(int32_t index)
{
    if (index == -1)
        std::for_each(content.begin(), content.end(), [](SmartPtr<MathMLTextNode> elem) {
            elem->setCursorIndex(-1);
            // smart_cast<MathMLStringNode>(elem)->clearV_Area();
        });
    cursorNodeContentIndex = index;
}
