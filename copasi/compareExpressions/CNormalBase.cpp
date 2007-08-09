// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/compareExpressions/CNormalBase.cpp,v $
//   $Revision: 1.1 $
//   $Name:  $
//   $Author: gauges $
//   $Date: 2007/08/08 10:27:29 $
// End CVS Header

// Copyright (C) 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#include "CNormalBase.h"

#include "CNormalFraction.h"
#include "CNormalSum.h"
#include "CNormalProduct.h"
#include "CNormalItem.h"
#include "CNormalItemPower.h"
#include "CNormalGeneralPower.h"
#include "CNormalFunction.h"
//#include "CNormalChoice.h"
//#include "CNormalLogical.h"

bool CNormalBase::areEqual(const CNormalBase& rhs) const
  {
    bool result = false;
    if (dynamic_cast<const CNormalItem*>(this) != NULL && dynamic_cast<const CNormalItem*>(&rhs) != NULL)
      {
        result = (static_cast<const CNormalItem&>(*this) == static_cast<const CNormalItem&>(rhs));
      }
    else if (dynamic_cast<const CNormalFraction*>(this) != NULL && dynamic_cast<const CNormalFraction*>(&rhs) != NULL)
      {
        result = (static_cast<const CNormalFraction&>(*this) == static_cast<const CNormalFraction&>(rhs));
      }
    else if (dynamic_cast<const CNormalSum*>(this) != NULL && dynamic_cast<const CNormalSum*>(&rhs) != NULL)
      {
        result = (static_cast<const CNormalSum&>(*this) == static_cast<const CNormalSum&>(rhs));
      }
    else if (dynamic_cast<const CNormalProduct*>(this) != NULL && dynamic_cast<const CNormalProduct*>(&rhs) != NULL)
      {
        result = (static_cast<const CNormalProduct&>(*this) == static_cast<const CNormalProduct&>(rhs));
      }
    else if (dynamic_cast<const CNormalItemPower*>(this) != NULL && dynamic_cast<const CNormalItemPower*>(&rhs) != NULL)
      {
        result = (static_cast<const CNormalItemPower&>(*this) == static_cast<const CNormalItemPower&>(rhs));
      }
    else if (dynamic_cast<const CNormalGeneralPower*>(this) != NULL && dynamic_cast<const CNormalGeneralPower*>(&rhs) != NULL)
      {
        result = (static_cast<const CNormalGeneralPower&>(*this) == static_cast<const CNormalGeneralPower&>(rhs));
      }
    else if (dynamic_cast<const CNormalFunction*>(this) != NULL && dynamic_cast<const CNormalFunction*>(&rhs) != NULL)
      {
        result = (static_cast<const CNormalFunction&>(*this) == static_cast<const CNormalFunction&>(rhs));
      }
    /*
    else if(dynamic_cast<const CNormalLogical*>(this)!=NULL && dynamic_cast<const CNormalLogical*>(&rhs)!=NULL)
    {
        result=(static_cast<const CNormalLogical&>(*this)==static_cast<const CNormalLogical&>(rhs));
    }
    else if(dynamic_cast<const CNormalChoice*>(this)!=NULL && dynamic_cast<const CNormalChoice*>(&rhs)!=NULL)
    {
        result=(static_cast<const CNormalChoice&>(*this)==static_cast<const CNormalChoice&>(rhs));
    }
    */
    return result;
  }