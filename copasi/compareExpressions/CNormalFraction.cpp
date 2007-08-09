// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/compareExpressions/CNormalFraction.cpp,v $
//   $Revision: 1.1 $
//   $Name:  $
//   $Author: gauges $
//   $Date: 2007/08/08 10:27:29 $
// End CVS Header

// Copyright (C) 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#include <vector>
#include <math.h>
#include <sstream>
#include "copasi.h"

#include "CNormalProduct.h"
#include "CNormalSum.h"
#include "CNormalLcm.h"
#include "CNormalItemPower.h"
#include "CNormalFraction.h"

/**
 * Default constructor
 */
CNormalFraction::CNormalFraction(): CNormalBase(), mpNumerator(new CNormalSum()), mpDenominator(new CNormalSum()){}

/**
 * Copy contructor
 */
CNormalFraction::CNormalFraction(const CNormalFraction& src): CNormalBase(src),
    mpNumerator(new CNormalSum(*src.mpNumerator)),
mpDenominator(new CNormalSum(*src.mpDenominator)){}

/**
 * Assignment operator
 */
CNormalFraction& CNormalFraction::operator=(const CNormalFraction& src)
{
  if (this->mpNumerator != NULL) delete this->mpNumerator;
  if (this->mpDenominator != NULL) delete this->mpDenominator;
  this->mpNumerator = new CNormalSum(*src.mpNumerator);
  this->mpDenominator = new CNormalSum(*src.mpDenominator);
  return *this;
}

CNormalFraction::~CNormalFraction()
{
  if (mpNumerator != NULL) delete mpNumerator;
  if (mpDenominator != NULL) delete mpDenominator;
}

/**
 * Set numerator of this fraction
 * @return true.
 */
bool CNormalFraction::setNumerator(const CNormalSum& numerator)
{
  if (mpNumerator != NULL)
    {
      delete mpNumerator;
    }
  mpNumerator = new CNormalSum(numerator);
  return true;
}

/**
 * Set denominator of this fraction.
 * @return true.
 */
bool CNormalFraction::setDenominator(const CNormalSum& denominator)
{
  if (mpDenominator != NULL)
    {
      delete mpDenominator;
    }
  mpDenominator = new CNormalSum(denominator);
  if (mpDenominator->getSize() == 0)
    return false;
  return true;
}

/**
 * Check if the denominator of this fraction equals 1.
 * @return bool
 */
bool CNormalFraction::checkDenominatorOne() const
  {
    if ((mpDenominator->getProducts().size() == 1)
        && (mpDenominator->getFractions().size() == 0)
        && ((*mpDenominator->getProducts().begin())->getItemPowers().size() == 0)
        && (fabs((*mpDenominator->getProducts().begin())->getFactor() - 1.0) < 1.E-100)
)
      {
        return true;
      }
    return false;
  }

/**
 * Set the denominator of this fraction equal 1.
 * @return true.
 */
bool CNormalFraction::setDenominatorOne()
{
  CNormalProduct* product = new CNormalProduct();
  CNormalSum* sum = new CNormalSum();
  sum->add(*product);
  delete product;
  setDenominator(*sum);
  delete sum;
  return true;
}

/**
 * Retrieve the numerator of this fraction.
 * @return mpNumerator.
 */
const CNormalSum& CNormalFraction::getNumerator() const
  {
    return *mpNumerator;
  }

/**
 * Retrieve the denominator of this fraction.
 * @return mpDenominator.
 */
const CNormalSum& CNormalFraction::getDenominator() const
  {
    return *mpDenominator;
  }

/**
 * Check if this fraction contains further fractions.
 * @return bool.
 */
bool CNormalFraction::checkForFractions() const
  {
    if (mpNumerator->getFractions().size() + mpDenominator->getFractions().size() == 0)
      return false;
    return true;
  }

/**
 * Expand this fraction by a lcm.
 * @return true.
 */
bool CNormalFraction::expand(const CNormalLcm& lcm)
{
  mpNumerator->multiply(lcm);
  mpDenominator->multiply(lcm);
  if (mpDenominator->getSize() == 0)
    return false;
  return true;
}

/**
 * Cancel common items of mpNumerator and mpDenominator.
 * @return true.
 */
bool CNormalFraction::cancel()
{
  if (*mpDenominator == *mpNumerator)
    {
      setDenominatorOne();
      setNumerator(*mpDenominator);
      return true;
    }

  if (mpDenominator->getProducts().size() != 0)
    {
      C_FLOAT64 factor = (*mpDenominator->getProducts().begin())->getFactor();
      if (fabs(factor) < 1.0E-100)
        return false;
      else
        {
          mpNumerator->multiply(1.0 / factor);  //factor != 0 as checked earlier
          mpDenominator->multiply(1.0 / factor);
        }
    }

  if (checkForFractions() == false)
    {
      std::set<CNormalItemPower*, compareItemPowers >::iterator it = (*mpDenominator->getProducts().begin())->getItemPowers().begin();
      std::set<CNormalItemPower*, compareItemPowers >::iterator itEnd = (*mpDenominator->getProducts().begin())->getItemPowers().end();
      std::vector<CNormalItemPower*> tmpV;
      while (it != itEnd)
        {//runs through all item powers in the first product of the denominator
          C_FLOAT64 exp = mpNumerator->checkFactor(**it);
          if (fabs(exp) >= 1.0E-100)
            {
              exp = mpDenominator->checkFactor(**it) < exp ? mpDenominator->checkFactor(**it) : exp;
              if (fabs(exp) >= 1.0E-100)
                {
                  CNormalItemPower* itemPower = new CNormalItemPower((*it)->getItem(), exp);
                  tmpV.push_back(itemPower);
                }
            }
          ++it;
        }
      std::vector<CNormalItemPower*>::iterator it2 = tmpV.begin();
      std::vector<CNormalItemPower*>::iterator itEnd2 = tmpV.end();
      while (it2 != itEnd2)
        {
          mpNumerator->divide(**it2);
          mpDenominator->divide(**it2);
          delete *it2;
          ++it2;
        }
    }
  return true;
}

/**
 * Multiply this fraction with a number.
 * @return true.
 */
bool CNormalFraction::multiply(const C_FLOAT64& number)
{
  mpNumerator->multiply(number);
  if (mpNumerator->getSize() == 0)
    setDenominatorOne();
  return true;
}

/**
 * Multiply a fraction with an itempower.
 * @return true.
 */
bool CNormalFraction::multiply(const CNormalItemPower& itemPower)
{
  C_FLOAT64 exp = mpDenominator->checkFactor(itemPower);
  if (fabs(exp) >= 1.0E-100)
    {
      CNormalItemPower* tmp1 = new CNormalItemPower(itemPower.getItem(), exp);
      mpDenominator->divide(*tmp1);
      if (fabs(itemPower.getExp() - exp) >= 1.0E-100)
        {
          CNormalItemPower* tmp2 = new CNormalItemPower(itemPower.getItem(), fabs(itemPower.getExp() - exp));
          mpNumerator->multiply(*tmp2);
          return true;
        }
      return true;
    }
  else
    mpNumerator->multiply(itemPower);
  return true;
}

/**
 * Find lcm of denominators of respective fractions contained in this fraction.
 * @return pointer to lcm.
 */
const CNormalLcm* CNormalFraction::findLcm() const
  {
    CNormalLcm* lcm = new CNormalLcm();

    std::set<CNormalFraction*>::const_iterator it;
    std::set<CNormalFraction*>::const_iterator itEnd = mpNumerator->getFractions().end();
    for (it = mpNumerator->getFractions().begin(); it != itEnd; ++it)
      {
        lcm->add((*it)->getDenominator());
      }

    std::set<CNormalFraction*>::const_iterator it2;
    std::set<CNormalFraction*>::const_iterator it2End = mpDenominator->getFractions().end();
    for (it2 = mpDenominator->getFractions().begin(); it2 != it2End; ++it2)
      {
        lcm->add((*it2)->getDenominator());
      }

    return lcm;
  }

/**
 * Multiply this fraction by a lcm.
 * @return CNormalSum*, pointer to result of multiplication ie. a normal sum.
 * This fraction is useless now.
 */
const CNormalSum* CNormalFraction::multiply(CNormalLcm lcm)
{
  if (mpDenominator->getFractions().size() != 0)
    return false;
  if (lcm.remove(*mpDenominator) == false)
    return false;
  mpNumerator->multiply(lcm);
  CNormalSum * sum = new CNormalSum(*mpNumerator);
  return sum;
}

/**
 * Check if numerator and denominator contain fractions,
 * find lcm of respective denominators, expand this fraction by lcm and cancel.
 * @return true
 */
bool CNormalFraction::simplify()
{
  if (mpNumerator->getFractions().size() + mpDenominator->getFractions().size() > 0)
    {
      std::set<CNormalFraction*>::const_iterator it;
      std::set<CNormalFraction*>::const_iterator itEnd = mpNumerator->getFractions().end();
      for (it = mpNumerator->getFractions().begin(); it != itEnd; ++it)
        {
          (*it)->simplify();
        }
      std::set<CNormalFraction*>::const_iterator it2;
      std::set<CNormalFraction*>::const_iterator it2End = mpDenominator->getFractions().end();
      for (it2 = mpDenominator->getFractions().begin(); it2 != it2End; ++it2)
        {
          (*it2)->simplify();
        }
      const CNormalLcm* lcm = findLcm();
      expand(*lcm);
      delete lcm;
    }
  cancel();
  return true;
}

bool CNormalFraction::operator==(const CNormalFraction & rhs) const
  {
    if ((*mpNumerator == *rhs.mpNumerator) && (*mpDenominator == *rhs.mpDenominator))
      return true;
    else
      return false;
  }

std::ostream & operator<< (std::ostream &os, const CNormalFraction & d)
{
  os << d.toString();
  return os;
}

std::string CNormalFraction::toString() const
  {
    std::ostringstream os;
    if (this->checkDenominatorOne() == true)
      os << *this->mpNumerator;
    else
      os << "(" << *this->mpNumerator << ")/(" << *this->mpDenominator << ")";
    return os.str();
  }

CNormalFraction* CNormalFraction::copy() const
  {
    return new CNormalFraction(*this);
  }

bool CNormalFraction::operator<(const CNormalFraction& src) const
  {
    bool result = false;
    if ((*this->mpNumerator) < (*src.mpNumerator))
      {
        result = true;
      }
    else if ((*this->mpNumerator) == (*src.mpNumerator))
      {
        if ((*this->mpDenominator) < (*src.mpDenominator))
          {
            result = true;
          }
      }
    return result;
  }