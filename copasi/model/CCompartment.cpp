// Copyright (C) 2010 - 2013 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// Copyright (C) 2008 - 2009 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

// Copyright (C) 2001 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

// CCompartment
//
// Derived from Gepasi's ccompart.cpp
// (C) Pedro Mendes 1995-2000
//
// Converted for COPASI by Pedro Mendes

#include <iostream>
#include <cmath>

#include "copasi.h"
#include "utilities/CReadConfig.h"
#include "utilities/CCopasiVector.h"
#include "utilities/utility.h"
#include "report/CCopasiObjectReference.h"
#include "report/CKeyFactory.h"
#include "report/CRenameHandler.h"
#include "CCompartment.h"
#include "CModel.h"
#include "copasi/report/CCopasiRootContainer.h"

CCompartment::CCompartment(const std::string & name,
                           const CCopasiContainer * pParent):
  CModelEntity(name, pParent, "Compartment"),
  mMetabolites("Metabolites", this),
  mDimensionality(3)
{
  initObjects();

  mKey = CCopasiRootContainer::getKeyFactory()->add("Compartment", this);

  setStatus(FIXED);

  *mpIValue = 1.0;

  CONSTRUCTOR_TRACE;
}

CCompartment::CCompartment(const CCompartment & src,
                           const CCopasiContainer * pParent):
  CModelEntity(src, pParent),
  mMetabolites(src.mMetabolites, this),
  mDimensionality(src.mDimensionality)
{
  mKey = CCopasiRootContainer::getKeyFactory()->add("Compartment", this);
  CONSTRUCTOR_TRACE;
  initObjects();
}

CCompartment::~CCompartment()
{
  CCopasiRootContainer::getKeyFactory()->remove(mKey);
  DESTRUCTOR_TRACE;
}

// virtual
std::string CCompartment::getChildObjectUnits(const CCopasiObject * pObject) const
{
  if (mpModel == NULL) return "";

  if (pObject == mpValueReference ||
      pObject == mpIValueReference)
    {
      switch (mDimensionality)
        {
          case 1:
            return mpModel->getLengthUnitsDisplayString();
            break;

          case 2:
            return mpModel->getAreaUnitsDisplayString();
            break;

          case 3:
            return mpModel->getVolumeUnitsDisplayString();
            break;

          default:
            return "";
            break;
        }
    }
  else if (pObject == mpRateReference)
    {
      std::string Unit = getChildObjectUnits(mpValueReference);
      std::string TimeUnit = mpModel->getTimeUnitsDisplayString();

      if (Unit == "")
        {
          if (TimeUnit == "")
            {
              return "";
            }

          return "1/" + TimeUnit;
        }

      if (TimeUnit == "")
        {
          return Unit;
        }

      return Unit + "/" + TimeUnit;
    }

  return "";
}

void CCompartment::cleanup() {mMetabolites.cleanup();}

std::set< const CCopasiObject * > CCompartment::getDeletedObjects() const
{
  std::set< const CCopasiObject * > Deleted = CModelEntity::getDeletedObjects();

  // We must not add the metabolites as CModel::appendDependentMetabolites does that.

  return Deleted;
}

C_INT32 CCompartment::load(CReadConfig & configbuffer)
{
  C_INT32 Fail = 0;
  std::string tmp;

  if ((Fail = configbuffer.getVariable("Compartment", "string",
                                       (void *) & tmp,
                                       CReadConfig::SEARCH)))
    return Fail;

  setObjectName(tmp);

  C_FLOAT64 tmpdbl;

  if ((Fail = configbuffer.getVariable("Volume", "C_FLOAT64",
                                       (void *) & tmpdbl)))
    return Fail;

  setInitialValue(tmpdbl);

  return Fail;
}

CCopasiVectorNS < CMetab > & CCompartment::getMetabolites()
{return mMetabolites;}

const CCopasiVectorNS < CMetab > & CCompartment::getMetabolites() const
{return mMetabolites;}

/* Note: the metabolite stored in mMetabolites has definitely mpCompartment set.
   In the case the compartment is part of a model also mpModel is set. */
bool CCompartment::createMetabolite(const CMetab & metabolite)
{
  CMetab * pMetab = new CMetab(metabolite);

  if (addMetabolite(pMetab)) return true;

  delete pMetab;
  return false;
}

bool CCompartment::addMetabolite(CMetab * pMetabolite)
{
  if (!pMetabolite) return false;

  std::string oldCN = pMetabolite->getCN();

  bool success = mMetabolites.add(pMetabolite, true);

  //if a metabolite is added to a compartment successfully the CN of
  //the metabolite is changed. This needs to be handled similarly to a
  //rename.
  if (success && smpRenameHandler && getObjectParent())
    {
      std::string newCN = pMetabolite->getCN();
      smpRenameHandler->handle(oldCN, newCN);
    }

  return success;
}

bool CCompartment::setDimensionality(unsigned C_INT32 dim)
{
  if (dim > 3)
    return false;

  mDimensionality = dim;
  return true;
}

unsigned C_INT32 CCompartment::getDimensionality() const
{
  return mDimensionality;
}

void CCompartment::initObjects()
{
  mpIValueReference->setObjectName("InitialVolume");
  mpValueReference->setObjectName("Volume");
}

std::ostream & operator<<(std::ostream &os, const CCompartment & d)
{
  os << "++++CCompartment: " << d.getObjectName() << " mValue " << *d.mpValue;
  os << "    CCompartment.mMetabolites " << std::endl << d.mMetabolites;
  os << "----CCompartment " << std::endl;

  return os;
}
