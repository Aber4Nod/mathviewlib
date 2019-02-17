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

      if (Model::getNodeName(Model::asNode(el)) != "mrow") {
          // creating mrow parent element
          typename Model::Node nodeParent = Model::createNewChild(Model::asNode(el), 
                Model::getNodeNamespace(Model::asNode(el)),
                Model::toModelString("mrow"), Model::toModelString(""));
          printf("[insertAfter]: nodeParent node name: %s\n", Model::getNodeName(nodeParent).c_str());
          Model::replaceNode(Model::asNode(xml_element), nodeParent);
          Model::insertChild(nodeParent, Model::asNode(xml_element));

          // creating default next element
          typename Model::Node node = Model::createNewChild(nodeParent, 
                Model::getNodeNamespace(Model::asNode(xml_element)),
                Model::toModelString("munderover"), Model::toModelString(""));

          // typename Model::Node node = Model::createNode(
              // Model::getNodeNamespace(xml_element_copy), "munderover");
          Model::insertNextSibling(Model::asNode(xml_element), node);
          // Model::insertChild(nodeParent, node);

          // Model::setParent(xml_element_copy, nodeParent);
          // Model::setParent(node, nodeParent);
          
          Model::setNextSibling(nodeParent, nextS);
          Model::setPrevSibling(nextS, nodeParent);

          // Model::setNextSibling(xml_element_copy, node);
          // Model::setPrevSibling(node, xml_element_copy);
          setCurrent(Model::asElement(nodeParent));
          Model::setNodeValue(node, "");
          return nodeParent;
      }

      typename Model::Node node = Model::createNewChild(Model::asNode(el), 
            Model::getNodeNamespace(Model::asNode(xml_element)),
            Model::toModelString("munderover"), Model::toModelString(""));
      Model::insertNextSibling(Model::asNode(xml_element), node);
      // setCurrent(Model::asElement(node));
      return Model::asNode(xml_element);
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
