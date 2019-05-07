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

#include "defs.h"

#include "Clock.hh"
#include "View.hh"
#include "Element.hh"
#include "Builder.hh"
#include "MathMLNamespaceContext.hh"
#include "MathMLOperatorDictionary.hh"
#include "AreaId.hh"
#include "AbstractLogger.hh"
#include "FormattingContext.hh"
#include "MathGraphicDevice.hh"
#include "MathMLTokenElement.hh"

#include <iostream>

View::View(const SmartPtr<AbstractLogger>& l)
  : logger(l), defaultFontSize(DEFAULT_FONT_SIZE), freezeCounter(0)
{ }

View::~View()
{
  // When the view is destroyed the formatting tree must have
  // been destroyed already, because elements need a healthy
  // view for de-registering themselves from the linker
  assert(!rootElement);
}

bool
View::freeze()
{
  return freezeCounter++ == 0;
}

bool
View::thaw()
{
  assert(freezeCounter > 0);
  return --freezeCounter == 0;
}

SmartPtr<AbstractLogger>
View::getLogger() const
{ return logger; }

void
View::setOperatorDictionary(const SmartPtr<MathMLOperatorDictionary>& d)
{ dictionary = d; }

SmartPtr<MathMLOperatorDictionary>
View::getOperatorDictionary() const
{ return dictionary; }

void
View::setBuilder(const SmartPtr<Builder>& b)
{
  resetRootElement();
  builder = b;
  if (builder)
    {
      builder->setMathMLNamespaceContext(mathmlContext);
      builder->setLogger(logger);
    }
}

SmartPtr<Builder>
View::getBuilder() const
{ return builder; }

AreaRef
View::formatElement(const SmartPtr<Element>& elem) const
{
  if (!elem) return nullptr;

  if (elem->dirtyLayout())
    {
        std::cout << "its dirty!" << std::endl;
      const SmartPtr<MathGraphicDevice> mgd = mathmlContext ? mathmlContext->getGraphicDevice() : nullptr;
      assert(mgd != nullptr);
      FormattingContext ctxt(mgd);
      Length defaultSize(getDefaultFontSize(), Length::PT_UNIT);
      scaled l = mgd->evaluate(ctxt, defaultSize, scaled::zero());
      ctxt.setSize(l);
      ctxt.setActualSize(ctxt.getSize());
      ctxt.setAvailableWidth(getAvailableWidth());
      Clock perf;
      perf.Start();
      elem->format(ctxt);
      perf.Stop();
      getLogger()->out(LOG_WARNING, "formatting time: %dms", perf());
    }

  return elem->getArea();
}

SmartPtr<Element>
View::getRootElement() const
{
  bool rootDirty = !rootElement ||
    rootElement->dirtyStructure() || rootElement->dirtyAttribute() ||
    rootElement->dirtyAttributeP();

  if (rootDirty)
    {
      Clock perf;
	
      perf.Start();
      rootElement = builder->getRootElement();
      perf.Stop();

      getLogger()->out(LOG_WARNING, "build time: %dms", perf());
    }
  
  return rootElement;
}

void 
View::deleteSelectedElements() const
{
    builder->deleteSelectedElements();
}

void
View::resetRootElement()
{
  rootElement = nullptr;
}

AreaRef
View::getRootArea() const
{ return formatElement(getRootElement()); }

BoundingBox
View::getBoundingBox() const
{
  if (AreaRef rootArea = getRootArea())
      return rootArea->box();
  else
    return BoundingBox();
}

AreaRef
View::getAreaAt(const scaled& x, const scaled& y) const
{
  if (AreaRef rootArea = getRootArea())
    {
      std::cout << "[getAreaAt]: current root area: " << rootArea << std::endl;
      return rootArea->searchByCoordsSimple(x, y); // log it
      // AreaId deepId(rootArea);
      // return rootArea->searchByCoords(deepId, x, y); // log it
    }
  return nullptr;
}

SmartPtr<Element>
View::getElementAt(const scaled& x, const scaled& y, Point* elemOrigin, BoundingBox* elemBox) const
{
  if (AreaRef rootArea = getRootArea())
    {
      AreaId deepId(rootArea);
      if (rootArea->searchByCoords(deepId, x, y))
	for (int i = deepId.size(); i >= 0; i--)
	  {
	    AreaRef area = deepId.getArea(i);
	    if (SmartPtr<Element> elem = area->getElement())
	      {
		if (elemOrigin) deepId.getOrigin(*elemOrigin, 0, i);
		if (elemBox) *elemBox = area->box();
		return elem;
	      }
	  }
    }

  return nullptr;
}

bool
View::getElementExtents(const SmartPtr<Element>& refElem, const SmartPtr<Element>& elem,
			Point* elemOrigin, BoundingBox* elemBox) const
{
  assert(refElem);
  assert(elem);
  if (getRootArea())
    if (AreaRef elemArea = elem->getArea())
      {
	if (elemOrigin)
	  {
	    if (AreaRef refArea = refElem->getArea())
	      {
		AreaId elemId(refArea);
		if (refArea->searchByArea(elemId, elemArea))
		  elemId.getOrigin(*elemOrigin);
		else
		  return false;
	      }
	    else
	      return false;
	  }
	
	if (elemBox) *elemBox = elemArea->box();

	return true;
      }

  return false;
}

bool
View::getElementExtents(const SmartPtr<Element>& elem, Point* elemOrigin, BoundingBox* elemBox) const
{
  return getElementExtents(getRootElement(), elem, elemOrigin, elemBox);
}

bool
View::getElementLength(const SmartPtr<Element>& elem, CharIndex& length) const
{
  assert(elem);
  if (getRootArea())
    if (AreaRef elemArea = elem->getArea())
      {
	length = elemArea->length();
	return true;
      }

  return false;
}

SmartPtr<Element>
View::getCharAt(const scaled& x, const scaled& y, CharIndex& index, Point* charOrig, BoundingBox* charBox) const
{
  if (AreaRef rootArea = getRootArea())
    {
      AreaId deepId(rootArea);
      if (rootArea->searchByCoords(deepId, x, y))
	for (int i = deepId.size(); i >= 0; i--)
	  {
	    AreaRef area = deepId.getArea(i);
	    if (SmartPtr<Element> elem = area->getElement())
	      {
		Point deepOrigin;
		deepId.accumulateOrigin(deepOrigin);
		
		AreaRef deepArea = deepId.getArea();
		CharIndex deepIndex;
		if (!deepArea->indexOfPosition(x - deepOrigin.x, y - deepOrigin.y, deepIndex))
		  deepIndex = 0;
		
		index = deepId.getLength(i, -1) + deepIndex;
		
		if (charOrig || charBox)
		  {
		    if (!deepArea->positionOfIndex(deepIndex, charOrig, charBox))
		      return nullptr;
		  }
		
		return elem;
	      }
	  }
    }

  return nullptr;
}

bool
View::getCharExtents(const SmartPtr<Element>& refElem, const SmartPtr<Element>& elem, CharIndex index,
		     Point* charOrig, BoundingBox* charBox) const
{
  assert(refElem);
  assert(elem);

  Point elemOrig;
  if (getElementOrigin(refElem, elem, elemOrig))
    if (AreaRef elemArea = elem->getArea())
      {
	AreaId deepId(elemArea);
	if (elemArea->searchByIndex(deepId, index))
	  {
	    AreaRef deepArea = deepId.getArea();
	    Point deepOrig;
	    deepId.getOrigin(deepOrig);

	    if (deepArea->positionOfIndex(index - deepId.getLength(), charOrig, charBox))
	      {
		if (charOrig)
		  {
		    charOrig->x += elemOrig.x + deepOrig.x;
		    charOrig->y += elemOrig.y + deepOrig.y;
		  }

		return true;
	      }
	  }
      }
  
  return false;
}

bool
View::getCharExtents(const SmartPtr<Element>& elem, CharIndex index,
		     Point* charOrig, BoundingBox* charBox) const
{
  return getCharExtents(getRootElement(), elem, index, charOrig, charBox);
}

void
View::setMathMLNamespaceContext(const SmartPtr<MathMLNamespaceContext>& ctxt)
{
  mathmlContext = ctxt;
  if (builder) builder->setMathMLNamespaceContext(mathmlContext);
}

SmartPtr<MathMLNamespaceContext>
View::getMathMLNamespaceContext(void) const
{ return mathmlContext; }

void
View::render(RenderingContext& ctxt, const scaled& x, const scaled& y) const
{
  // std::cerr << "View::render " << &ctxt << std::endl;
  if (AreaRef rootArea = getRootArea())
    {
      Clock perf;
      perf.Start();

      // Basically (x, y) are the coordinates of the origin
      rootArea->render(ctxt, x, y);

      perf.Stop();
      getLogger()->out(LOG_WARNING, "rendering time: %dms", perf());
    }
}

void
View::setDirtyLayout() const
{
  if (SmartPtr<Element> elem = getRootElement())
    {
      //elem->setDirtyAttributeD();
      elem->setDirtyLayoutD();	  
    }
}

void
View::setDefaultFontSize(unsigned size)
{
  assert(size > 0);
  if (defaultFontSize != size)
    {
      defaultFontSize = size;
      setDirtyLayout();
    }
}

void
View::setAvailableWidth(const scaled& width)
{
  if (width != availableWidth)
    {
      availableWidth = width;
      setDirtyLayout();
    }
}

int32_t
View::deleteGlyph(const scaled& x, const scaled& y) const
{
    AreaRef area = getAreaAt(x, y);
    if (area)
    {
        uint32_t index = area->getGlyphArea()->getParent()->getIndexOfChild(area->getGlyphArea());
        area->getGlyphArea()->getParent()->getParent()->getNode()->DeleteGlyph(index);
        return 1;
    }

    std::cout << "GlyphArea at x: " << x.toDouble() << " y: " << y.toDouble() << " not found" << std::endl;
    return 0;
}

int32_t
View::insertGlyphAfter(const scaled& x, const scaled& y, char c) const
{
    AreaRef area = getAreaAt(x, y);
    if (area)
    {
        uint32_t index = area->getGlyphArea()->getParent()->getIndexOfChild(area->getGlyphArea());
        area->getGlyphArea()->getParent()->getParent()->getNode()->InsertGlyphAfter(index, c);
        return 1;
    }

    std::cout << "GlyphArea at x: " << x.toDouble() << " y: " << y.toDouble() << " not found" << std::endl;
    return 0;
}

int32_t
View::insertGlyphBefore(const scaled& x, const scaled& y, char c) const
{
    AreaRef area = getAreaAt(x, y);
    if (area)
    {
        uint32_t index = area->getGlyphArea()->getParent()->getIndexOfChild(area->getGlyphArea());
        area->getGlyphArea()->getParent()->getParent()->getNode()->InsertGlyphBefore(index, c);
        return 1;
    }

    std::cout << "GlyphArea at x: " << x.toDouble() << " y: " << y.toDouble() << " not found" << std::endl;
    return 0;
}

void
View::lookUpContent(const scaled& x, const scaled& y) const
{
    AreaRef area = getAreaAt(x, y);
    if (area)
    {
        area->getGlyphArea()->getParent()->getParent()->getNode()->LookUpContent();
        return;
    }

    std::cout << "GlyphArea at x: " << x.toDouble() << " y: " << y.toDouble() << " not found" << std::endl;
}

int32_t
View::deleteElement(const scaled& x, const scaled& y) const
{
    AreaRef area = getAreaAt(x, y);
    if (area)
    {
        area->getGlyphArea()->getParent()->getParent()->getNode()->DeleteParentElement();
        return 1;
    }

    std::cout << "GlyphArea at x: " << x.toDouble() << " y: " << y.toDouble() << " not found" << std::endl;
    return 0;
}

int32_t
View::insertElementAfter(const scaled& x, const scaled& y) const
{
    std::cout << "in insertElementAfter" << std::endl;
    AreaRef area = getAreaAt(x, y);
    if (area)
    {
        area->getGlyphArea()->getParent()->getParent()->getNode()->InsertElementAfter();
        return 1;
    }

    std::cout << "GlyphArea at x: " << x.toDouble() << " y: " << y.toDouble() << " not found" << std::endl;
    return 0;
}

int32_t
View::insertElementCursor(const scaled& x, const scaled& y) const
{
    std::cout << "in insertElementCursor" << std::endl;
    AreaRef area = getAreaAt(x, y);
    if (area)
    {
        MathMLTokenElement *_elem = static_cast<MathMLTokenElement *>(getElementByFlag(Element::FCursorSet));
        if (_elem != nullptr) {
            std::cout << "[View::insertElementCursor]: reseting old cursor positions " << std::endl;
            _elem->resetFlag(Element::FCursorSet);
            _elem->setNodeIndex(-1);
            _elem->setNodeContentIndex(-1);
            _elem->setDirtyLayout();
            _elem->setDirtyStructure();
        }

        const WrapperArea *wrapperArea = smart_cast<const WrapperArea>(area);
        if (wrapperArea)
        {
            SmartPtr<Element> wrappedElem = wrapperArea->getElement();
            std::cout << "testing wrapperElem: " << wrappedElem << std::endl;
            wrappedElem->setInsertSetCursor();
            wrappedElem->setDirtyLayout();
            wrappedElem->setDirtyStructure();
            return 1;
        }

        uint32_t index = area->getGlyphArea()->getParent()->getIndexOfChild(area->getGlyphArea());
        AreaRef harea = area->getGlyphArea()->getParent()->getParent();
        std::cout << "[View::insertElementCursor]: beginning to counting normalized index for node: " << area->getGlyphArea()->getParent()->getParent()->getNode() << std::endl;
        int32_t normalized_index = area->getGlyphArea()->getParent()->getParent()->getNode()->normalizeGlyphAreaIndex(
            harea, index);
        // uint32_t normalized_index = index;
        std::cout << "[View::insertElementCursor]: normalized_index: " << normalized_index << std::endl;
        area->getGlyphArea()->getParent()->getParent()->getNode()->insertInnerElementCursor(normalized_index);
        // area->getGlyphArea()->getParent()->getParent()->getNode()->insertElementCursor();

        return 1;
    }
    std::cout << "Parent glypharea at x: " << x.toDouble() << " y: " << y.toDouble() << " not found" << std::endl;
    std::cout << "GlyphArea at x: " << x.toDouble() << " y: " << y.toDouble() << " not found" << std::endl;
    return 0;
}

Element*
View::getElementByFlag(Element::Flags f) const
{
    return getRootElement()->getElementByFlag(f);
}

int32_t
View::insertElementAfterCursor(char c) const
{
    MathMLTokenElement *_elem = static_cast<MathMLTokenElement *>(getElementByFlag(Element::FCursorSet));
    // printf("[View::insertElementAfterCursor]: size of content: %d", _elem->getSize());
    
    // MathMLTextNode *last_node = static_cast<MathMLTextNode *>(_elem->getChild(_elem->getSize()));
    // static_cast<MathMLStringNode *>(last_node)->InsertGlyphAfter(-1, 't');
    _elem->append(std::string(1, c));
    printf("[View::insertElementAfterCursor]: content after inserting: %s", _elem->GetRawContent().c_str());
    _elem->setDirtyLayout();
    _elem->setDirtyStructure();
    _elem->setContentSet();
    std::cout << "[View::insertElementAfterCursor]: Cursor element address: " << _elem << std::endl;
    return 1;
}

int32_t 
View::insertElementAfterCursor(std::string name) const
{
    MathMLTokenElement *_elem = static_cast<MathMLTokenElement *>(getElementByFlag(Element::FCursorSet));
    _elem->setInsertElementName(name);
    _elem->setDirtyLayout();
    _elem->setDirtyStructure();
    return 1;
}

int32_t
View::insertGlyphAfterCursor(char c)
{
    MathMLTokenElement *_elem = static_cast<MathMLTokenElement *>(getElementByFlag(Element::FCursorSet));
    _elem->insertGlyphAfterCursor(c);
    _elem->setDirtyLayout();
    _elem->setDirtyStructure();
}

void
View::deleteGLyphBeforeCursor()
{
    MathMLTokenElement *_elem = static_cast<MathMLTokenElement *>(getElementByFlag(Element::FCursorSet));
    if (!_elem)
        return;

    if (_elem->getCursorNodeIndex() == 0 && _elem->getcursorNodeContentIndex() == -1)
        _elem->setDeleteSet();
    else
        _elem->deleteGLyphBeforeCursor();

    _elem->setDirtyLayout();
    _elem->setDirtyStructure();
}

void
View::moveCursorLeft()
{
    MathMLTokenElement *_elem = static_cast<MathMLTokenElement *>(getElementByFlag(Element::FCursorSet));
    if (_elem->decreaseCursorPosition() == -1)
    {
        builder->presetModelParentRow(_elem);
        _elem->setMovePrev();
    }
}

void
View::moveCursorRight()
{
    MathMLTokenElement *_elem = static_cast<MathMLTokenElement *>(getElementByFlag(Element::FCursorSet));
    if (_elem->increaseCursorPosition() == -1) {
        std::cout << "[View::moveCursorRight]" << std::endl;
        builder->presetModelParentRow(_elem);
        _elem->setMoveNext();
    }
}

void
View::stepCursorRight()
{
    MathMLTokenElement *_elem = static_cast<MathMLTokenElement *>(getElementByFlag(Element::FCursorSet));
    if (_elem == nullptr)
        return;

    if (!_elem->rawTextElementSet() || _elem->getContentLength())
        _elem->resetCursor();
    _elem->setInsertSetCursor();
}

void
View::stepCursorLeft()
{
    MathMLTokenElement *_elem = static_cast<MathMLTokenElement *>(getElementByFlag(Element::FCursorSet));
    if (_elem == nullptr)
        return;

    if (!_elem->rawTextElementSet() || _elem->getContentLength())
        _elem->resetCursor();
    _elem->setInsertSetCursorLeft();
}

void
View::stepCursorUp()
{
    MathMLTokenElement *_elem = static_cast<MathMLTokenElement *>(getElementByFlag(Element::FCursorSet));
    if (_elem == nullptr)
        return;

    if (!_elem->rawTextElementSet() || _elem->getContentLength())
        _elem->resetCursor();

    _elem->setMoveUp();
}

void
View::stepCursorDown()
{
    MathMLTokenElement *_elem = static_cast<MathMLTokenElement *>(getElementByFlag(Element::FCursorSet));
    if (_elem == nullptr)
        return;

    if (!_elem->rawTextElementSet() || _elem->getContentLength())
        _elem->resetCursor();

    _elem->setMoveDown();
}

SmartPtr<Element>
View::getElementAtPos(const scaled& x, const scaled& y) const
{
    AreaRef area = getAreaAt(x, y);
    if (!area)
        return nullptr;

    SmartPtr<Element> _element;
    const WrapperArea *wrapperArea = smart_cast<const WrapperArea>(area);
    if (wrapperArea)
        _element = wrapperArea->getElement();
    else
        _element = area->getGlyphArea()->getParent()->getParent()->getNode()->getParentElement();
    return _element;
}

bool
View::copyElement() const
{
    return builder->copyElement();
}

bool
View::isSelectedElement(SmartPtr<Element> _element) const
{
    return builder->isSelectedElement(_element);
}

void
View::unselectElement(SmartPtr<Element> _element) const
{
    builder->unselectElement(_element);
}

bool
View::selectElement(const scaled& x, const scaled& y) const
{
    SmartPtr<Element> _element = getElementAtPos(x, y);
    if (!_element)
        return false;

    if (isSelectedElement(_element)) {
        unselectElement(_element);
        return true;
    }

    _element->setSelected();
    _element->setDirtyLayout();
    _element->setDirtyStructure();
    builder->selectElement();
    return true;
}

bool
View::insertCopiedElement() const
{
    if (!builder->isCopiedElement())
        return false;

    MathMLTokenElement *_elem = static_cast<MathMLTokenElement *>(getElementByFlag(Element::FCursorSet));
    if (_elem == nullptr)
        return false;

    builder->presetModelParentRow(_elem);
    std::cout << "setting insert copied! " << _elem << std::endl;
    _elem->setInsertCopied();
    std::cout << "insert copied setted! " << _elem << std::endl;
    return true;
}
