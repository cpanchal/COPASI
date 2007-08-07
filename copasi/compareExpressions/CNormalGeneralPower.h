// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/compareExpressions/CNormalGeneralPower.h,v $
//   $Revision: 1.1 $
//   $Name:  $
//   $Author: gauges $
//   $Date: 2007/08/07 16:49:49 $
// End CVS Header

// Copyright (C) 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#ifndef COPASI_CNormalGeneralPower_H__
#define COPASI_CNormalGeneralPower_H__

#include <iostream>
#include <string>
#include "CNormalBase.h"

class CNormalFraction;

class CNormalGeneralPower: public CNormalBase
  {
  public:
    enum Type
    {
      POWER = 0,
      MODULO,
      INVALID
    };

  protected:
    Type mType;
    CNormalFraction* mpLeft;
    CNormalFraction* mpRight;

    static char* SYMBOLS[];

  public:
    CNormalGeneralPower();
    CNormalGeneralPower(const CNormalGeneralPower& src);
    virtual ~CNormalGeneralPower();

    CNormalGeneralPower& operator=(const CNormalGeneralPower& src);

    bool operator==(const CNormalGeneralPower& rhs) const;
    bool operator<(const CNormalGeneralPower& rhs) const;

    virtual CNormalGeneralPower* copy() const;
    virtual bool simplify();

    virtual std::string toString() const;

    CNormalFraction& getLeft();
    const CNormalFraction& getLeft() const;
    void setLeft(const CNormalFraction& left);
    CNormalFraction& getRight();
    const CNormalFraction& getRight() const;
    void setRight(const CNormalFraction& right);
    Type getType() const;
    void setType(Type type);
  };

std::ostream& operator<<(std::ostream& os, const CNormalGeneralPower& pow);

#endif /* COPASI_CNormalGeneralPower_H__ */