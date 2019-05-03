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

  typename Model::Node updateCurrent(const typename Model::Element& el, String value)
  {
      typename Model::Element xml_element = element();
      typename Model::Node node = Model::createNode(Model::getNodeNamespace(Model::asNode(xml_element)), Model::getNodeName(Model::asNode(xml_element)));
      Model::setNextSibling(node, Model::getNextSibling(Model::asNode(xml_element)));
      Model::setParent(node, Model::asNode(el));
      Model::setNextSibling(Model::getPrevSibling(Model::asNode(xml_element)), node);
      // Model::setPrevSibling(node, Model::getPrevSibling(Model::asNode(xml_element))); // todo useless because of insertPrevSibling
      Model::insertPrevSibling(Model::asNode(xml_element), node);
      Model::setNodeValue(node, value);

      setCurrent(Model::asElement(node));
      // iter.next();
      // Model::asNode(xml_element)->prev = node; // total useless

      Model::unlinkNode(Model::asNode(xml_element));
      Model::freeNode(Model::asNode(xml_element));
      return node;
  }

typename Model::Element
swapNext(const typename Model::Element& el)
{
  typename Model::Element nextValidSibling = findValidNodeForward(Model::getNextSibling(Model::asNode(el)));
  Model::replaceNode(Model::asNode(el), Model::asNode(nextValidSibling));
  Model::insertNextSibling(Model::asNode(nextValidSibling), Model::asNode(el));
  setCurrent(nextValidSibling);
  return el;
}

typename Model::Element
swapPrev(const typename Model::Element& el)
{
  typename Model::Element prevValidSibling = findValidNodePrev(Model::asNode(el));
  Model::replaceNode(Model::asNode(el), Model::asNode(prevValidSibling));
  Model::insertPrevSibling(Model::asNode(prevValidSibling), Model::asNode(el));
  setCurrent(el);
  return el;
}

typename Model::Element
insertParent(const typename Model::Element& el, std::string name = "mstyle")
{
    typename Model::Element xml_element = element();

    typename Model::Node nodeParent = Model::createNewChild(Model::asNode(el), 
          Model::getNodeNamespace(Model::asNode(el)),
          Model::toModelString(name), Model::toModelString(""));

    Model::replaceNode(Model::asNode(xml_element), nodeParent);
    Model::insertChild(nodeParent, Model::asNode(xml_element));

    setCurrent(Model::asElement(nodeParent));
    return xml_element;
}

typename Model::Node
insertAfterPrepareMROW(const typename Model::Element& el)
{
    typename Model::Element xml_element = element();
    typename Model::Node nextS = Model::getNextSibling(
        Model::asNode(xml_element));
    typename Model::Node prevS = Model::getPrevSibling(
        Model::asNode(xml_element));

    if (Model::getNodeName(Model::asNode(el)) == "mrow") 
        return Model::asNode(xml_element);

    typename Model::Node nodeParent = Model::createNewChild(Model::asNode(el), 
          Model::getNodeNamespace(Model::asNode(el)),
          Model::toModelString("mrow"), Model::toModelString(""));
    printf("[insertAfter]: nodeParent node name: %s\n", Model::getNodeName(nodeParent).c_str());
    Model::replaceNode(Model::asNode(xml_element), nodeParent);
    Model::insertChild(nodeParent, Model::asNode(xml_element));

    setCurrent(Model::asElement(nodeParent));
    return nodeParent;
}

  typename Model::Node
  insertAfter(const typename Model::Element& el, std::string name = "mo", String str = "")
  {
      typename Model::Element xml_element = element();
      typename Model::Node nextS = Model::getNextSibling(
          Model::asNode(xml_element));
      typename Model::Node prevS = Model::getPrevSibling(
          Model::asNode(xml_element));

      printf("iterator insertAfter now\n");
      // if (Model::getNodeName(Model::asNode(el)) != "mrow") {
      //     // creating mrow parent element
      //     typename Model::Node nodeParent = Model::createNewChild(Model::asNode(el), 
      //           Model::getNodeNamespace(Model::asNode(el)),
      //           Model::toModelString("mrow"), Model::toModelString(""));
      //     printf("[insertAfter]: nodeParent node name: %s\n", Model::getNodeName(nodeParent).c_str());
      //     Model::replaceNode(Model::asNode(xml_element), nodeParent);
      //     Model::insertChild(nodeParent, Model::asNode(xml_element));
      // 
      //     // creating default next element
      //     typename Model::Node node = Model::createNewChild(nodeParent, 
      //           Model::getNodeNamespace(Model::asNode(xml_element)),
      //           Model::toModelString("mi"), Model::toModelString("t"));
      // 
      //     // typename Model::Node node = Model::createNode(
      //         // Model::getNodeNamespace(xml_element_copy), "munderover");
      //     Model::insertNextSibling(Model::asNode(xml_element), node);
      //     // Model::insertChild(nodeParent, node);
      // 
      //     // Model::setParent(xml_element_copy, nodeParent);
      //     // Model::setParent(node, nodeParent);
      // 
      //     Model::setNextSibling(nodeParent, nextS);
      //     Model::setPrevSibling(nextS, nodeParent);
      // 
      //     // Model::setNextSibling(xml_element_copy, node);
      //     // Model::setPrevSibling(node, xml_element_copy);
      //     setCurrent(Model::asElement(nodeParent));
      //     // Model::setNodeValue(node, "");
      // 
      //     // inserting cursor
      //     // typename Model::Node node1 = Model::createNewChild(Model::asNode(el), 
      //           // Model::getNodeNamespace(Model::asNode(xml_element)),
      //           // Model::toModelString("mi"), Model::toModelString(""));
      //     // Model::insertNextSibling(node, node1);
      // 
      //     return nodeParent;
      // }
      // 
      typename Model::Node node = Model::createNode(
          Model::getNodeNamespace(Model::asNode(xml_element)), name);
      Model::insertNextSibling(Model::asNode(xml_element), node);
      // Model::setNodeValue(node, "");
      if (!name.compare("mi") || !name.compare("mo") || !name.compare("mn") || !name.compare("mtext")) {
          typename Model::Node node_text = Model::NewText(Model::toModelString(str));
          Model::insertChild(node, node_text);
      }
      // setCurrent(Model::asElement(node));
      return node;
  }

  typename Model::Node
  insertBefore(const typename Model::Element& el, std::string name = "mo")
  {
      typename Model::Element xml_element = element();
      typename Model::Node node = Model::createNode(
          Model::getNodeNamespace(Model::asNode(xml_element)), name);

      Model::insertPrevSibling(Model::asNode(xml_element), node);

      if (!name.compare("mi") || !name.compare("mo") || !name.compare("mn") || !name.compare("mtext"))
      {
          typename Model::Node node_text = Model::NewText(Model::toModelString(""));
          Model::insertChild(node, node_text);
      }
      return node;
  }

    // 0 - no next Model::Element
    // 1 - has next Model::Element - set it as current for ElementIterator

    // todo remove mrow parent element, if it was constructed dynamically during runtime of program
    bool
    deleteElement(const typename Model::Element& el)
    {
        typename Model::Node curNode  = Model::asNode(el);
        printf("[deleteElement]: preparing to unlink curNode\n");
        typename Model::Node nextNode = Model::asNode(findValidNodeForward(Model::getNextSibling(curNode)));

        Model::unlinkNode(curNode);
        Model::freeNode(curNode);
        if (!nextNode)
            return 0;
        setCurrent(Model::asElement(nextNode));
        return 1;
    }

    bool
    hasValidNodeNext(const typename Model::Element& _p0) const
    {
        typename Model::Node p0 = Model::getNextSibling(Model::asNode(_p0));
        for (typename Model::Node p = p0; p; p = Model::getNextSibling(p))
          if (this->valid(p)) return 1;
        return 0;
    }

    bool
    hasValidNodePrev(const typename Model::Element& _p0) const
    {
        typename Model::Node p0 = Model::getPrevSibling(Model::asNode(_p0));
        for (typename Model::Node p = p0; p; p = Model::getPrevSibling(p))
          if (this->valid(p)) return 1;
        return 0;
    }

  typename Model::Element
  findValidNodeForward(const typename Model::Node& p0) const
  {
    for (typename Model::Node p = p0; p; p = Model::getNextSibling(p))
      if (this->valid(p)) return Model::asElement(p);
    return typename Model::Element();
  }
  
  typename Model::Element
  findValidNodePrev(const typename Model::Node& _p0) const
  {
      typename Model::Node p0 = Model::getPrevSibling(_p0);
    for (typename Model::Node p = p0; p; p = Model::getPrevSibling(p))
      if (this->valid(p)) return Model::asElement(p);
    return typename Model::Element();
  }

  typename Model::Element currentElement;
};

#endif // __TemplateElementIterator_hh__
