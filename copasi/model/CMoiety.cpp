#include <stdio.h>
#include <math.h>

#define  COPASI_TRACE_CONSTRUCTION

#include "copasi.h"
#include "utilities/CCopasiMessage.h"
#include "CMoiety.h"
#include "CCompartment.h"
#include "utilities/readwrite.h"
#include "utilities/CCopasiVector.h"
#include "utilities/utility.h"

CMoiety::CMoiety(const std::string & name,
                 const CCopasiContainer * pParent):
    CCopasiContainer(name, pParent, "Moiety"),
    mName(mObjectName),
    mNumber(0),
    mINumber(0),
    mEquation("Equation", this)
{CONSTRUCTOR_TRACE;}

CMoiety::CMoiety(const CMoiety & src,
                 const CCopasiContainer * pParent):
    CCopasiContainer(src, pParent),
    mName(mObjectName),
    mNumber(src.mNumber),
    mINumber(src.mINumber),
    mEquation(src.mEquation, this)
{CONSTRUCTOR_TRACE;}

CMoiety::~CMoiety() {DESTRUCTOR_TRACE;}

void CMoiety::add(C_FLOAT64 value, CMetab & metabolite)
{
  CChemEqElement element;
  element.setMultiplicity(value);
  element.setMetabolite(metabolite);

  mEquation.add(element);
}
//TODO do we really need both add methods?
void CMoiety::add(C_FLOAT64 value, CMetab * metabolite)
{add(value, *metabolite);}

void CMoiety::cleanup() {mEquation.cleanup();}

C_FLOAT64 CMoiety::dependentNumber()
{
  mNumber = mINumber;

  for (unsigned C_INT32 i = 1; i < mEquation.size(); i++)
    mNumber -= mEquation[i]->getMultiplicity() *
               mEquation[i]->getMetabolite().getNumberDbl();

  return mNumber;
}

C_FLOAT64 CMoiety::dependentRate()
{
  C_FLOAT64 Rate = 0.0;

  for (unsigned C_INT32 i = 1; i < mEquation.size(); i++)
    Rate -= mEquation[i]->getMultiplicity() *
            mEquation[i]->getMetabolite().getRate() *
            mEquation[i]->getMetabolite().getCompartment()->getVolumeInv();

  return Rate * mEquation[0]->getMetabolite().getCompartment()->getVolume();
}

const std::string & CMoiety::getName() const {return mName;}

std::string CMoiety::getDescription() const
  {
    std::string Description;
    for (unsigned C_INT32 i = 0; i < mEquation.size(); i++)
      {
        if (i)
          {
            if (mEquation[i]->getMultiplicity() < 0.0)
              Description += " - ";
            else
              Description += " + ";
          }
        if (fabs(mEquation[i]->getMultiplicity()) != 1.0)
          Description += StringPrint("%3.1f * ",
                                     fabs(mEquation[i]->getMultiplicity()));
        Description += mEquation[i]->getMetaboliteName();
        //Description += "{" + mEquation[i]->getCompartmentName() + "}";
        Description += "{" + mEquation[i]->getMetabolite().getCompartment()->getName() + "}";
      }
    return Description;
  }

void CMoiety::setName(const std::string name)
{
  mName = name;
}

void CMoiety::setInitialValue()
{
  mINumber = 0;

  for (unsigned C_INT32 i = 0; i < mEquation.size(); i++)
    mINumber += mEquation[i]->getMultiplicity() *
                mEquation[i]->getMetabolite().getInitialNumberDbl();
  return;
}

/**
 * Return the number value Wei Sun
 */
C_FLOAT64 CMoiety::getNumber() const
  {
    return mINumber;
  }

/**
 * Returns the address of mNumber
 */
void * CMoiety::getNumberAddr()
{
  return &mINumber;
}

/**
 * Saves in Gepasi 3.21 format
 */
C_INT32 CMoiety::saveOld(CWriteConfig & configBuffer)
{
  C_INT32 c = 0, t = 7, Fail = 0;

  Fail = configBuffer.setVariable("Metabolite", "string", (void *) & mEquation);
  if (Fail)
    return Fail;
  // we write mNumber instead of concentration, which is ok because Gepasi recalculates this itself
  Fail = configBuffer.setVariable("Concentration", "C_FLOAT64", (void *) & mNumber);
  if (Fail)
    return Fail;
  Fail = configBuffer.setVariable("Compartment", "C_INT32", (void *) & c);
  if (Fail)
    return Fail;
  Fail = configBuffer.setVariable("Type", "C_INT32", (void *) & t);
  return Fail;
}
