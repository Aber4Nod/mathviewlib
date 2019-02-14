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

#ifndef __TemplateElementIterator_hh__
#define __TemplateElementIterator_hh__

#include "TemplateElementValidator.hh"

template <class Model>
class TemplateElementIterator : public TemplateElementValidator<Model>
{
public:
  TemplateElementIterator(const typename Model::Element& root, const String& ns = "*", const String& n = "*")
    : TemplateElementValidator<Model>(ns, n),
      currentElement(findValidNodeForward(Model::getFirstChild(Model::asNode(root))))
  { assert(root); }

  typename Model::Element element(void) const { return currentElement; }
  bool more(void) const { return currentElement; }
  void next(void)
  { 
    // assert(currentElement);
    if (currentElement != nullptr)
        currentElement = findValidNodeForward(Model::getNextSibling(Model::asNode(currentElement)));
  }
  void setCurrent(typename Model::Element newElement)
  { currentElement = newElement; }

  typename Model::Node updateCurrent(const typename Model::Element& el)
  {
      typename Model::Element xml_element = element();
      typename Model::Node node = Model::createNode(Model::getNodeNamespace(Model::asNode(xml_element)), "mi");
      Model::setNextSibling(node, Model::getNextSibling(Model::asNode(xml_element)));
      Model::setParent(node, Model::asNode(el));
      Model::setNextSibling(Model::getPrevSibling(Model::asNode(xml_element)), node);
      // Model::setPrevSibling(node, Model::getPrevSibling(Model::asNode(xml_element))); // todo useless because of insertPrevSibling
      Model::insertPrevSibling(Model::asNode(xml_element), node);
      Model::setNodeValue(node, "");

      setCurrent(Model::asElement(node));
      // iter.next();
      // Model::asNode(xml_element)->prev = node; // total useless

      Model::unlinkNode(Model::asNode(xml_element));
      Model::freeNode(Model::asNode(xml_element));
      return node;
  }

  typename Model::Node
  insertAfter(const typename Model::Element& el)
  {
      typename Model::Element xml_element = element();
      typename Model::Node nextS = Model::getNextSibling(
          Model::asNode(xml_element));
      typename Model::Node prevS = Model::getPrevSibling(
          Model::asNode(xml_element));
          
      printf("current value %s\n", Model::getNodeValue(Model::asNode(xml_element)).c_str());
      printf("next value %s\n", Model::getNodeValue(nextS).c_str());
      printf("prev value %s\n", Model::getNodeValue(prevS).c_str());

      // creating mrow parent element
      typename Model::Node nodeParent = Model::createNode(
          Model::getNodeNamespace(Model::asNode(xml_element)), "mrow");
      Model::unlinkNode(Model::asNode(xml_element));

      Model::setParent(nodeParent, Model::asNode(el));
      Model::insertChild(Model::asNode(el), nodeParent);
      Model::insertChild(nodeParent, Model::asNode(xml_element));
      // std::cout << "xml_element value: " << Model::getNodeValue(Model::asNode(xml_element));

      // creating default next element
      typename Model::Node node = Model::createNode(
          Model::getNodeNamespace(Model::asNode(xml_element)), "munderover");
      Model::insertChild(nodeParent, node);

      Model::setParent(Model::asNode(xml_element), nodeParent);
      Model::setParent(node, nodeParent);
      
      Model::setNextSibling(nodeParent, nextS);
      // Model::setNextSibling(prevS, nodeParent);
      // Model::insertNextSibling(nodeParent, nextS);
      Model::setPrevSibling(nextS, nodeParent);
      // Model::setPrevSibling(nodeParent, prevS);
      // Model::setNextSibling(nodeParent, Model::getNextSibling(
      //     Model::asNode(xml_element)));
          
      Model::setNextSibling(Model::asNode(xml_element), node);
      Model::setPrevSibling(node, Model::asNode(xml_element));

      // Model::insertNextSibling(Model::asNode(xml_element), node);
      Model::setNodeValue(node, "");

      // Model::insertNextSibling(nodeParent, Model::getNextSibling(
          // Model::asNode(xml_element)));
      setCurrent(Model::asElement(nodeParent));
      return nodeParent;
  }
  


protected:
  typename Model::Element
  findValidNodeForward(const typename Model::Node& p0) const
  {
    for (typename Model::Node p = p0; p; p = Model::getNextSibling(p))
      if (this->valid(p)) return Model::asElement(p);
    return typename Model::Element();
  }
    
  typename Model::Element currentElement;
};

#endif // __TemplateElementIterator_hh__
