// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/model/CObjectLists.cpp,v $
//   $Revision: 1.15 $
//   $Name:  $
//   $Author: shoops $
//   $Date: 2007/07/24 18:40:23 $
// End CVS Header

// Copyright (C) 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#include "copasi.h"

#include "CObjectLists.h"
#include "report/CCopasiObject.h"
#include "model/CModel.h"
#include "report/CKeyFactory.h"

//static
const std::string CObjectLists::ListTypeName[] =
  {
    "Not Set",
    "Single Object",

    "Metabolites",
    "Non-Constant Metabolites",
    "Concentrations of Metabolites",
    "Non-Constant Concentrations of Metabolites",
    "Numbers of Metabolites",
    "Non-Constant Numbers of Metabolites",
    "Non-Constant Concentration Rates",
    "Non-Constant Particle Rates",
    "Initial Concentrations",
    "Initial Numbers",

    "Reactions",
    "Concentration Fluxes of Reactions",
    "Particle Fluxes of Reactions",

    "Global Quantity",
    "Global Quantity Initial Values",
    "Global Quantity Values",
    "Non-Constant Global Quantity Values",
    "Constant Global Quantity Values",
    "Values of Global Quantities with ODE",
    "Values of Global Quantities with Assignment",
    "Global Quantity Rates",

    "Compartments",
    "Compartment Volumes",
    "Compartment Initial Volumes",
    "Compartment Volume Rates",

    "All initial Values",
    "Local Parameter Values",
    "All Parameter Values",
    "All Parameter and Initial Values",

    "All Variables of the model",
    "All independent Variables of the model",

    ""
  };

//static
std::vector<const CCopasiObject*>
CObjectLists::getListOfConstObjects(ListType t, const CModel* model)
{
  std::vector<CCopasiObject*> tmp = getListOfObjects(t, model);
  return std::vector<const CCopasiObject*>(tmp.begin(), tmp.end());

  //I know this is unnecessary copying. But I want to avoid reinterpret_cast
}

//static
std::vector<CCopasiObject*>
CObjectLists::getListOfObjects(ListType t, const CModel* model)
{
  ObjectList ret;

  const CCopasiVector< CMetab > & metabs = model->getMetabolites();
  unsigned C_INT32 i, imax = metabs.size();

  const CCopasiVectorN<CReaction> & reacs = model->getReactions();
  unsigned C_INT32 reacMax = reacs.size();

  switch (t)
    {
    case EMPTY_LIST:
    case SINGLE_OBJECT:
      break;

    case ALL_METABS:
      for (i = 0; i < imax; ++i)
        ret.push_back(metabs[i]);
      break;

    case NON_CONST_METABS:
      for (i = 0; i < imax; ++i)
        if (metabs[i]->getStatus() != CModelEntity::FIXED)
          ret.push_back(metabs[i]);
      break;

    case ALL_METAB_CONCENTRATIONS:
      for (i = 0; i < imax; ++i)
        ret.push_back(const_cast<CCopasiObject*>
                      (metabs[i]->getObject(CCopasiObjectName("Reference=Concentration"))));
      break;

    case NON_CONST_METAB_CONCENTRATIONS:
      for (i = 0; i < imax; ++i)
        if (metabs[i]->getStatus() != CModelEntity::FIXED)
          ret.push_back(const_cast<CCopasiObject*>
                        (metabs[i]->getObject(CCopasiObjectName("Reference=Concentration"))));
      break;

    case ALL_METAB_NUMBERS:
      for (i = 0; i < imax; ++i)
        ret.push_back(const_cast<CCopasiObject*>
                      (metabs[i]->getObject(CCopasiObjectName("Reference=ParticleNumber"))));
      break;

    case NON_CONST_METAB_NUMBERS:
      for (i = 0; i < imax; ++i)
        if (metabs[i]->getStatus() != CModelEntity::FIXED)
          ret.push_back(const_cast<CCopasiObject*>
                        (metabs[i]->getObject(CCopasiObjectName("Reference=ParticleNumber"))));
      break;

    case NON_CONST_METAB_CONC_RATES:
      for (i = 0; i < imax; ++i)
        if (metabs[i]->getStatus() != CModelEntity::FIXED)
          ret.push_back(const_cast<CCopasiObject*>
                        (metabs[i]->getObject(CCopasiObjectName("Reference=Rate"))));
      break; //not possible at the moment

    case NON_CONST_METAB_PART_RATES:
      for (i = 0; i < imax; ++i)
        if (metabs[i]->getStatus() != CModelEntity::FIXED)
          ret.push_back(const_cast<CCopasiObject*>
                        (metabs[i]->getObject(CCopasiObjectName("Reference=ParticleNumberRate"))));
      break;

    case ALL_METAB_INITIAL_CONCENTRATIONS:
      for (i = 0; i < imax; ++i)
        ret.push_back(const_cast<CCopasiObject*>
                      (metabs[i]->getObject(CCopasiObjectName("Reference=InitialConcentration"))));
      break;

    case ALL_METAB_INITIAL_NUMBERS:
      for (i = 0; i < imax; ++i)
        ret.push_back(const_cast<CCopasiObject*>
                      (metabs[i]->getObject(CCopasiObjectName("Reference=InitialParticleNumber"))));
      break;

      // reactions

    case REACTIONS:
      for (i = 0; i < reacMax; ++i)
        ret.push_back(reacs[i]);
      break;

    case REACTION_CONC_FLUXES:
      for (i = 0; i < reacMax; ++i)
        ret.push_back(const_cast<CCopasiObject*>
                      (reacs[i]->getObject(CCopasiObjectName("Reference=Flux"))));
      break;

    case REACTION_PART_FLUXES:
      for (i = 0; i < reacMax; ++i)
        ret.push_back(const_cast<CCopasiObject*>
                      (reacs[i]->getObject(CCopasiObjectName("Reference=ParticleFlux"))));
      break;

      // global parameters

    case GLOBAL_PARAMETERS:
      {
        const CCopasiVectorN< CModelValue > & params = model->getModelValues();
        for (i = 0; i < params.size(); ++i)
          ret.push_back(params[i]);
      }
      break;

    case GLOBAL_PARAMETER_INITIAL_VALUES:
      {
        const CCopasiVectorN< CModelValue > & params = model->getModelValues();
        for (i = 0; i < params.size(); ++i)
          if (params[i]->getStatus() != CModelEntity::ASSIGNMENT)
            ret.push_back(const_cast<CCopasiObject*>
                          (params[i]->getObject(CCopasiObjectName("Reference=InitialValue"))));
      }
      break;

    case GLOBAL_PARAMETER_VALUES:
      {
        const CCopasiVectorN< CModelValue > & params = model->getModelValues();
        for (i = 0; i < params.size(); ++i)
          ret.push_back(const_cast<CCopasiObject*>
                        (params[i]->getObject(CCopasiObjectName("Reference=Value"))));
      }
      break;

    case NON_CONST_GLOBAL_PARAMETER_VALUES:
      {
        const CCopasiVectorN< CModelValue > & params = model->getModelValues();
        for (i = 0; i < params.size(); ++i)
          if (params[i]->getStatus() != CModelEntity::FIXED
              && !params[i]->isUsedOnce())
            ret.push_back(const_cast<CCopasiObject*>
                          (params[i]->getObject(CCopasiObjectName("Reference=Value"))));
      }
      break;

    case CONST_GLOBAL_PARAMETER_VALUES:
      {
        const CCopasiVectorN< CModelValue > & params = model->getModelValues();
        for (i = 0; i < params.size(); ++i)
          if (params[i]->getStatus() == CModelEntity::FIXED
              || params[i]->isUsedOnce())
            ret.push_back(const_cast<CCopasiObject*>
                          (params[i]->getObject(CCopasiObjectName("Reference=Value"))));
      }
      break;

    case ODE_GLOBAL_PARAMETER_VALUES:
      {
        const CCopasiVectorN< CModelValue > & params = model->getModelValues();
        for (i = 0; i < params.size(); ++i)
          if (params[i]->getStatus() == CModelEntity::ODE)
            ret.push_back(const_cast<CCopasiObject*>
                          (params[i]->getObject(CCopasiObjectName("Reference=Value"))));
      }
      break;

    case ASS_GLOBAL_PARAMETER_VALUES:
      {
        const CCopasiVectorN< CModelValue > & params = model->getModelValues();
        for (i = 0; i < params.size(); ++i)
          if (params[i]->getStatus() == CModelEntity::ASSIGNMENT)
            ret.push_back(const_cast<CCopasiObject*>
                          (params[i]->getObject(CCopasiObjectName("Reference=Value"))));
      }
      break;

    case GLOBAL_PARAMETER_RATES:
      {
        const CCopasiVectorN< CModelValue > & params = model->getModelValues();
        for (i = 0; i < params.size(); ++i)
          if (params[i]->getStatus() == CModelEntity::ODE)
            ret.push_back(const_cast<CCopasiObject*>
                          (params[i]->getObject(CCopasiObjectName("Reference=Rate"))));
      }
      break;

      // compartments

    case COMPARTMENTS:
    case COMPARTMENT_VOLUMES:
    case COMPARTMENT_INITIAL_VOLUMES:
    case COMPARTMENT_RATES:
      break;

    case ALL_INITIAL_VALUES:
      {
        ret = getListOfObjects(ALL_METAB_INITIAL_CONCENTRATIONS, model);
        ObjectList l2 = getListOfObjects(GLOBAL_PARAMETER_INITIAL_VALUES, model);

        ObjectList::const_iterator it;
        for (it = l2.begin(); it != l2.end(); ++it) ret.push_back(*it);

        //TODO: Compartments
      }
      break;

    case ALL_LOCAL_PARAMETER_VALUES:
      {
        for (i = 0; i < reacMax; ++i)
          {
            const CFunctionParameters & params = reacs[i]->getFunctionParameters();
            unsigned C_INT32 j, jmax = params.size();
            for (j = 0; j < jmax; ++j)
              if (params[j]->getUsage() == CFunctionParameter::PARAMETER)
                if (reacs[i]->isLocalParameter(j))
                  {
                    CCopasiParameter * par =
                      dynamic_cast<CCopasiParameter*>
                      (GlobalKeys.get(reacs[i]->getParameterMappings()[j][0]));
                    if (par)
                      ret.push_back(const_cast<CCopasiObject*>
                                    (par->getObject(CCopasiObjectName("Reference=Value"))));
                  }
          }
      }
      break;

    case ALL_PARAMETER_VALUES:
      {
        ret = getListOfObjects(CONST_GLOBAL_PARAMETER_VALUES, model);
        ObjectList l2 = getListOfObjects(ALL_LOCAL_PARAMETER_VALUES, model);

        ObjectList::const_iterator it;
        for (it = l2.begin(); it != l2.end(); ++it) ret.push_back(*it);
      }
      break;

    case ALL_PARAMETER_AND_INITIAL_VALUES:
      {
        ret = getListOfObjects(ALL_PARAMETER_VALUES, model);
        ObjectList l2 = getListOfObjects(ALL_INITIAL_VALUES, model);

        ObjectList::const_iterator it;
        for (it = l2.begin(); it != l2.end(); ++it) ret.push_back(*it);
      }
      break;

    case ALL_VARIABLES:
      {
        ret = getListOfObjects(NON_CONST_METAB_CONCENTRATIONS, model);
        ObjectList l2 = getListOfObjects(NON_CONST_GLOBAL_PARAMETER_VALUES, model);

        ObjectList::const_iterator it;
        for (it = l2.begin(); it != l2.end(); ++it) ret.push_back(*it);
        //TODO extend to compartments
      }
      break;

    default:
      break;
    }

  //debug output

#ifdef COPASI_DEBUG
  std::vector<CCopasiObject*>::const_iterator it, itEnd = ret.end();
  for (it = ret.begin(); it != itEnd; ++it)
    std::cout << (*it)->getObjectDisplayName() << std::endl;
#endif // COPASI_DEBUG

  return ret;
}

//static
bool CObjectLists::existsFixedMetab(const CModel* model)
{
  bool fixedMetab = false;
  const CCopasiVector< CMetab > & metabs = model->getMetabolites();
  C_INT32 i, imax = metabs.size();
  for (i = 0; i < imax; ++i)
    if (metabs[i]->getStatus() == CModelEntity::FIXED)
    {fixedMetab = true; break;}
  return fixedMetab;
}
