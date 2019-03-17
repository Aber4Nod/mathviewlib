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

#ifndef __TemplateBuilder_hh__
#define __TemplateBuilder_hh__

#include <vector>

#include "defs.h"
#include "Attribute.hh"
#include "MathML.hh"
#include "MathMLTableContentFactory.hh"
#include "MathMLNamespaceContext.hh"
#include "MathMLAttributeSignatures.hh"
#include "ValueConversion.hh"
#include "AbstractLogger.hh"
#include <unordered_map>
#include "StringHash.hh"
#include <iostream>

template <class Model, class Builder, class RefinementContext>
class TemplateBuilder : public Builder
{
protected:
  template <typename ElementBuilder>
  SmartPtr<typename ElementBuilder::type>
  getElement(const typename Model::Element& el) const
  {
    SmartPtr<typename ElementBuilder::type> elem;
    elem = smart_cast<typename ElementBuilder::type>(this->linkerAssoc(el));
    if (elem)
      return elem;

    elem = ElementBuilder::type::create(ElementBuilder::getContext(*this));
    this->linkerAdd(el, elem);
    return elem;
  }

  template <typename ElementBuilder>
  typename Model::Node
  createNode(const typename Model::NameSpace& ns, typename Model::Node& new_elem) const
  {
      // todo make creating default nodes like in updateElement
      return ElementBuilder::create(*this, ns, new_elem);
  }
  
  template <typename ElementBuilder>
  SmartPtr<typename ElementBuilder::target_type>
  updateElement(const typename Model::Element& el) const
  {
    SmartPtr<typename ElementBuilder::type> elem = getElement<ElementBuilder>(el);
#if 0
    std::cerr << "BEFORE"
	      << " updateElement " << static_cast<typename ElementBuilder::type*>(elem)
	      << " dirtyAttribute=" << elem->dirtyAttribute() 
	      << " dirtyAttributeP=" << elem->dirtyAttributeP()
	      << " dirtyStructure=" << elem->dirtyStructure()
	      << " dirtyLayout=" << elem->dirtyLayout() << std::endl;
#endif
    if (elem->dirtyAttribute() || elem->dirtyAttributeP() || elem->dirtyStructure())
      {
          std::cout << "Current element address: " << elem << std::endl;
	ElementBuilder::begin(*this, el, elem);
	ElementBuilder::refine(*this, el, elem);
	ElementBuilder::construct(*this, el, elem);
	ElementBuilder::end(*this, el, elem);
      }
#if 0
    std::cerr << "AFTER"
	      << " updateElement " << static_cast<typename ElementBuilder::type*>(elem)
	      << " dirtyAttribute=" << elem->dirtyAttribute() 
	      << " dirtyAttributeP=" << elem->dirtyAttributeP()
	      << " dirtyStructure=" << elem->dirtyStructure()
	      << " dirtyLayout=" << elem->dirtyLayout() << std::endl;
#endif
    // if (elem->deleteSet())
    // {        
        // Model::freeNode(Model::getFirstChild(Model::asNode(el)));
        // Model::setNodeValue(n, "");
        // forgetElement(elem);
        // elem->resetFlag(MathMLElement::FDeleteSet);
        // std::cout << "returning nullptr" << std::endl;
        // return nullptr;
    // }
    return elem;
  }

  TemplateBuilder()
  {
    static struct
    {
      String tag;
      MathMLUpdateMethod update;
      MathMLCreateNode create;
    } mathml_tab[] = {
      { "math",          &TemplateBuilder::template updateElement<MathML_math_ElementBuilder> },
      { "mi",            &TemplateBuilder::template updateElement<MathML_mi_ElementBuilder> ,       &TemplateBuilder::template createNode<MathML_mi_ElementBuilder> },
      { "mn",            &TemplateBuilder::template updateElement<MathML_mn_ElementBuilder> },
      { "mo",            &TemplateBuilder::template updateElement<MathML_mo_ElementBuilder> },
      { "mtext",         &TemplateBuilder::template updateElement<MathML_mtext_ElementBuilder> },
      { "mspace",        &TemplateBuilder::template updateElement<MathML_mspace_ElementBuilder> },
      { "ms",            &TemplateBuilder::template updateElement<MathML_ms_ElementBuilder> },
      { "mrow",          &TemplateBuilder::template updateElement<MathML_mrow_ElementBuilder> },
      { "mfrac",         &TemplateBuilder::template updateElement<MathML_mfrac_ElementBuilder> ,    &TemplateBuilder::template createNode<MathML_mfrac_ElementBuilder> },
      { "msqrt",         &TemplateBuilder::template updateElement<MathML_msqrt_ElementBuilder> ,    &TemplateBuilder::template createNode<MathML_msqrt_ElementBuilder> },
      { "mroot",         &TemplateBuilder::template updateElement<MathML_mroot_ElementBuilder> ,    &TemplateBuilder::template createNode<MathML_mroot_ElementBuilder> },
      { "mstyle",        &TemplateBuilder::template updateElement<MathML_mstyle_ElementBuilder> },
      { "merror",        &TemplateBuilder::template updateElement<MathML_merror_ElementBuilder> },
      { "mpadded",       &TemplateBuilder::template updateElement<MathML_mpadded_ElementBuilder> },
      { "mphantom",      &TemplateBuilder::template updateElement<MathML_mphantom_ElementBuilder> },
      { "mfenced",       &TemplateBuilder::update_MathML_mfenced_Element },
      { "msub",          &TemplateBuilder::template updateElement<MathML_msub_ElementBuilder> ,     &TemplateBuilder::template createNode<MathML_msub_ElementBuilder> },
      { "msup",          &TemplateBuilder::template updateElement<MathML_msup_ElementBuilder> ,     &TemplateBuilder::template createNode<MathML_msup_ElementBuilder> },
      { "msubsup",       &TemplateBuilder::template updateElement<MathML_msubsup_ElementBuilder> ,  &TemplateBuilder::template createNode<MathML_msubsup_ElementBuilder> },
      { "munder",        &TemplateBuilder::template updateElement<MathML_munder_ElementBuilder> ,  &TemplateBuilder::template createNode<MathML_munder_ElementBuilder> },
      { "mover",         &TemplateBuilder::template updateElement<MathML_mover_ElementBuilder> ,  &TemplateBuilder::template createNode<MathML_mover_ElementBuilder> },
      { "munderover",    &TemplateBuilder::template updateElement<MathML_munderover_ElementBuilder> ,  &TemplateBuilder::template createNode<MathML_munderover_ElementBuilder> },
      { "mmultiscripts", &TemplateBuilder::template updateElement<MathML_mmultiscripts_ElementBuilder> },
      { "mtable",        &TemplateBuilder::template updateElement<MathML_mtable_ElementBuilder> },
      { "mtd",           &TemplateBuilder::template updateElement<MathML_mtd_ElementBuilder> },
      { "maligngroup",   &TemplateBuilder::template updateElement<MathML_maligngroup_ElementBuilder> },
      { "malignmark",    &TemplateBuilder::template updateElement<MathML_malignmark_ElementBuilder> },
      { "maction",       &TemplateBuilder::template updateElement<MathML_maction_ElementBuilder> },
      { "menclose",      &TemplateBuilder::template updateElement<MathML_menclose_ElementBuilder> },
      { "semantics",     &TemplateBuilder::update_MathML_semantics_Element },

      { "",              0 }
    };

    if (!mathmlMapInitialized)
      {
	for (unsigned i = 0; mathml_tab[i].update; i++) {
	  mathmlMap[mathml_tab[i].tag].updateMethod = mathml_tab[i].update;
      mathmlMap[mathml_tab[i].tag].createMethod = mathml_tab[i].create;
    }

	mathmlMapInitialized = true;
      }
  }

  ////////////////////////////////////
  // SPECIALIZED MATHML UPDATE METHODS
  ////////////////////////////////////

  SmartPtr<MathMLElement>
  update_MathML_mfenced_Element(const typename Model::Element& el) const
  {
    String open = ToString(getAttributeValue(el, ATTRIBUTE_SIGNATURE(MathML, Fenced, open)));
    String close = ToString(getAttributeValue(el, ATTRIBUTE_SIGNATURE(MathML, Fenced, close)));
    String separators = ToString(getAttributeValue(el, ATTRIBUTE_SIGNATURE(MathML, Fenced, separators)));

    std::vector<SmartPtr<MathMLElement> > content;
    getChildMathMLElements(el, content);

    SmartPtr<MathMLOperatorElement> openElem = MathMLOperatorElement::create(this->getMathMLNamespaceContext());
    openElem->setSize(0);
    openElem->append(open);
    openElem->SetFence();

    SmartPtr<MathMLOperatorElement> closeElem = MathMLOperatorElement::create(this->getMathMLNamespaceContext());
    closeElem->setSize(0);
    closeElem->append(close);
    closeElem->SetFence();
      
    std::vector< SmartPtr<MathMLElement> > outerRowContent;
    outerRowContent.reserve(3);
    outerRowContent.push_back(openElem);
    if (content.size() == 1)
      outerRowContent.push_back(content[0]);
    else
      {
	std::vector< SmartPtr<MathMLElement> > innerRowContent;
	if (content.size() > 0)
	  innerRowContent.reserve(separators.empty() ? content.size() : (2 * content.size() - 1));
	for (unsigned i = 0; i < content.size(); i++)
	  {
	    innerRowContent.push_back(content[i]);
	    if (!separators.empty() && i + 1 < content.size())
	      {
		SmartPtr<MathMLOperatorElement> sep = MathMLOperatorElement::create(this->mathmlContext);
		unsigned offset = (i < separators.length()) ? i : separators.length() - 1;
		sep->setSize(0);
		sep->append(separators.substr(offset, 1));
		sep->SetSeparator();
		innerRowContent.push_back(sep);
	      }
	  }
	SmartPtr<MathMLRowElement> innerRow = MathMLRowElement::create(this->getMathMLNamespaceContext());
	innerRow->swapContent(innerRowContent);
	outerRowContent.push_back(innerRow);
      }
    outerRowContent.push_back(closeElem);

    // this can probably remain attached to the model
    SmartPtr<MathMLRowElement> outerRow = MathMLRowElement::create(this->getMathMLNamespaceContext());
    outerRow->swapContent(outerRowContent);

    return outerRow;
  }

  SmartPtr<MathMLElement>
  update_MathML_semantics_Element(const typename Model::Element& el) const
  {
    typename Model::ElementIterator iter(el, MATHML_NS_URI);
    if (iter.more())
      {
      if (typename Model::Element e = iter.element())
        {
	if (Model::getNodeName(Model::asNode(e)) != "annotation"
	    && Model::getNodeName(Model::asNode(e)) != "annotation-xml")
          {
	  if (SmartPtr<MathMLElement> elem = getMathMLElementNoCreate(iter.element()))
	    return elem;
	  else
	    iter.next();
          }
        }
      }

    while (typename Model::Element e = iter.element())
      {
	if (Model::getNodeName(Model::asNode(e)) == "annotation-xml")
	  {
	    String encoding = Model::getAttribute(e, "encoding");
	    if (encoding == "MathML-Presentation")
	      return getMathMLElement(typename Model::ElementIterator(e, MATHML_NS_URI).element());
	  }
	iter.next();
      }

    return createMathMLDummyElement();
  }

  SmartPtr<MathMLTextNode>
  update_MathML_mglyph_Node(const typename Model::Element& el) const
  {
    assert(el);
    
    String alt        = Model::getAttribute(el, "alt");
    String fontFamily = Model::getAttribute(el, "fontfamily");
    String index      = Model::getAttribute(el, "index");
    
    if (alt.empty() || fontFamily.empty() || index.empty())
      {
	this->getLogger()->out(LOG_WARNING, "malformed `mglyph' element (some required attribute is missing)\n");
	return MathMLStringNode::create("?");
      }
    
    return MathMLGlyphNode::create(fontFamily, index, alt);
  }

  SmartPtr<MathMLTextNode>
  update_MathML_malignmark_Node(const typename Model::Element& el) const
  {
    assert(el);
    
    const String edge = Model::getAttribute(el, "edge");
    
    TokenId align = T__NOTVALID;
    
    if (!edge.empty())
      {
	if      (edge == "left") align = T_LEFT;
	else if (edge == "right") align = T_RIGHT;
	else
	  this->getLogger()->out(LOG_WARNING,
				 "malformed `malignmark' element, attribute `edge' has invalid value `%s' (ignored)",
				 std::string(edge).c_str());
      }
    
    return MathMLMarkNode::create(align);
  }

  //////////////////
  // MATHML BUILDERS
  //////////////////

  struct MathMLElementBuilder
  {
    typedef MathMLElement target_type;

    static SmartPtr<MathMLNamespaceContext>
    getContext(const TemplateBuilder& builder)
    { return builder.getMathMLNamespaceContext(); }

    static void
    begin(const TemplateBuilder&, const typename Model::Element&, const SmartPtr<MathMLElement>&)
    { }

    static void
    end(const TemplateBuilder&, const typename Model::Element&, const SmartPtr<MathMLElement>&)
    { }

    static void
    refine(const TemplateBuilder&, const typename Model::Element&, const SmartPtr<MathMLElement>&)
    { }

    static void
    construct(const TemplateBuilder&, const typename Model::Element&, const SmartPtr<MathMLElement>&)
    { }
    
    static typename Model::Node
    create(const TemplateBuilder&, const typename Model::NameSpace& ns, typename Model::Node& new_elem)
    {
        return Model::createNode(ns, "mi");
    }
  };

  struct MathMLBinContainerElementBuilder : public MathMLElementBuilder
  {
    static void
    construct(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLBinContainerElement>& elem)
    { elem->setChild(builder.getMathMLElement(el)); }
  };

  struct MathMLNormalizingContainerElementBuilder : public MathMLBinContainerElementBuilder
  {
    static void
    construct(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLNormalizingContainerElement>& elem)
    {
      std::vector<SmartPtr<MathMLElement> > content;
      builder.getChildMathMLElements(el, content);
    
      if (content.size() == 1)
	elem->setChild(content[0]);
      else
	{
	  SmartPtr<MathMLInferredRowElement> row = MathMLInferredRowElement::create(builder.getMathMLNamespaceContext());
	  row->swapContent(content);
	  elem->setChild(row);
	}
    }
  };

  struct MathMLLinearContainerElementBuilder : public MathMLElementBuilder
  {
    static void construct(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLLinearContainerElement>& elem)
    {
      std::vector<SmartPtr<MathMLElement> > content;
      builder.getChildMathMLElements(el, content);
      elem->swapContent(content);
    }
  };

  struct MathMLTokenElementBuilder : public MathMLElementBuilder
  {
    static void
    refine(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLTokenElement>& elem)
    {
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Token, mathvariant));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Token, mathsize));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Token, mathcolor));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Token, mathbackground));
    }

    static void
    construct(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLTokenElement>& elem)
    {
      std::vector<SmartPtr<MathMLTextNode> > content;
      // if (elem->deleteSet())
      // {
          // std::cout << "[MathMLTokenElementBuilder:construct]: FDeleteSet for nodetype: " << Model::getNodeName(Model::asNode(el)) << std::endl;
          // Model::unlinkNode(Model::asNode(el));  
          // Model::freeNode(Model::asNode(el));
          // return;
      // }

      builder.getChildMathMLTextNodes(el, content, elem);
      elem->swapContent(content); // should normalize spaces etc.
      std::cout << "[MathMLTokenElementBuilder:construct]: element: " << elem << " current length of contents: " << elem->getContentSize() << std::endl;
    }
  };

  struct MathML_mi_ElementBuilder : public MathMLTokenElementBuilder
  { 
      typedef MathMLIdentifierElement type;

      static typename Model::Node
      create(const TemplateBuilder& builder, const typename Model::NameSpace& ns, typename Model::Node& new_elem)
      {
          typename Model::Node node = Model::createNode(ns, "mtable");
          Model::setNewProp(node, Model::toModelString("frame"), Model::toModelString("dashed"));
          Model::setNewProp(node, Model::toModelString("equalcolumns"), Model::toModelString("false"));
          Model::setNewProp(node, Model::toModelString("framespacing"), Model::toModelString("0.5mm 0mm"));
        
          typename Model::Node node_mtr = Model::createNewChild(node, 
                Model::getNodeNamespace(node),
                Model::toModelString("mtr"), Model::toModelString(""));
          typename Model::Node node_mtd = Model::createNewChild(node_mtr, 
                Model::getNodeNamespace(node_mtr),
                Model::toModelString("mtd"), Model::toModelString(""));
          // if (setCursor)
          // {
              // Model::replaceNode(Model::asNode(el), node);
              // Model::insertChild(node_mtd, Model::asNode(el));

              // typename Model::Node node_mtd2 = Model::createNewChild(node_mtr, 
                    // Model::getNodeNamespace(node_mtr),
                    // Model::toModelString("mtd"), Model::toModelString(""));
              // typename Model::Node node_default = Model::createNewChild(node_mtd, 
                    // Model::getNodeNamespace(node_mtd),
                    // Model::toModelString("mi"), Model::toModelString(""));
              // typename Model::Node node_text = Model::NewText(Model::toModelString(""));
              // Model::insertChild(node_default, node_text);

              // typename Model::Node node_default = Model::createNode(Model::getNodeNamespace(Model::asNode(el)), "mi");
              // typename Model::Node node_text = Model::NewText(Model::toModelString(""));
              // Model::insertChild(node_default, node_text);

              // Model::insertNextSibling(Model::asNode(el), node_default);
          // }
          // else
          // {
              typename Model::Node node_default = Model::createNewChild(node_mtd, 
                    Model::getNodeNamespace(node_mtd),
                    Model::toModelString("mi"), Model::toModelString(""));
              typename Model::Node node_text = Model::NewText(Model::toModelString(""));
              Model::insertChild(node_default, node_text);

              builder.getMathMLElement(Model::asElement(node))->setFlag(MathMLActionElement::FWrapperSet);
              builder.getMathMLElement(Model::asElement(node_default))->setFlag(MathMLActionElement::FWrapperSet);
              new_elem = node;
          // }

          return node_default;
      }
  };

  struct MathML_mn_ElementBuilder : public MathMLTokenElementBuilder
  { typedef MathMLNumberElement type; };

  struct MathML_mo_ElementBuilder : public MathMLTokenElementBuilder
  {
    typedef MathMLOperatorElement type;

    static void
    refine(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLOperatorElement>& elem)
    {
      MathMLTokenElementBuilder::refine(builder, el, elem);
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Operator, form));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Operator, fence));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Operator, separator));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Operator, lspace));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Operator, rspace));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Operator, stretchy));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Operator, symmetric));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Operator, maxsize));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Operator, minsize));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Operator, largeop));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Operator, movablelimits));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Operator, accent));
    }
  };

  struct MathML_mtext_ElementBuilder : public MathMLTokenElementBuilder
  { typedef MathMLTextElement type; };

  struct MathML_ms_ElementBuilder : public MathMLTokenElementBuilder
  { 
    typedef MathMLStringLitElement type;

    static void
    refine(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLStringLitElement>& elem)
    {
      MathMLTokenElementBuilder::refine(builder, el, elem);
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, StringLit, lquote));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, StringLit, rquote));
    }
  };

  struct MathML_mspace_ElementBuilder : public MathMLElementBuilder
  {
    typedef MathMLSpaceElement type;

    static void
    refine(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLSpaceElement>& elem)
    {
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Space, width));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Space, height));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Space, depth));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Space, linebreak));
    }
  };

  struct MathML_mrow_ElementBuilder : public MathMLLinearContainerElementBuilder
  { typedef MathMLRowElement type; };

  struct MathML_mstyle_ElementBuilder : public MathMLNormalizingContainerElementBuilder
  {
    typedef MathMLStyleElement type;

    static void
    begin(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLStyleElement>&)
    { builder.refinementContext.push(el); }

    static void
    end(const TemplateBuilder& builder, const typename Model::Element&, const SmartPtr<MathMLStyleElement>&)
    { builder.refinementContext.pop(); }

    static void
    refine(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLStyleElement>& elem)
    {
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Style, scriptlevel));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Style, displaystyle));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Style, scriptsizemultiplier));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Style, scriptminsize));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Style, mathcolor));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Style, mathbackground));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Style, color));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Style, background));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Style, negativeveryverythickmathspace));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Style, negativeverythickmathspace));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Style, negativethickmathspace));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Style, negativemediummathspace));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Style, negativethinmathspace));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Style, negativeverythinmathspace));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Style, negativeveryverythinmathspace));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Style, veryverythinmathspace));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Style, verythinmathspace));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Style, thinmathspace));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Style, mediummathspace));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Style, thickmathspace));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Style, verythickmathspace));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Style, veryverythickmathspace));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Style, mathsize));
    }
  };

  struct MathML_merror_ElementBuilder : public MathMLNormalizingContainerElementBuilder
  { typedef MathMLErrorElement type;
  };

  struct MathML_mpadded_ElementBuilder : public MathMLNormalizingContainerElementBuilder
  {
    typedef MathMLPaddedElement type;
    
    static void
    refine(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLPaddedElement>& elem)
    {
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Padded, width));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Padded, lspace));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Padded, height));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Padded, depth));
    }
  };

  struct MathML_mphantom_ElementBuilder : public MathMLNormalizingContainerElementBuilder
  { typedef MathMLPhantomElement type; };

  struct MathML_maligngroup_ElementBuilder : public MathMLElementBuilder
  { 
    typedef MathMLAlignGroupElement type;

    static void
    refine(const TemplateBuilder&, const typename Model::Element&, const SmartPtr<MathMLAlignGroupElement>&)
    {
      // NO ATTRIBUTES TO REFINE???
    }
  };

  struct MathML_malignmark_ElementBuilder : public MathMLElementBuilder
  {
    typedef MathMLAlignMarkElement type;

    static void
    refine(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLAlignMarkElement>& elem)
    { builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, AlignMark, edge)); }
  };

  struct MathML_maction_ElementBuilder : public MathMLLinearContainerElementBuilder
  { 
    typedef MathMLActionElement type;

    static void
    refine(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLActionElement>& elem)
    {
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Action, actiontype));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Action, selection));
    }
  };

  struct MathML_menclose_ElementBuilder : public MathMLNormalizingContainerElementBuilder
  {
    typedef MathMLEncloseElement type;

    static void
    refine(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLEncloseElement>& elem)
    { builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Enclose, notation)); }
  };

  struct MathML_math_ElementBuilder : public MathMLNormalizingContainerElementBuilder
  {
    typedef MathMLmathElement type;

    static void
    refine(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLmathElement>& elem)
    {
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, math, mode));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, math, display));
    }
  };

  struct MathML_mfrac_ElementBuilder : public MathMLElementBuilder
  {
    typedef MathMLFractionElement type;

    static void
    refine(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLFractionElement>& elem)
    {
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Fraction, numalign));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Fraction, denomalign));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Fraction, linethickness));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Fraction, bevelled));
    }

    static void
    construct(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLFractionElement>& elem)
    {
      typename Model::ElementIterator iter(el, MATHML_NS_URI);
      SmartPtr<MathMLElement> element = builder.getMathMLElement(iter.element()); // todo optimize this - w/out double creation of element

      if (element->deleteSet())
      {
          builder.forgetElement(element);
          typename Model::Node node = iter.updateCurrent(el);
          element = builder.getMathMLElement(Model::asElement(node));
      }
      else
      if (element->insertSet())
      {
          typename Model::Node node = iter.insertAfterPrepareMROW(el);
          // element->resetFlag(MathMLActionElement::FInsertSet);
          // todo remove inserting after element (let if be in getmathmlchildelements)
          element = builder.getMathMLElement(Model::asElement(node));
      }
      else
      if (element->insertSetCursor())
      {
          typename Model::Node node = iter.insertAfterPrepareMROW(el);
          // element->resetFlag(MathMLActionElement::FInsertSetCursor);
          element = builder.getMathMLElement(Model::asElement(node));
      }

      if (element->wrapperSet() || element->wrapperIsNeeded())
      {
          iter = TemplateElementIterator<Model>(el, MATHML_NS_URI);
      }

      elem->setNumerator(element);
      iter.next();

      element = builder.getMathMLElement(iter.element()); // todo optimize this - w/out double creation of element
      if (element->deleteSet()) // todo total copipasta from numerator - must be unified
      {
          builder.forgetElement(element);
          typename Model::Node node = iter.updateCurrent(el);
          element = builder.getMathMLElement(Model::asElement(node));
      }
      else
      if (element->insertSet())
      {
          typename Model::Node node = iter.insertAfterPrepareMROW(el);
          // element->resetFlag(MathMLActionElement::FInsertSet);
          element = builder.getMathMLElement(Model::asElement(node));
      }
      else
      if (element->insertSetCursor())
      {
          typename Model::Node node = iter.insertAfterPrepareMROW(el);
          // element->resetFlag(MathMLActionElement::FInsertSetCursor);
          element = builder.getMathMLElement(Model::asElement(node));
      }

      if (element->wrapperSet() || element->wrapperIsNeeded())
      {
          iter = TemplateElementIterator<Model>(el, MATHML_NS_URI);
          iter.next();
      }

      elem->setDenominator(builder.getMathMLElement(iter.element()));
    }

    static typename Model::Node
    create(const TemplateBuilder& builder, const typename Model::NameSpace& ns, typename Model::Node& new_elem)
    {
        typename Model::Node node = Model::createNode(ns, "mfrac");
        typename MathMLBuilderMap::const_iterator m = mathmlMap.find("mi");
        typename Model::Node node_table_1;
        typename Model::Node node_numerator = (builder.*(m->second.createMethod))(Model::getNodeNamespace(node), node_table_1);
        Model::insertChild(node, node_table_1);

        typename Model::Node node_table_2;
        typename Model::Node node_denominator = (builder.*(m->second.createMethod))(Model::getNodeNamespace(node), node_table_2);
        Model::insertNextSibling(node_table_1, node_table_2);

        new_elem = node;
        return node_numerator;
    }
  };

  struct MathML_mroot_ElementBuilder : public MathMLElementBuilder
  {
    typedef MathMLRadicalElement type;

    static void
    construct(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLRadicalElement>& elem)
    {
      typename Model::ElementIterator iter(el, MATHML_NS_URI);
      elem->setBase(builder.getMathMLElement(iter.element()));
      iter.next();
      elem->setIndex(builder.getMathMLElement(iter.element()));
    }

    static typename Model::Node
    create(const TemplateBuilder& builder, const typename Model::NameSpace& ns, typename Model::Node& new_elem)
    {
        typename Model::Node node = Model::createNode(ns, "mroot");
        typename MathMLBuilderMap::const_iterator m = mathmlMap.find("mi");
        typename Model::Node node_table_1;
        typename Model::Node node_base = (builder.*(m->second.createMethod))(Model::getNodeNamespace(node), node_table_1);
        Model::insertChild(node, node_table_1);

        typename Model::Node node_table_2;
        typename Model::Node node_index = (builder.*(m->second.createMethod))(Model::getNodeNamespace(node), node_table_2);
        Model::insertNextSibling(node_table_1, node_table_2);

        new_elem = node;
        return node_base;
    }
  };

  struct MathML_msqrt_ElementBuilder : public MathMLElementBuilder
  {
    typedef MathMLRadicalElement type;

    static void
    construct(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLRadicalElement>& elem)
    {
      std::vector<SmartPtr<MathMLElement> > content;
      builder.getChildMathMLElements(el, content);
      if (content.size() == 1)
	elem->setBase(content[0]);
      else
	{
	  SmartPtr<MathMLInferredRowElement> row = MathMLInferredRowElement::create(builder.getMathMLNamespaceContext());
	  row->swapContent(content);
	  elem->setBase(row);
	}
      elem->setIndex(0);
    }

    static typename Model::Node
    create(const TemplateBuilder& builder, const typename Model::NameSpace& ns, typename Model::Node& new_elem)
    {
        typename Model::Node node = Model::createNode(ns, "msqrt");
        typename MathMLBuilderMap::const_iterator m = mathmlMap.find("mi");
        typename Model::Node node_table_1;
        typename Model::Node node_base = (builder.*(m->second.createMethod))(Model::getNodeNamespace(node), node_table_1);
        Model::insertChild(node, node_table_1);

        new_elem = node;
        return node_base;
    }
  };

  struct MathML_msub_ElementBuilder : public MathMLElementBuilder
  {
    typedef MathMLScriptElement type;

    static void
    refine(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLScriptElement>& elem)
    { builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Script, subscriptshift)); }

    static void
    construct(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLScriptElement>& elem)
    {
      typename Model::ElementIterator iter(el, MATHML_NS_URI);
      elem->setBase(builder.getMathMLElement(iter.element()));
      iter.next();
      elem->setSubScript(builder.getMathMLElement(iter.element()));
      elem->setSuperScript(0);
    }

    static typename Model::Node
    create(const TemplateBuilder& builder, const typename Model::NameSpace& ns, typename Model::Node& new_elem)
    {
        typename Model::Node node = Model::createNode(ns, "msub");
        typename MathMLBuilderMap::const_iterator m = mathmlMap.find("mi");
        typename Model::Node node_table_1;
        typename Model::Node node_base = (builder.*(m->second.createMethod))(Model::getNodeNamespace(node), node_table_1);
        Model::insertChild(node, node_table_1);

        typename Model::Node node_table_2;
        typename Model::Node node_subscript = (builder.*(m->second.createMethod))(Model::getNodeNamespace(node), node_table_2);
        Model::insertNextSibling(node_table_1, node_table_2);

        new_elem = node;
        return node_base;
    }
  };

  struct MathML_msup_ElementBuilder : public MathMLElementBuilder
  {
    typedef MathMLScriptElement type;

    static void
    refine(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLScriptElement>& elem)
    { builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Script, superscriptshift)); }

    static void
    construct(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLScriptElement>& elem)
    {
      typename Model::ElementIterator iter(el, MATHML_NS_URI);
      elem->setBase(builder.getMathMLElement(iter.element()));
      iter.next();
      elem->setSubScript(0);
      elem->setSuperScript(builder.getMathMLElement(iter.element()));
    }

    static typename Model::Node
    create(const TemplateBuilder& builder, const typename Model::NameSpace& ns, typename Model::Node& new_elem)
    {
        typename Model::Node node = Model::createNode(ns, "msup");
        typename MathMLBuilderMap::const_iterator m = mathmlMap.find("mi");
        typename Model::Node node_table_1;
        typename Model::Node node_base = (builder.*(m->second.createMethod))(Model::getNodeNamespace(node), node_table_1);
        Model::insertChild(node, node_table_1);

        typename Model::Node node_table_2;
        typename Model::Node node_subscript = (builder.*(m->second.createMethod))(Model::getNodeNamespace(node), node_table_2);
        Model::insertNextSibling(node_table_1, node_table_2);

        new_elem = node;
        return node_base;
    }
  };
  
  struct MathML_msubsup_ElementBuilder : public MathMLElementBuilder
  {
    typedef MathMLScriptElement type;

    static void
    refine(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLScriptElement>& elem)
    {
      MathML_msub_ElementBuilder::refine(builder, el, elem);
      MathML_msup_ElementBuilder::refine(builder, el, elem);
    }

    static void
    construct(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLScriptElement>& elem)
    {
      typename Model::ElementIterator iter(el, MATHML_NS_URI);
      elem->setBase(builder.getMathMLElement(iter.element()));
      iter.next();
      elem->setSubScript(builder.getMathMLElement(iter.element()));
      iter.next();
      elem->setSuperScript(builder.getMathMLElement(iter.element()));
    }

    static typename Model::Node
    create(const TemplateBuilder& builder, const typename Model::NameSpace& ns, typename Model::Node& new_elem)
    {
        typename Model::Node node = Model::createNode(ns, "msubsup");
        typename MathMLBuilderMap::const_iterator m = mathmlMap.find("mi");
        typename Model::Node node_table_1;
        typename Model::Node node_base = (builder.*(m->second.createMethod))(Model::getNodeNamespace(node), node_table_1);
        Model::insertChild(node, node_table_1);

        typename Model::Node node_table_2;
        typename Model::Node node_subscript = (builder.*(m->second.createMethod))(Model::getNodeNamespace(node), node_table_2);
        Model::insertNextSibling(node_table_1, node_table_2);

        typename Model::Node node_table_3;
        typename Model::Node node_superscript = (builder.*(m->second.createMethod))(Model::getNodeNamespace(node), node_table_3);
        Model::insertNextSibling(node_table_2, node_table_3);

        new_elem = node;
        return node_base;
    }
  };

  struct MathML_munder_ElementBuilder : public MathMLElementBuilder
  {
    typedef MathMLUnderOverElement type;

    static void
    refine(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLUnderOverElement>& elem)
    { builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, UnderOver, accentunder)); }

    static void
    construct(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLUnderOverElement>& elem)
    {
      typename Model::ElementIterator iter(el, MATHML_NS_URI);
      elem->setBase(builder.getMathMLElement(iter.element()));
      iter.next();
      elem->setUnderScript(builder.getMathMLElement(iter.element()));
      elem->setOverScript(0);
    }

    static typename Model::Node
    create(const TemplateBuilder& builder, const typename Model::NameSpace& ns, typename Model::Node& new_elem)
    {
        typename Model::Node node = Model::createNode(ns, "munder");
        typename MathMLBuilderMap::const_iterator m = mathmlMap.find("mi");
        typename Model::Node node_table_1;
        typename Model::Node node_base = (builder.*(m->second.createMethod))(Model::getNodeNamespace(node), node_table_1);
        Model::insertChild(node, node_table_1);

        typename Model::Node node_table_2;
        typename Model::Node node_underscript = (builder.*(m->second.createMethod))(Model::getNodeNamespace(node), node_table_2);
        Model::insertNextSibling(node_table_1, node_table_2);

        new_elem = node;
        return node_base;
    }
  };

  struct MathML_mover_ElementBuilder : public MathMLElementBuilder
  {
    typedef MathMLUnderOverElement type;

    static void
    refine(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLUnderOverElement>& elem)
    { builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, UnderOver, accent)); }

    static void
    construct(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLUnderOverElement>& elem)
    {
      typename Model::ElementIterator iter(el, MATHML_NS_URI);
      elem->setBase(builder.getMathMLElement(iter.element()));
      iter.next();
      elem->setUnderScript(0);
      elem->setOverScript(builder.getMathMLElement(iter.element()));
    }

    static typename Model::Node
    create(const TemplateBuilder& builder, const typename Model::NameSpace& ns, typename Model::Node& new_elem)
    {
        typename Model::Node node = Model::createNode(ns, "mover");
        typename MathMLBuilderMap::const_iterator m = mathmlMap.find("mi");
        typename Model::Node node_table_1;
        typename Model::Node node_base = (builder.*(m->second.createMethod))(Model::getNodeNamespace(node), node_table_1);
        Model::insertChild(node, node_table_1);

        typename Model::Node node_table_2;
        typename Model::Node node_overscript = (builder.*(m->second.createMethod))(Model::getNodeNamespace(node), node_table_2);
        Model::insertNextSibling(node_table_1, node_table_2);

        new_elem = node;
        return node_base;
    }
  };

  struct MathML_munderover_ElementBuilder : public MathMLElementBuilder
  {
    typedef MathMLUnderOverElement type;

    static void
    refine(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLUnderOverElement>& elem)
    {
      MathML_munder_ElementBuilder::refine(builder, el, elem);
      MathML_mover_ElementBuilder::refine(builder, el, elem);
    }

    static void
    construct(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLUnderOverElement>& elem)
    {
      typename Model::ElementIterator iter(el, MATHML_NS_URI);
      elem->setBase(builder.getMathMLElement(iter.element()));
      iter.next();
      elem->setUnderScript(builder.getMathMLElement(iter.element()));
      iter.next();
      elem->setOverScript(builder.getMathMLElement(iter.element()));
    }

    static typename Model::Node
    create(const TemplateBuilder& builder, const typename Model::NameSpace& ns, typename Model::Node& new_elem)
    {
        typename Model::Node node = Model::createNode(ns, "munderover");
        typename MathMLBuilderMap::const_iterator m = mathmlMap.find("mi");
        typename Model::Node node_table_1;
        typename Model::Node node_base = (builder.*(m->second.createMethod))(Model::getNodeNamespace(node), node_table_1);
        Model::insertChild(node, node_table_1);

        typename Model::Node node_table_2;
        typename Model::Node node_underscript = (builder.*(m->second.createMethod))(Model::getNodeNamespace(node), node_table_2);
        Model::insertNextSibling(node_table_1, node_table_2);

        typename Model::Node node_table_3;
        typename Model::Node node_overscript = (builder.*(m->second.createMethod))(Model::getNodeNamespace(node), node_table_3);
        Model::insertNextSibling(node_table_2, node_table_3);

        new_elem = node;
        return node_base;
    }
  };

  struct MathML_mtd_ElementBuilder : public MathMLNormalizingContainerElementBuilder
  { typedef MathMLTableCellElement type; };

  struct MathML_mtable_ElementBuilder : public MathMLElementBuilder
  {
    typedef MathMLTableElement type;

    static void
    refine(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLTableElement>& elem)
    {
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Table, align));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Table, rowalign));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Table, columnalign));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Table, groupalign));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Table, alignmentscope));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Table, columnwidth));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Table, rowspacing));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Table, columnspacing));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Table, rowlines));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Table, columnlines));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Table, frame));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Table, framespacing));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Table, equalrows));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Table, equalcolumns));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Table, displaystyle));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Table, side));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Table, minlabelspacing));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, Table, width));
    }

    static const SmartPtr<Value>
    refineAlignAttribute(const SmartPtr<Value>& cellAlign,
			 const SmartPtr<Value>& rowAlign,
			 const SmartPtr<Value>& tableAlign)
    {
#if 1
      if (cellAlign) return cellAlign;
      else if (rowAlign) return rowAlign;
      else if (tableAlign) return tableAlign;
      else return 0;
#else
      // Because of a bug in GCC-3.4 the following code, which is
      // syntactically and semantically correct, does not compile
      // and the compiler issues a misleading error message
      return (cellAlign ? cellAlign : (rowAlign ? rowAlign : (tableAlign ? tableAlign : 0)));
#endif
    }

    static void
    construct(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLTableElement>& elem)
    {
      MathMLTableContentFactory tableContentFactory;

      unsigned rowIndex = 0;

      const SmartPtr<Value> tableRowAlign = builder.getAttributeValue(el, ATTRIBUTE_SIGNATURE(MathML, Table, rowalign));
      const SmartPtr<Value> tableColumnAlign = builder.getAttributeValue(el, ATTRIBUTE_SIGNATURE(MathML, Table, columnalign));
      const SmartPtr<Value> tableGroupAlign = builder.getAttributeValue(el, ATTRIBUTE_SIGNATURE(MathML, Table, groupalign));
      for (typename Model::ElementIterator iter(el, MATHML_NS_URI); iter.more(); iter.next())
	{
	  typename Model::Element row = iter.element();

	  const SmartPtr<Value> rowRowAlign = builder.getAttributeValue(row, ATTRIBUTE_SIGNATURE(MathML, TableRow, rowalign));
	  const SmartPtr<Value> rowColumnAlign = builder.getAttributeValue(row, ATTRIBUTE_SIGNATURE(MathML, TableRow, columnalign));
	  const SmartPtr<Value> rowGroupAlign = builder.getAttributeValue(row, ATTRIBUTE_SIGNATURE(MathML, TableRow, groupalign));

	  const String name = Model::getNodeName(Model::asNode(row));
	  if (name == "mtr" || name == "mlabeledtr")
	    {
	      unsigned columnIndex = 0;
	      for (typename Model::ElementIterator iter(row, MATHML_NS_URI); iter.more(); iter.next())
		{
		  typename Model::Element cell = iter.element();

		  const SmartPtr<Value> cellRowAlign = builder.getAttributeValue(cell, ATTRIBUTE_SIGNATURE(MathML, TableCell, rowalign));
		  const SmartPtr<Value> cellColumnAlign = builder.getAttributeValue(cell, ATTRIBUTE_SIGNATURE(MathML, TableCell, columnalign));
		  const SmartPtr<Value> cellGroupAlign = builder.getAttributeValue(cell, ATTRIBUTE_SIGNATURE(MathML, TableCell, groupalign));
		  const int cellRowSpan = ToInteger(builder.getAttributeValue(cell, ATTRIBUTE_SIGNATURE(MathML, TableCell, rowspan)));
		  const int cellColumnSpan = ToInteger(builder.getAttributeValue(cell, ATTRIBUTE_SIGNATURE(MathML, TableCell, columnspan)));
		  SmartPtr<MathMLElement> elem = builder.getMathMLElement(cell);
		  SmartPtr<MathMLTableCellElement> cellElem = smart_cast<MathMLTableCellElement>(elem);
		  if (!cellElem)
		    {
		      cellElem = MathMLTableCellElement::create(builder.getMathMLNamespaceContext());
		      cellElem->setChild(elem);
		      // WARNING: should we clear the dirty flags?
		    }
		  if (name == "mtr" || columnIndex > 0)
		    {
		      cellElem->setSpan(cellRowSpan, cellColumnSpan);
		      columnIndex = tableContentFactory.setChild(rowIndex, columnIndex, cellRowSpan, cellColumnSpan, cellElem);
		    }
		  else
		    {
		      cellElem->setSpan(1, 1);
		      tableContentFactory.setLabelChild(rowIndex, cellElem);
		    }

		  // now rowIndex and columnIndex are final values
		  cellElem->setPosition(rowIndex, columnIndex);

		  const SmartPtr<Value> rowAlign =
		    refineAlignAttribute(cellRowAlign,
					 rowRowAlign,
					 GetComponent(tableRowAlign, rowIndex));
		  const SmartPtr<Value> columnAlign =
		    refineAlignAttribute(cellColumnAlign,
					 GetComponent(rowColumnAlign, columnIndex),
					 GetComponent(tableColumnAlign, columnIndex));
		  const SmartPtr<Value> groupAlign =
		    refineAlignAttribute(cellGroupAlign,
					 GetComponent(rowGroupAlign, columnIndex),
					 GetComponent(tableGroupAlign, columnIndex));

		  cellElem->setAlignment(ToTokenId(rowAlign), ToTokenId(columnAlign));
		  //cellElem->setGroupAlignment(groupAlign);
		}

	      rowIndex++;
	    }
	  else
	    {
	      // issue a warning message or promote to mtr with single mtd inside
	    }
	}

      elem->updateContent(tableContentFactory);
    }
  };

  struct MathML_mmultiscripts_ElementBuilder : public MathMLElementBuilder
  {
    typedef MathMLMultiScriptsElement type;

    static void
    refine(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLMultiScriptsElement>& elem)
    {
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, MultiScripts, subscriptshift));
      builder.refineAttribute(elem, el, ATTRIBUTE_SIGNATURE(MathML, MultiScripts, superscriptshift));
    }

    static void
    construct(const TemplateBuilder& builder, const typename Model::Element& el, const SmartPtr<MathMLMultiScriptsElement>& elem)
    {
      typename Model::ElementIterator iter(el, MATHML_NS_URI);
      unsigned i = 0;
      unsigned nScripts = 0;
      unsigned nPreScripts = 0;
      bool preScripts = false;

      elem->setBase(builder.getMathMLElement(iter.element()));
      iter.next();
      while (iter.more())
	{
	  typename Model::Element node = iter.element();
	  assert(node);
	  const String nodeName = Model::getNodeName(Model::asNode(node));
	  if (nodeName == "mprescripts")
	    {
	      if (preScripts)
		builder.getLogger()->out(LOG_WARNING, "multiple <mprescripts> elements in mmultiscript");
	      else
		{
		  if (i % 2 == 1) elem->setSuperScript(nScripts, 0);
		  preScripts = true;
		  i = 0;
		}
	    }
	  else
	    {
	      if (i % 2 == 0) // sub script
		{
		  SmartPtr<MathMLElement> sub = (nodeName == "none") ? 0 : builder.getMathMLElement(node);
		  if (preScripts) elem->setPreSubScript(nPreScripts, sub);
		  else elem->setSubScript(nScripts, sub);
		}
	      else // super script
		{
		  SmartPtr<MathMLElement> sup = (nodeName == "none") ? 0 : builder.getMathMLElement(node);
		  if (preScripts)
		    {
		      elem->setPreSuperScript(nPreScripts, sup);
		      nPreScripts++;
		    }
		  else
		    {
		      elem->setSuperScript(nScripts, sup);
		      nScripts++;
		    }
		}
	      i++;
	    }

	  iter.next();
	}

      if (preScripts && i % 2 == 1) elem->setPreSuperScript(nPreScripts, 0);
      elem->setScriptsSize(nScripts);
      elem->setPreScriptsSize(nPreScripts);
    }
  };

  ////////////////////////////
  // BUILDER AUXILIARY METHODS
  ////////////////////////////

  SmartPtr<Attribute>
  getAttribute(const typename Model::Element& el, const AttributeSignature& signature) const
  {
    SmartPtr<Attribute> attr;
  
    if (signature.fromElement)
      if (Model::hasAttribute(el, signature.name))
	attr = Attribute::create(signature, Model::getAttribute(el, signature.name));

    if (!attr && signature.fromContext)
      attr = refinementContext.get(signature);

    return attr;
  }

  SmartPtr<Value>
  getAttributeValue(const typename Model::Element& el, const AttributeSignature& signature) const
  {
    if (SmartPtr<Attribute> attr = getAttribute(el, signature))
      return attr->getValue();
    else
      return signature.getDefaultValue();
  }

  void
  refineAttribute(const SmartPtr<Element>& elem,
		  const typename Model::Element& el, const AttributeSignature& signature) const
  {
    if (SmartPtr<Attribute> attr = getAttribute(el, signature)) elem->setAttribute(attr);
    else elem->removeAttribute(signature);
  }

  ///////////////////////////////////////
  // BUILDER AUXILIARY METHODS FOR MATHML
  ///////////////////////////////////////

  SmartPtr<MathMLElement>
  getMathMLElementNoCreate(const typename Model::Element& el) const // xml2 element
  {
    if (el)
      {
	std::cout << "createMathMLElement " << Model::getNodeName(Model::asNode(el)) << std::endl;
	typename MathMLBuilderMap::const_iterator m = mathmlMap.find(Model::getNodeName(Model::asNode(el))); // creating node
	if (m != mathmlMap.end()) 
	  {
	    SmartPtr<MathMLElement> elem = (this->*(m->second.updateMethod))(el);
	    if (elem == nullptr)
            return 0;

        if (elem->wrapperSet() && !elem->wrapperIsNeeded())
        {
            std::cout << "entering in wrapperSet block for node: " << Model::getNodeName(Model::asNode(el)) << std::endl;
            typename Model::Node mtr     = Model::getFirstChild(Model::asNode(el));
            typename Model::Node mtd     = Model::getFirstChild(mtr);
            typename Model::Node mtoken  = Model::getFirstChild(mtd);
            SmartPtr<MathMLElement> elem_token = getMathMLElement(Model::asElement(mtoken));
            // TODO: add exception for mathmltoken type - raw content
            if (smart_cast<MathMLTokenElement>(elem_token)->getContentLength())
            {
                // relink nodes (unlink mtr, mtd and replace mtoken)
                std::cout << "beginning ro relinking nodes for node with content: " << smart_cast<MathMLTokenElement>(elem_token)->GetRawContent() << std::endl;

                Model::unlinkNode(mtoken);
                Model::replaceNode(Model::asNode(el), mtoken);
                Model::unlinkNode(Model::asNode(el));
                Model::freeNode(Model::asNode(el));
                
                elem_token->resetFlag(Element::FWrapperSet);
                // el = Model::asElement(mtoken);
                elem = elem_token;
            }
        }
        else
        if (elem->wrapperIsNeeded() && !smart_cast<MathMLTokenElement>(elem)->getContentLength() && !elem->wrapperSet())
        {
            typename Model::Node node = Model::createNode(Model::getNodeNamespace(Model::asNode(el)), "mtable");
            Model::setNewProp(node, Model::toModelString("frame"), Model::toModelString("dashed"));
            Model::setNewProp(node, Model::toModelString("equalcolumns"), Model::toModelString("false"));
            Model::setNewProp(node, Model::toModelString("framespacing"), Model::toModelString("0.5mm 0mm"));
            typename Model::Node node_mtr = Model::createNewChild(node, 
                  Model::getNodeNamespace(node),
                  Model::toModelString("mtr"), Model::toModelString(""));
            typename Model::Node node_mtd = Model::createNewChild(node_mtr, 
                  Model::getNodeNamespace(node_mtr),
                  Model::toModelString("mtd"), Model::toModelString(""));

            std::cout << "beginning getting new table wrapper element" << std::endl;
            elem->setWrapperSet();
            Model::replaceNode(Model::asNode(el), node);
            Model::unlinkNode(Model::asNode(el));
            Model::insertChild(node_mtd, Model::asNode(el));
            elem = getMathMLElement(Model::asElement(node));
            elem->setWrapperSet();
        }

	    elem->resetDirtyStructure();
	    elem->resetDirtyAttribute();
        std::cout << "ended of creation of mathmlelement: " <<  Model::getNodeName(Model::asNode(el)) << std::endl;
	    return elem;
	  }
      }

    return 0;
  }

  SmartPtr<MathMLElement>
  getMathMLElement(const typename Model::Element& el) const
  {
    if (SmartPtr<MathMLElement> elem = getMathMLElementNoCreate(el))
    {
        if (elem->insertSet())
        {
            printf("[getMathMLElement]: insertSet triggered\n");
            // static int counter = 0;
            // if (counter++ > 1)
                // assert(0 == 1);
            // typename Model::Node node = iter.insertAfter(el);
            // _elem = getMathMLElement(Model::asElement(node));
        }
        else
        return elem;
    }
    else {
      printf("[getMathMLElement]: creating createMathMLCursorElement\n");
      return createMathMLDummyElement();
    }
  }

  void
  getChildMathMLElements(const typename Model::Element& el, std::vector<SmartPtr<MathMLElement> >& content) const
  {
      std::cout << "[getChildMathMLElements]: getting child elements" << std::endl;
    content.clear();
    for (typename Model::ElementIterator iter(el, MATHML_NS_URI); iter.more(); iter.next()) {
        SmartPtr<MathMLElement> _elem = getMathMLElement(iter.element());
        if (_elem->insertSet())
        {
            printf("[getChildMathMLElements]: insertSet triggered\n");
            _elem->resetFlag(MathMLActionElement::FInsertSet);
            typename Model::Node node = iter.insertAfter(el);
            _elem = getMathMLElement(Model::asElement(node));
        }
        else
        if (_elem->insertSetCursor())
        {
            typename Model::Node node = iter.insertAfter(el);
            _elem = getMathMLElement(Model::asElement(node));
        }
        else
        if (_elem->cursorSet() && !smart_cast<MathMLTokenElement>(_elem)->getInsertElementName().empty())
        {
            printf("[getChildMathMLElements]: cursorSet triggered\n");
            smart_cast<MathMLTokenElement>(_elem)->setInsertElementName("");

            typename MathMLBuilderMap::const_iterator m = mathmlMap.find("munderover");
            typename Model::Node node_table;
            typename Model::Node node = (this->*(m->second.createMethod))(Model::getNodeNamespace(Model::asNode(iter.element())), node_table); // returning node where cursor must be set
            Model::insertNextSibling(Model::asNode(iter.element()), node_table);
            _elem->resetFlag(MathMLActionElement::FCursorSet);
             getMathMLElement(Model::asElement(node))->setFlag(MathMLActionElement::FCursorSet);
            _elem = getMathMLElement(iter.element());

            // ### old version below
            // typename Model::Node node = (this->*(m->second.createMethod))(el);
            // Model::insertNextSibling(Model::asNode(iter.element()), node);
            // // typename Model::Node node = iter.insertAfter(el, "mtable");
            // // _elem = getMathMLElement(Model::asElement(node));
            // _elem = getMathMLElement(iter.element());
        }
        content.push_back(_elem);

        if (_elem->insertSetCursor())
        {
            _elem->resetFlag(MathMLActionElement::FInsertSetCursor);
            iter.next();

            _elem = getMathMLElement(iter.element());
            _elem->setFlag(MathMLActionElement::FCursorSet);
            content.push_back(_elem);
        }
    }
  }

  void
  getChildMathMLTextNodes(const typename Model::Element& el, std::vector<SmartPtr<MathMLTextNode> >& content, const SmartPtr<MathMLTokenElement>& elem) const
  {
    bool first = true;
    content.clear();
    for (typename Model::NodeIterator iter(Model::asNode(el)); iter.more(); )
      {
	typename Model::Node n = iter.node();
	assert(n);

    // static int32_t counter;
	switch (Model::getNodeType(n))
	  {
	  case Model::TEXT_NODE:
	    {
	      // ok, we have a chunk of text
          String s;
          // if (elem->contentSet())
          // {
              // Model::unlinkNode(n);  
              // Model::freeNode(n);
              // forgetElement(elem);
              // delete elem;
          // }

         std::cout << "[construct]: name of textnode: " << Model::getNodeName(n) << std::endl;
         std::cout << "[construct]: name of parentnode: " << Model::getNodeName(Model::getParent(n)) << std::endl;

          // ------------- block fot changing node content
          
          if (elem->contentSet())
          {
              std::cout << "[construct]: FContentSet is set, value: " << elem->GetRawContent() << " xmlNodeType: " << Model::getNodeType(n) << std::endl;
              Model::setNodeValue(n, elem->GetRawContent());
              elem->resetFlag(MathMLElement::FContentSet);
          }
          // -------------
          iter.next();
          // if (!elem->contentSet()) {
    	      s = collapseSpaces(Model::getNodeValue(n));
    	      

    	      // ...but spaces at the at the beginning (end) are deleted only if this
    	      // is the very first (last) chunk in the token.
    	      if (first) s = trimSpacesLeft(s);
    	      if (!iter.more()) s = trimSpacesRight(s);

    	      content.push_back(createMathMLTextNode(s));
              std::cout << "[getChildMathMLTextNodes]: pushing back textnode: " << s << "current size: " << content.size() << std::endl;
          // }
	    }
	    break;
      
	  case Model::ELEMENT_NODE:
	    {	    
	      if (Model::getNodeNamespaceURI(n) == MATHML_NS_URI)
		{
		  const String nodeName = Model::getNodeName(n);
		  if (nodeName == "mglyph")
		    content.push_back(update_MathML_mglyph_Node(Model::asElement(n)));
		  else if (nodeName == "malignmark")
		    content.push_back(update_MathML_malignmark_Node(Model::asElement(n)));
		}
	    }
	    iter.next();
	    break;
	    
	  default:
	    iter.next();
	    break;
	  }
	first = false;
      }
  }

  SmartPtr<MathMLTextNode>
  createMathMLTextNode(const String& content) const
  {
    if (content == MathMLFunctionApplicationNode::getContent())
      return MathMLFunctionApplicationNode::create();
    else if (content == MathMLInvisibleTimesNode::getContent())
      return MathMLInvisibleTimesNode::create();
    else
      return MathMLStringNode::create(content);
  }

  SmartPtr<MathMLElement>
  createMathMLDummyElement(void) const
  {
    SmartPtr<MathMLElement> elem = MathMLDummyElement::create(this->getMathMLNamespaceContext());
    elem->resetDirtyStructure();
    elem->resetDirtyAttribute();
    return elem;
  }
  
  SmartPtr<MathMLElement>
  createMathMLCursorElement(void) const
  {
    SmartPtr<MathMLElement> elem = MathMLCursorElement::create(this->getMathMLNamespaceContext());
    elem->resetDirtyStructure();
    elem->resetDirtyAttribute();
    return elem;
  }

public:
  static SmartPtr<Builder> create(void) { return new TemplateBuilder(); }

  virtual void
  forgetElement(Element* elem) const
  { this->linkerRemove(elem); }

  virtual SmartPtr<Element>
  getRootElement() const
  {
    if (typename Model::Element root = this->getRootModelElement())
      {
	const String ns = Model::getNodeNamespaceURI(Model::asNode(root));
	if (ns == MATHML_NS_URI) return getMathMLElement(root);
      }
    return 0;
  }

private:
  typedef SmartPtr<class MathMLElement> (TemplateBuilder::* MathMLUpdateMethod)(const typename Model::Element&) const;
  typedef typename Model::Node (TemplateBuilder:: *MathMLCreateNode)(const typename Model::NameSpace&, typename Model::Node&) const;
  typedef struct { MathMLUpdateMethod updateMethod; MathMLCreateNode createMethod; } servingMethods; 
  typedef std::unordered_map<String, servingMethods, StringHash, StringEq> MathMLBuilderMap;
  static MathMLBuilderMap mathmlMap;
  static bool mathmlMapInitialized;
  mutable RefinementContext refinementContext;
};

template <class Model, class Builder, class RefinementContext>
typename TemplateBuilder<Model,Builder,RefinementContext>::MathMLBuilderMap TemplateBuilder<Model,Builder,RefinementContext>::mathmlMap;

template <class Model, class Builder, class RefinementContext>
bool TemplateBuilder<Model,Builder,RefinementContext>::mathmlMapInitialized = false;

#endif // __TemplateBuilder_hh__
