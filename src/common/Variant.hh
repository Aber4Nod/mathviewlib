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

#ifndef __Variant_hh__
#define __Variant_hh__

#include <vector>

#include "SmartPtr.hh"
#include "Value.hh"

template <typename T>
class Variant : public Value
{
protected:
  Variant(const T& v) : value(v) { }
  virtual ~Variant() { }

public:
  static SmartPtr<Variant> create(const T& v) { return new Variant(v); }
  T getValue(void) const { return value; }

private:
  T value;
};

template <>
class Variant<void> : public Value
{
protected:
  Variant(void) { }
  virtual ~Variant() { }

public:
  static SmartPtr< Variant<void> > create(void)
  { return new Variant(); }
};

// The following full specialization is for sequences of Values
// There are more accessor methods and getValue returns a
// const reference instead of a copy fo the object
template <>
class Variant< std::vector< SmartPtr<Value> > > : public Value
{
protected:
  Variant(const std::vector< SmartPtr<Value> >& v) : content(v) { }
  virtual ~Variant() { }

public:
  static SmartPtr< Variant< std::vector< SmartPtr<Value> > > > create(const std::vector< SmartPtr<Value> >& v)
  { return new Variant(v); }
  const std::vector< SmartPtr<Value> >& getValue(void) const { return content; }
  unsigned getSize(void) const { return content.size(); }
  SmartPtr<Value> getValue(unsigned i) const { return content[i]; }

private:
  std::vector< SmartPtr<Value> > content;
};

typedef Variant< std::vector< SmartPtr<Value> > > ValueSequence;

template <typename T>
/* */ T
as(const Value* v)
{
  if (const Variant<T>* obj = dynamic_cast<const Variant<T>*>(v))
    return obj->getValue();
  else
    throw Value::TypeError();
}

#endif // __Variant_hh__
