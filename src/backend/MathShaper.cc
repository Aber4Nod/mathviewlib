// Copyright (C) 2000-2007, Luca Padovani <padovani@sti.uniurb.it>.
// Copyright (C) 2013, Khaled Hosny <khaledhosny@eglug.org>.
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
#include <hb.h>

#include <vector>

#include "Area.hh"
#include "AreaFactory.hh"
#include "MathShaper.hh"
#include "ShapingContext.hh"
#include "GlyphArea.hh"

#include <iostream>

MathShaper::MathShaper(const hb_font_t* font)
  : m_font(font)
{
  m_mathfont = MathFont::create(font);
}

MathShaper::~MathShaper()
{ }

void
MathShaper::shape(ShapingContext& context) const
{
  std::cout << "[MathShaper]::shape // shaping this char: " << context.thisChar() << std::endl;
  if (context.nextChar())
      std::cout << "[MathShaper]::shape // shaping next char: " << context.nextChar() << std::endl;
  hb_font_t* font = const_cast<hb_font_t*>(m_font);
  hb_face_t* face = hb_font_get_face(font);
  int upem = hb_face_get_upem(face);

  UCS4String source = context.getSource();
  // std::cout << "[MathShaper]::shape // context source: " << StringOfUCS4String(context.getSource()).c_str() << std::endl;
  hb_buffer_t* buffer = hb_buffer_create();

  hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
  hb_buffer_set_script(buffer, hb_script_from_string("Math", -1));
  
  // std::cout << "[MathShaper]::shape // source.c_str(): " << source.c_str() << std::endl;
  hb_buffer_add_utf32(buffer, source.c_str(), source.length(), 0, source.length());

  int scriptLevel = context.getScriptLevel();
  if (scriptLevel > 0)
  {
      // std::cout << "[MathShaper]::shape // shaping with scipt value: " << scriptLevel << std::endl;
      hb_feature_t features[] = {
        { HB_TAG('s','s','t','y'), (unsigned)scriptLevel, 0, (unsigned)-1 },
        { 0, 0, 0, 0 }
      };
      hb_shape(font, buffer, features, 1);
  }
  else
  {
      // std::cout << "[MathShaper]::shape // shaping without scipt value" << std::endl;
      hb_shape(font, buffer, nullptr, 0);
  }

  unsigned len = hb_buffer_get_length(buffer);
  hb_glyph_info_t* glyphs = hb_buffer_get_glyph_infos(buffer, nullptr);

  const SmartPtr<AreaFactory> factory = context.getFactory();
  std::vector<AreaRef> areaV;

  for (unsigned i = 0; i < len; i++)
    {
      unsigned variantId, glyphId;

      glyphId = glyphs[i].codepoint;

      std::cout << "[MathShaper]::shape // getting GlyphArea with glyphId: " << glyphs[i].codepoint << ", len: " << len << std::endl;
      AreaRef glyphArea = getGlyphArea(glyphId, context.getSize());
      variantId = glyphId;

      if (glyphArea->box().verticalExtent() < context.getVSpan())
        {
          assert(len == 1);
          scaled span = (context.getVSpan() * upem).getValue() / context.getSize().getValue();
          variantId = m_mathfont->getVariant(variantId, span, false);
        }

      if (glyphArea->box().horizontalExtent() < context.getHSpan())
        {
          assert(len == 1);
          scaled span = (context.getHSpan() * upem).getValue() / context.getSize().getValue();
          variantId = m_mathfont->getVariant(variantId, span, true);
        }

      if (variantId != glyphId)
        glyphArea = getGlyphArea(variantId, context.getSize()); // going to Qt_Shaper from format stringnode

      areaV.push_back(glyphArea);
    }
    
  SmartPtr<HorizontalArrayArea> harea = factory->horizontalArray(areaV);
  std::cout << "[MathShaper::shape]: pushing to harea: " << harea << std::endl;
  for (const auto & elem : areaV)
  {
      SmartPtr<const class GlyphArea> garea = elem->getGlyphArea();
      garea->setParent(harea);
  }

  context.pushArea(source.length(), harea);

  hb_buffer_destroy(buffer);
}

bool
MathShaper::shapeCombiningChar(const ShapingContext&) const
{
  return false;
}

bool
MathShaper::computeCombiningCharOffsetsAbove(const AreaRef& base,
                                             const AreaRef& script,
                                             scaled& dx,
                                             scaled& dy) const
{
  //default value of dx and dy
  dx = (base->box().width - script->box().width) / 2;
  dy = base->box().height + script->box().depth;

  std::cout << "[MathShaper]::computeCombiningCharOffsetsAbove // computed offsets: dx = " << dx.getValue() << " dy = " << dy.getValue() << std::endl;
  return true;
}

bool
MathShaper::computeCombiningCharOffsetsBelow(const AreaRef& base,
                                             const AreaRef& script,
                                             scaled& dxUnder) const
{
  dxUnder = (base->box().width - script->box().width) / 2;

  std::cout << "[MathShaper]::computeCombiningCharOffsetsBelow // computed offset: dxUnder = " << dxUnder.getValue() << std::endl;
  return true;
}
