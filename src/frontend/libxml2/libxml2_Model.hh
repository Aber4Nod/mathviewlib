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

#ifndef __libxml2_Model_hh__
#define __libxml2_Model_hh__

#include <libxml/tree.h>
#include <cassert>

#include "String.hh"

#include "TemplateNodeIterator.hh"
#include "TemplateElementIterator.hh"

struct libxml2_Model
{
  enum {
    TEXT_NODE = XML_TEXT_NODE,
    ELEMENT_NODE = XML_ELEMENT_NODE
  };

  typedef xmlNode* Node;
  typedef xmlElement* Element;
  typedef xmlDoc* Document;
  typedef TemplateNodeIterator<libxml2_Model> NodeIterator;
  typedef TemplateElementIterator<libxml2_Model> ElementIterator;

  // method for parsing a model
  // MUST be available
  static xmlDoc* document(const class AbstractLogger&, const String&, bool = false);
  static xmlDoc* documentFromBuffer(const class AbstractLogger&, const String&, bool = false);

  // method for freeing a document
  // MUST be available, may be noop
  static void freeDocument(xmlDoc*);

  static Element getDocumentElement(const Document&);

  // methods for casting an element to a node and viceversa
  static Node asNode(const Element& el) { return (Node) el; }
  static Element asElement(const Node& n) { return (n && n->type == XML_ELEMENT_NODE) ? (xmlElement*) n : 0; }

  // methods for querying nodes
  // MUST be available for TemplateBuilder to work
  static unsigned getNodeType(const Node& n) { return n->type; }
  static String getNodeName(const Node&);
  static String getNodeValue(const Node&);
  static String getNodeNamespaceURI(const Node&);

  // methods for querying elements
  // MUST be available for TemplateBuilder to work
  static String getElementValue(const Element&);
  static String getAttribute(const Element&, const String&);
  // MUST be implemented if the default RefinementContext is used
  static bool hasAttribute(const Element&, const String&);

  // methods for navigating the model
  // must be available if the default iterators are used
  static Node getParent(const Node& n)
  { return n->parent; }
  static Node getFirstChild(const Node& n)
  { return n->children; }
  static Node getNextSibling(const Node& n)
  { return n->next; }
  static Node getPrevSibling(const Node& n)
  { return n->prev; }
  static void setNextSibling(const Node& curNode, const Node& nextNode)
  { curNode->next = nextNode; }
  static void setPrevSibling(const Node& curNode, const Node& prevNode)
  { curNode->prev = prevNode; }
  static void setParent(const Node& curNode, const Node& parent)
  { curNode->parent = parent; }

  // auxiliary conversion functions from/to libxml2 strings
  static String fromModelString(const xmlChar* str) { return reinterpret_cast<const String::value_type*>(str); }
  static const xmlChar* toModelString(const String& str) { return BAD_CAST(str.c_str()); }

  static void setNodeValue(const Node& n, const String& str);
  static void unlinkNode(const Node& n);
  static void freeNode(const Node& n);
  static Node createNode(xmlNsPtr ns, const String& str)
  { return xmlNewNode(ns, xmlStrdup(toModelString(str))); };
  static xmlNsPtr getNodeNamespace(const Node&);
  static Node insertNextSibling(const Node& cur, const Node& elem)
  { return xmlAddNextSibling(cur, elem); }
  static Node insertPrevSibling(const Node& cur, const Node& elem)
  { return xmlAddPrevSibling(cur, elem); }
  static Node insertChild(const Node& parent, const Node& child)
  { return xmlAddChild(parent, child); }

  // MUST be available if the default linker is used
  struct Hash
  {
    size_t operator()(xmlElement* el) const
    {
      assert(el);
      assert(sizeof(size_t) == sizeof(xmlElement*));
      return reinterpret_cast<size_t>(el);
    }
  };


};

#endif // __libxml2_Model_hh__
