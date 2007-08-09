// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/compareExpressions/CNormalProduct.cpp,v $
//   $Revision: 1.1 $
//   $Name:  $
//   $Author: gauges $
//   $Date: 2007/08/08 10:27:29 $
// End CVS Header

// Copyright (C) 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#include <set>
#include <math.h>
#include <sstream>
#include "copasi.h"

#include "CNormalSum.h"
#include "CNormalLcm.h"
#include "CNormalItem.h"
#include "CNormalItemPower.h"
#include "CNormalProduct.h"
#include "CNormalFunction.h"
#include "CNormalGeneralPower.h"

bool compareItemPowers::operator()(const CNormalItemPower* itemPower1, const CNormalItemPower* itemPower2)
{
  return *itemPower1 < *itemPower2;
}

/**
 * Default constructor
 */
CNormalProduct::CNormalProduct(): CNormalBase(), mFactor(1.0)
{}

/**
 * Copy contructor
 */
CNormalProduct::CNormalProduct(const CNormalProduct& src): CNormalBase(src), mFactor(src.mFactor)
{
  std::set<CNormalItemPower*, compareItemPowers >::const_iterator it;
  std::set<CNormalItemPower*, compareItemPowers >::const_iterator itEnd = src.mItemPowers.end();
  for (it = src.mItemPowers.begin(); it != itEnd; ++it)
    {
      mItemPowers.insert(new CNormalItemPower(**it));
    }
}

/**
 * Assignment operator
 */
CNormalProduct & CNormalProduct::operator=(const CNormalProduct& src)
{
  mFactor = src.mFactor;
  std::set<CNormalItemPower*, compareItemPowers >::const_iterator it;
  std::set<CNormalItemPower*, compareItemPowers >::const_iterator itEnd = src.mItemPowers.end();
  for (it = src.mItemPowers.begin(); it != itEnd; ++it)
    mItemPowers.insert(new CNormalItemPower(**it));
  return *this;
}

/**
 * Destructor
 */
CNormalProduct::~CNormalProduct()
{
  std::set<CNormalItemPower*, compareItemPowers >::const_iterator it;
  std::set<CNormalItemPower*, compareItemPowers >::const_iterator itEnd = mItemPowers.end();
  for (it = mItemPowers.begin(); it != itEnd; ++it)
    delete *it;
}

/**
 * Set the factor for this product.
 * @return true.
 */
bool CNormalProduct::setFactor(const C_FLOAT64& number)
{
  mFactor = number;
  if (fabs(mFactor) < 1.0E-100)
    {
      mItemPowers.clear();
      //mPowers.clear();
    }
  return true;
}

/**
 * Multiply this product with a number.
 * @return true.
 */
bool CNormalProduct::multiply(const C_FLOAT64& number)
{
  mFactor = mFactor * number;
  if (fabs(mFactor) < 1.0E-100)
    {
      mItemPowers.clear();
      //mPowers.clear();
    }
  return true;
}

/**
 * Multiply an item to this product.
 * @return true.
 */
bool CNormalProduct::multiply(const CNormalGeneralPower& pow)
{
  if (fabs(mFactor) < 1.0E-100)
    return true;
  std::set <CNormalItemPower*, compareItemPowers >::const_iterator it;
  std::set <CNormalItemPower*, compareItemPowers >::const_iterator itEnd = mItemPowers.end();
  for (it = mItemPowers.begin(); it != itEnd; ++it)
    {
      if ((*it)->getItem().areEqual(pow) == true)
        {
          (*it)->setExp((*it)->getExp() + 1.0);
          return true;
        }
    }
  CNormalItemPower* tmp = new CNormalItemPower(pow, 1.0);
  mItemPowers.insert(tmp);
  return true;
}

/**
 * Multiply an item to this product.
 * @return true.
 */
bool CNormalProduct::multiply(const CNormalFunction& fun)
{
  if (fabs(mFactor) < 1.0E-100)
    return true;
  std::set <CNormalItemPower*, compareItemPowers >::const_iterator it;
  std::set <CNormalItemPower*, compareItemPowers >::const_iterator itEnd = mItemPowers.end();
  for (it = mItemPowers.begin(); it != itEnd; ++it)
    {
      if ((*it)->getItem().areEqual(fun) == true)
        {
          (*it)->setExp((*it)->getExp() + 1.0);
          return true;
        }
    }
  CNormalItemPower* tmp = new CNormalItemPower(fun, 1.0);
  mItemPowers.insert(tmp);
  return true;
}

/**
 * Multiply an item to this product.
 * @return true.
 */
bool CNormalProduct::multiply(const CNormalItem& item)
{
  if (fabs(mFactor) < 1.0E-100)
    return true;
  std::set <CNormalItemPower*, compareItemPowers >::const_iterator it;
  std::set <CNormalItemPower*, compareItemPowers >::const_iterator itEnd = mItemPowers.end();
  for (it = mItemPowers.begin(); it != itEnd; ++it)
    {
      if ((*it)->getItem().areEqual(item) == true)
        {
          (*it)->setExp((*it)->getExp() + 1.0);
          return true;
        }
    }
  CNormalItemPower* tmp = new CNormalItemPower(item, 1.0);
  mItemPowers.insert(tmp);
  return true;
}

/**
 * Multiply an itempower to this product.
 * @return true.
 */
bool CNormalProduct::multiply(const CNormalItemPower& itemPower)
{
  if (fabs(mFactor) < 1.0E-100)
    return true;
  std::set <CNormalItemPower*, compareItemPowers >::const_iterator it;
  std::set <CNormalItemPower*, compareItemPowers >::const_iterator itEnd = mItemPowers.end();
  for (it = mItemPowers.begin(); it != itEnd; ++it)
    {
      if ((*it)->getItem().areEqual(itemPower.getItem()))
        {
          (*it)->setExp((*it)->getExp() + itemPower.getExp());
          return true;
        }
    }
  CNormalItemPower* tmp = new CNormalItemPower(itemPower);
  mItemPowers.insert(tmp);
  return true;
}

/**
 * Multiply a list of itempowers to this product.
 * @return true.
 */
bool CNormalProduct::multiply(const std::set <CNormalItemPower*, compareItemPowers >& itemPowers)
{
  if (fabs(mFactor) < 1.0E-100)
    return true;
  std::set <CNormalItemPower*, compareItemPowers >::const_iterator it;
  std::set <CNormalItemPower*, compareItemPowers >::const_iterator itEnd = itemPowers.end();
  for (it = itemPowers.begin(); it != itEnd; ++it)
    multiply(**it);
  return true;
}

/**
 * Remove an itempower from this product, provided it is a factor of it.
 * @return true.
 */
bool CNormalProduct::remove(std::set<CNormalItemPower*, compareItemPowers >::iterator it)
{
  delete(*it);
  mItemPowers.erase(it);
  return true;
}

/**
 * Remove an itempower from this product, provided it is a factor of it.
 * -used in multiplication with lcm-
 * @return true.
 */
bool CNormalProduct::remove(const CNormalItemPower& itemPower)
{
  std::set <CNormalItemPower*, compareItemPowers >::iterator it;
  std::set <CNormalItemPower*, compareItemPowers >::iterator itEnd = mItemPowers.end();
  for (it = mItemPowers.begin(); it != itEnd; ++it)
    {
      if ((*it)->getItem().areEqual(itemPower.getItem()))
        {
          C_FLOAT64 dif = (*it)->getExp() - itemPower.getExp();
          if (dif >= 1.0E-100)
            {
              (*it)->setExp(dif);
              return true;
            }
          if (fabs(dif) < 1.0E-100)
            {
              delete(*it);
              mItemPowers.erase(it);
              return true;
            }
          return false;
        }
    }
  return false;
}

/**
 * Multiply this product with another product.
 * @return true
 */
bool CNormalProduct::multiply(const CNormalProduct& product)
{
  multiply(product.getFactor());
  if (fabs(mFactor) < 1.0E-100)
    return true;
  multiply(product.getItemPowers());
  return true;
}

/**
 * Multiply this product with a sum NOT CONTAINING FRACTIONS!.
 * @return sum.
 */
CNormalSum* CNormalProduct::multiply(const CNormalSum& sum)
{
  if (fabs(mFactor) < 1.0E-100)
    {
      CNormalSum* zeroSum = new CNormalSum();
      zeroSum->add(*this);
      return zeroSum;
    }
  CNormalSum* tmp = new CNormalSum(sum);
  CNormalSum* newsum = new CNormalSum();
  std::set<CNormalProduct*>::const_iterator it;
  std::set<CNormalProduct*>::const_iterator itEnd = tmp->getProducts().end();
  for (it = tmp->getProducts().begin(); it != itEnd; ++it)
    {
      (*it)->multiply(*this);
      newsum->add(**it);
    }
  delete tmp;
  return newsum;
}

/**
 * Multiply this product with a lcm.
 * @return true
 */
const CNormalSum* CNormalProduct::multiply(const CNormalLcm& lcm)
{
  if (fabs(mFactor) < 1.0E-100)
    {
      CNormalSum* zeroSum = new CNormalSum();
      zeroSum->add(*this);
      return zeroSum;
    }
  CNormalLcm* tmp = new CNormalLcm(lcm);
  multiply(tmp->getItemPowers());
  CNormalSum* sum = new CNormalSum();
  sum->add(*this);
  std::vector<CNormalSum*>::const_iterator it;
  std::vector<CNormalSum*>::const_iterator itEnd = tmp->getSums().end();
  for (it = tmp->getSums().begin(); it != itEnd; ++it)
    {
      sum->multiply(**it);
    }
  delete tmp;
  return sum;
}

/**
 * Retrieve the factor of this product.
 * @return mFactor.
 */
const C_FLOAT64 & CNormalProduct::getFactor() const
  {
    return mFactor;
  }

/**
 * Retrieve the set of itempowers of this product.
 * @return mItemPowers.
 */
const std::set <CNormalItemPower*, compareItemPowers >& CNormalProduct::getItemPowers() const
  {
    return mItemPowers;
  }

bool CNormalProduct::checkSamePowerList(const CNormalProduct & rhs) const
  {
    if (mItemPowers.size() != rhs.mItemPowers.size())
      return false;
    std::set<CNormalItemPower*>::const_iterator it;
    std::set<CNormalItemPower*>::const_iterator itEnd = mItemPowers.end();
    std::set<CNormalItemPower*>::const_iterator it2;
    std::set<CNormalItemPower*>::const_iterator it2End = rhs.mItemPowers.end();
    for (it = mItemPowers.begin(), it2 = rhs.mItemPowers.begin(); it != itEnd; ++it, ++it2)
      {
        if (!(**it == **it2))
          return false;
      }
    return true;
  }

bool CNormalProduct::operator==(const CNormalProduct & rhs) const
  {
    if (mFactor != rhs.mFactor)
      return false;
    if (checkSamePowerList(rhs))
      return true;
    return false;
  }

std::ostream & operator<< (std::ostream &os, const CNormalProduct & d)
{
  os << d.toString();
  return os;
}

std::string CNormalProduct::toString() const
  {
    std::ostringstream os;
    bool firstFactor = true;
    if ((fabs(this->mFactor - 1.0) >= 1.0E-100) || (this->mItemPowers.size() == 0))
      {
        if (this->mFactor < 0.0)
          os << "(" << this->mFactor << ")";
        else
          os << this->mFactor;
        firstFactor = false;
      }
    std::set <CNormalItemPower*, compareItemPowers >::const_iterator it;
    std::set <CNormalItemPower*, compareItemPowers >::const_iterator itEnd = this->mItemPowers.end();
    for (it = this->mItemPowers.begin(); it != itEnd; ++it)
      {
        if (firstFactor == false)
          os << " * ";
        os << **it;
        firstFactor = false;
      }
    return os.str();
  }

CNormalProduct* CNormalProduct::copy() const
  {
    return new CNormalProduct(*this);
  }

bool CNormalProduct::operator<(const CNormalProduct& rhs) const
  {
    bool result = false;
    if (this->mFactor < rhs.mFactor)
      {
        result = true;
      }
    else if (this->mFactor == rhs.mFactor)
      {
        std::set<CNormalItemPower*, compareItemPowers >::const_iterator it = this->mItemPowers.begin(), endit = this->mItemPowers.end();
        std::set<CNormalItemPower*, compareItemPowers >::const_iterator it2 = rhs.mItemPowers.begin();
        compareItemPowers comp;
        while (result == false && it != endit)
          {
            if (comp(*it, *it2))
              {
                result = true;
              }
            if ((**it) == (**it2))
              {
                break;
              }
            ++it;
            ++it2;
          }
      }
    return result;
  }