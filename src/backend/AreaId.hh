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

#ifndef __AreaId_hh__
#define __AreaId_hh__

#include <vector>

#include "Area.hh"
#include "Point.hh"

class AreaId
{
public:
  AreaId(const AreaRef& r) : root(r) { };

  void append(AreaIndex);
  void append(AreaIndex, const AreaRef&);
  void append(AreaIndex, const AreaRef&, const scaled&, const scaled&);
  void pop_back(void);

  bool empty(void) const { return pathV.empty(); }
  int size(void) const { return pathV.size(); }
  void clear(void) { pathV.clear(); areaV.clear(); originV.clear(); lengthV.clear(); }

  AreaRef getArea(int = -1) const;
  void getOrigin(struct Point&, int = 0, int = -1) const;
  void accumulateOrigin(struct Point&, int = 0, int = -1) const;
  CharIndex getLength(int = 0, int = -1) const;

  typedef std::vector<AreaIndex> PathVector;
  typedef std::vector<AreaRef> AreaVector;
  typedef std::vector<Point> OriginVector;
  typedef std::vector<CharIndex> LengthVector;

  const PathVector& getPath(void) const { return pathV; }
  const AreaVector& getAreas(void) const { validateAreas(); return areaV; }
  const OriginVector& getOrigins(void) const { validateOrigins(); return originV; }
  const LengthVector& getLengths(void) const { validateLengths(); return lengthV; }

protected:
  AreaId(const AreaRef& r, const PathVector::const_iterator& begin, const PathVector::const_iterator& end)
    : root(r), pathV(begin, end) { }

  void accumulateOriginAux(const OriginVector::const_iterator&, const OriginVector::const_iterator&, struct Point&) const;
  void accumulateLengthAux(const LengthVector::const_iterator&, const LengthVector::const_iterator&, CharIndex&) const;

private:
  void validateAreas(void) const;
  void validateOrigins(void) const;
  void validateLengths(void) const;

  const AreaRef root;
  PathVector pathV;
  mutable AreaVector areaV;
  mutable OriginVector originV;
  mutable LengthVector lengthV;
};

#endif // __AreaId_hh__

