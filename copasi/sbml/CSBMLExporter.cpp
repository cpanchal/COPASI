// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/sbml/CSBMLExporter.cpp,v $
//   $Revision: 1.5 $
//   $Name:  $
//   $Author: gauges $
//   $Date: 2007/11/30 19:51:36 $
// End CVS Header

// Copyright (C) 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#include "CSBMLExporter.h"

#include "SBMLImporter.h"
#include "sbml/SBMLDocument.h"
#include "sbml/Compartment.h"
#include "sbml/Model.h"
#include "sbml/Species.h"
#include "sbml/Parameter.h"
#include "sbml/Reaction.h"
#include "sbml/KineticLaw.h"
#include "sbml/SBMLWriter.h"
#include "sbml/SpeciesReference.h"
#include "sbml/math/ASTNode.h"
#include "CopasiDataModel/CCopasiDataModel.h"
#include "SBMLIncompatibility.h"
#include "model/CCompartment.h"
#include "model/CModel.h"
#include "model/CEvent.h"
#include "model/CMetab.h"
#include "function/CExpression.h"
#include "function/CEvaluationNode.h"
#include "model/CReaction.h"
#include "utilities/CCopasiParameter.h"
#include "model/CModelValue.h"
#include "function/CFunction.h"
#include "report/CKeyFactory.h"
#include "ConverterASTNode.h"
#include "utilities/CCopasiTree.h"
#include "model/CChemEqElement.h"
#include "utilities/CVersion.h"

// TODO maybe replace checkForUnsupportedFunctionCalls with
// TODO checkForUnsupportedNodes
// TODO This will check for unsupported function calls, unsupported constant nodes
// TODO (exponentiale, pi, nan and inf)

/**
 * Creates the units for the SBML model.
 */
void CSBMLExporter::createUnits(const CCopasiDataModel& dataModel)
{
  createTimeUnit(dataModel);
  createVolumeUnit(dataModel);
  createSubstanceUnit(dataModel);
}

/**
 * Creates the time unit for the SBML model.
 */
void CSBMLExporter::createTimeUnit(const CCopasiDataModel& dataModel)
{
  if (dataModel.getModel() == NULL || this->mpSBMLDocument == NULL || this->mpSBMLDocument->getModel() == NULL) return;
  UnitDefinition uDef("time");
  uDef.setId("time");
  Unit unit;

  switch (dataModel.getModel()->getTimeUnitEnum())
    {
    case CModel::d:
      unit = Unit(UNIT_KIND_SECOND, 1, 0);
      unit.setMultiplier(86400);
      break;
    case CModel::h:
      unit = Unit(UNIT_KIND_SECOND, 1, 0);
      unit.setMultiplier(3600);
      break;
    case CModel::min:
      unit = Unit(UNIT_KIND_SECOND, 1, 0);
      unit.setMultiplier(60);
      break;
    case CModel::ms:
      unit = Unit(UNIT_KIND_SECOND, 1, -3);
      break;
    case CModel::micros:
      unit = Unit(UNIT_KIND_SECOND, 1, -6);
      break;
    case CModel::ns:
      unit = Unit(UNIT_KIND_SECOND, 1, -9);
      break;
    case CModel::ps:
      unit = Unit(UNIT_KIND_SECOND, 1, -12);
      break;
    case CModel::fs:
      unit = Unit(UNIT_KIND_SECOND, 1, -15);
      break;
    default:
      CCopasiMessage(CCopasiMessage::EXCEPTION, "SBMLExporter Error: Unknown copasi time unit.");
      break;
    }

  uDef.addUnit(&unit);
  Model* pSBMLModel = this->mpSBMLDocument->getModel();
  UnitDefinition* pUdef = pSBMLModel->getUnitDefinition("time");
  if (pUdef != NULL)
    {
      // check if it is the same unit as the exisiting one if there is one
      // if yes, return, else replace the existing one
      if (!SBMLImporter::areSBMLUnitDefinitionsIdentical(pUdef, &uDef))
        {
          (*pUdef) = uDef;
        }
    }
  else
    {
      if (unit.getKind() != UNIT_KIND_SECOND || unit.getScale() != 1 || unit.getExponent() != 0 || unit.getMultiplier() != 1.0)
        {
          // set the unit definition
          pSBMLModel->addUnitDefinition(&uDef);
        }
    }
}

/**
 * Creates the volume unit for the SBML model.
 */
void CSBMLExporter::createVolumeUnit(const CCopasiDataModel& dataModel)
{
  if (dataModel.getModel() == NULL || this->mpSBMLDocument == NULL || this->mpSBMLDocument->getModel() == NULL) return;
  UnitDefinition uDef("volume");
  uDef.setId("volume");
  Unit unit;
  switch (dataModel.getModel()->getVolumeUnitEnum())
    {
    case CModel::ml:
      unit = Unit(UNIT_KIND_LITRE, 1, -3);
      break;
    case CModel::microl:
      unit = Unit(UNIT_KIND_LITRE, 1, -6);
      break;
    case CModel::nl:
      unit = Unit(UNIT_KIND_LITRE, 1, -9);
      break;
    case CModel::pl:
      unit = Unit(UNIT_KIND_LITRE, 1, -12);
      break;
    case CModel::fl:
      unit = Unit(UNIT_KIND_LITRE, 1, -15);
      break;
    case CModel::m3:
      unit = Unit(UNIT_KIND_METRE, 3, 0);
      break;
    default:
      CCopasiMessage(CCopasiMessage::EXCEPTION, "SBMLExporter Error: Unknown copasi volume unit.");
      break;
    }
  uDef.addUnit(&unit);
  Model* pSBMLModel = this->mpSBMLDocument->getModel();
  UnitDefinition* pUdef = pSBMLModel->getUnitDefinition("volume");
  if (pUdef != NULL)
    {
      // check if it is the same unit as the exisiting one if there is one
      // if yes, return, else replace the existing one
      if (!SBMLImporter::areSBMLUnitDefinitionsIdentical(pUdef, &uDef))
        {
          (*pUdef) = uDef;
        }
    }
  else
    {
      if (unit.getKind() != UNIT_KIND_LITRE || unit.getScale() != 1 || unit.getExponent() != 0 || unit.getMultiplier() != 1.0)
        {
          // set the unit definition
          pSBMLModel->addUnitDefinition(&uDef);
        }
    }
}

/**
 * Creates the substance unit for the SBML model.
 */
void CSBMLExporter::createSubstanceUnit(const CCopasiDataModel& dataModel)
{
  if (dataModel.getModel() == NULL || this->mpSBMLDocument == NULL || this->mpSBMLDocument->getModel() == NULL) return;
  UnitDefinition uDef("substance");
  uDef.setId("substance");
  Unit unit;
  switch (dataModel.getModel()->getQuantityUnitEnum())
    {
    case CModel::mMol:
      unit = Unit(UNIT_KIND_MOLE, 1, -3);
      break;
    case CModel::microMol:
      unit = Unit(UNIT_KIND_MOLE, 1, -6);
      break;
    case CModel::nMol:
      unit = Unit(UNIT_KIND_MOLE, 1, -9);
      break;
    case CModel::pMol:
      unit = Unit(UNIT_KIND_MOLE, 1, -12);
      break;
    case CModel::fMol:
      unit = Unit(UNIT_KIND_MOLE, 1, -15);
      break;
    case CModel::number:
      unit = Unit(UNIT_KIND_ITEM, 1, 0);
      break;
    default:
      CCopasiMessage(CCopasiMessage::EXCEPTION, "SBMLExporter Error: Unknown copasi quantity unit.");
      break;
    }
  uDef.addUnit(&unit);
  Model* pSBMLModel = this->mpSBMLDocument->getModel();
  UnitDefinition* pUdef = pSBMLModel->getUnitDefinition("substance");
  if (pUdef != NULL)
    {
      // check if it is the same unit as the exisiting one if there is one
      // if yes, return, else replace the existing one
      if (!SBMLImporter::areSBMLUnitDefinitionsIdentical(pUdef, &uDef))
        {
          (*pUdef) = uDef;
        }
    }
  else
    {
      if (unit.getKind() != UNIT_KIND_MOLE || unit.getScale() != 1 || unit.getExponent() != 0 || unit.getMultiplier() != 1.0)
        {
          // set the unit definition
          pSBMLModel->addUnitDefinition(&uDef);
        }
    }
}

/**
 * Creates the compartments for the model.
 */
void CSBMLExporter::createCompartments(CCopasiDataModel& dataModel)
{
  // make sure the SBML Document already exists anf that it has a Model set
  if (dataModel.getModel() == NULL || this->mpSBMLDocument == NULL || this->mpSBMLDocument->getModel() == NULL) return;
  CCopasiVectorNS<CCompartment>::const_iterator it = dataModel.getModel()->getCompartments().begin(), endit = dataModel.getModel()->getCompartments().end();
  while (it != endit)
    {
      createCompartment(**it);
      ++it;
    }
}

/**
 * Creates the compartment for the given COPASI compartment.
 */
void CSBMLExporter::createCompartment(CCompartment& compartment)
{
  Compartment* pSBMLCompartment = NULL;
  std::map<const CCopasiObject*, SBase*>::iterator pos = this->mCOPASI2SBMLMap.find(&compartment);
  if (pos != this->mCOPASI2SBMLMap.end())
    {
      pSBMLCompartment = dynamic_cast<Compartment*>(pos->second);
      assert(pSBMLCompartment);
    }
  else
    {
      pSBMLCompartment = this->mpSBMLDocument->getModel()->createCompartment();
      this->mCOPASI2SBMLMap[&compartment] = pSBMLCompartment;

      std::string id = compartment.getSBMLId();
      if (id == "")
        {
          id = CSBMLExporter::createUniqueId(this->mIdMap, "compartment_");
          this->mIdMap.insert(std::make_pair(id, pSBMLCompartment));
          compartment.setSBMLId(id);
        }
      pSBMLCompartment->setId(id);
      pSBMLCompartment->setSpatialDimensions(3);
      pSBMLCompartment->setConstant(true);
    }
  this->mHandledSBMLObjects.insert(pSBMLCompartment);
  pSBMLCompartment->setName(compartment.getObjectName().c_str());
  double value = compartment.getInitialValue();
  // if the value is NaN, unset the initial volume
  if (!isnan(value))
    {
      pSBMLCompartment->setVolume(value);
    }
  else
    {
      pSBMLCompartment->unsetVolume();
    }
  // fill assignment set
  // a compartment can either have an assignment or an initial assignment but not
  // both
  CModelEntity::Status status = compartment.getStatus();
  if (status == CModelEntity::ODE || status == CModelEntity::ASSIGNMENT)
    {
      this->mAssignmentVector.push_back(&compartment);
    }
  // fill initial assignment set
  else if (compartment.getInitialExpressionPtr() != NULL)
    {
      this->mInitialAssignmentVector.push_back(&compartment);
    }
}

/**
 * Creates the compartments for the model.
 */
void CSBMLExporter::createMetabolites(CCopasiDataModel& dataModel)
{
  // make sure the SBML Document already exists anf that it has a Model set
  if (dataModel.getModel() == NULL || this->mpSBMLDocument == NULL || this->mpSBMLDocument->getModel() == NULL) return;
  CCopasiVector<CMetab>::const_iterator it = dataModel.getModel()->getMetabolites().begin(), endit = dataModel.getModel()->getMetabolites().end();
  while (it != endit)
    {
      createMetabolite(**it);
      ++it;
    }
}

/**
 * Creates the species for the given COPASI metabolite.
 */
void CSBMLExporter::createMetabolite(CMetab& metab)
{
  Species* pSBMLSpecies = NULL;
  std::map<const CCopasiObject*, SBase*>::iterator pos = this->mCOPASI2SBMLMap.find(&metab);
  if (pos != mCOPASI2SBMLMap.end())
    {
      pSBMLSpecies = dynamic_cast<Species*>(pos->second);
      assert(pSBMLSpecies);
    }
  else
    {
      pSBMLSpecies = this->mpSBMLDocument->getModel()->createSpecies();
      this->mCOPASI2SBMLMap[&metab] = pSBMLSpecies;
      std::string id = metab.getSBMLId();
      if (id == "")
        {
          id = CSBMLExporter::createUniqueId(this->mIdMap, "species_");
          metab.setSBMLId(id);
          this->mIdMap.insert(std::make_pair(id, pSBMLSpecies));
        }
      pSBMLSpecies->setId(id);
    }
  this->mHandledSBMLObjects.insert(pSBMLSpecies);
  pSBMLSpecies->setName(metab.getObjectName().c_str());
  pSBMLSpecies->setBoundaryCondition(metab.getStatus() == CModelEntity::FIXED);
  pSBMLSpecies->setConstant(metab.getStatus() == CModelEntity::FIXED);

  const Compartment* pSBMLCompartment = dynamic_cast<const Compartment*>(this->mCOPASI2SBMLMap[const_cast<CCompartment*>(metab.getCompartment())]);
  assert(pSBMLCompartment);
  pSBMLSpecies->setCompartment(pSBMLCompartment->getId());
  double value = metab.getInitialConcentration();
  // if the value is NaN, pnset the initial amount
  if (!isnan(value))
    {
      // if we set the concentration on a species that had the amount
      // set, the meaning of the model is different when the user changed
      // to size of the corresponding compartment
      // So if the amount had been set, we try to keep this.
      if (pSBMLSpecies->isSetInitialAmount())
        {
          pSBMLSpecies->setInitialAmount(value*metab.getCompartment()->getInitialValue());
        }
      else
        {
          pSBMLSpecies->setInitialConcentration(value);
        }
    }
  else
    {
      pSBMLSpecies->unsetInitialConcentration();
      pSBMLSpecies->unsetInitialAmount();
    }
  CModelEntity::Status status = metab.getStatus();
  // a species can either have an assignment or an initial assignment but not
  // both
  if (status == CModelEntity::ODE || status == CModelEntity::ASSIGNMENT)
    {
      this->mAssignmentVector.push_back(&metab);
    }
  // fill initial assignment set
  else if (metab.getInitialExpressionPtr() != NULL)
    {
      this->mInitialAssignmentVector.push_back(&metab);
    }
}

/**
 * Creates the parameters for the model.
 */
void CSBMLExporter::createParameters(CCopasiDataModel& dataModel)
{
  // make sure the SBML Document already exists anf that it has a Model set
  if (dataModel.getModel() == NULL || this->mpSBMLDocument == NULL || this->mpSBMLDocument->getModel() == NULL) return;
  CCopasiVectorN<CModelValue>::const_iterator it = dataModel.getModel()->getModelValues().begin(), endit = dataModel.getModel()->getModelValues().end();
  while (it != endit)
    {
      createParameter(**it);
      ++it;
    }
}

/**
 * Creates the parameter for the given COPASI parameter.
 */
void CSBMLExporter::createParameter(CModelValue& modelValue)
{
  Parameter* pParameter = NULL;
  std::map<const CCopasiObject*, SBase*>::iterator pos = this->mCOPASI2SBMLMap.find(&modelValue);
  if (pos != this->mCOPASI2SBMLMap.end())
    {
      pParameter = dynamic_cast<Parameter*>(pos->second);
      assert(pParameter);
    }
  else
    {
      pParameter = this->mpSBMLDocument->getModel()->createParameter();
      this->mCOPASI2SBMLMap[&modelValue] = pParameter;
      std::string id = modelValue.getSBMLId();
      if (id == "")
        {
          id = CSBMLExporter::createUniqueId(this->mIdMap, "parameter_");
          modelValue.setSBMLId(id);
          this->mIdMap.insert(std::make_pair(id, pParameter));
        }
      pParameter->setId(id);
    }
  this->mHandledSBMLObjects.insert(pParameter);
  pParameter->setName(modelValue.getObjectName());
  double value = modelValue.getInitialValue();
  // if the value is NaN, unset the parameters value
  if (!isnan(value))
    {
      pParameter->setValue(value);
    }
  else
    {
      pParameter->unsetValue();
    }
  // fill assignment set
  // a parameter can either have an assignment or an initial assignment but not
  // both
  CModelEntity::Status status = modelValue.getStatus();
  if (status == CModelEntity::ODE || status == CModelEntity::ASSIGNMENT)
    {
      this->mAssignmentVector.push_back(&modelValue);
    }
  // fill initial assignment set
  else if (modelValue.getInitialExpressionPtr() != NULL)
    {
      this->mInitialAssignmentVector.push_back(&modelValue);
    }
}

/**
 * Creates the reactions for the model.
 */
void CSBMLExporter::createReactions(CCopasiDataModel& dataModel)
{
  // make sure the SBML Document already exists anf that it has a Model set
  if (dataModel.getModel() == NULL || this->mpSBMLDocument == NULL || this->mpSBMLDocument->getModel() == NULL) return;
  CCopasiVectorNS<CReaction>::const_iterator it = dataModel.getModel()->getReactions().begin(), endit = dataModel.getModel()->getReactions().end();
  while (it != endit)
    {
      createReaction(**it, dataModel);
      ++it;
    }
}

/**
 * Creates the reaction for the given COPASI reaction.
 */
void CSBMLExporter::createReaction(CReaction& reaction, CCopasiDataModel& dataModel)
{
  // TODO make sure the mCOPASI2SBMLMap is already updated
  Reaction* pSBMLReaction = NULL;
  // if the reaction has nothing set but the name, we don't do anything
  if (reaction.getChemEq().getSubstrates().size() == 0 &&
      reaction.getChemEq().getProducts().size() == 0) return;
  std::map<const CCopasiObject*, SBase*>::iterator pos = this->mCOPASI2SBMLMap.find(&reaction);
  if (pos != this->mCOPASI2SBMLMap.end())
    {
      pSBMLReaction = dynamic_cast<Reaction*>(pos->second);
      assert(pSBMLReaction);
    }
  else
    {
      /* create a new reaction object */
      pSBMLReaction = this->mpSBMLDocument->getModel()->createReaction();
      mCOPASI2SBMLMap[&reaction] = pSBMLReaction;
      std::string id = reaction.getSBMLId();
      if (id == "")
        {
          id = CSBMLExporter::createUniqueId(this->mIdMap, "reaction_");
          reaction.setSBMLId(id);
          this->mIdMap.insert(std::make_pair(id, pSBMLReaction));
        }
      pSBMLReaction->setId(id);
    }
  this->mHandledSBMLObjects.insert(pSBMLReaction);
  pSBMLReaction->setName(reaction.getObjectName().c_str());
  pSBMLReaction->setReversible(reaction.isReversible());
  const CChemEq& chemicalEquation = reaction.getChemEq();
  /* Add all substrates */
  unsigned int counter;
  std::set<std::string> usedReferences;
  for (counter = 0; counter < chemicalEquation.getSubstrates().size(); counter++)
    {
      const CChemEqElement* element = chemicalEquation.getSubstrates()[counter];
      const CMetab* pMetabolite = element->getMetabolite();
      assert(pMetabolite);
      SpeciesReference* sRef = NULL;
      if (!(sRef = pSBMLReaction->getReactant(pMetabolite->getSBMLId())))
        {
          sRef = pSBMLReaction->createReactant();
          sRef->setSpecies(pMetabolite->getSBMLId().c_str());
        }
      sRef->setStoichiometry(element->getMultiplicity());
      sRef->setDenominator(1);
      usedReferences.insert(sRef->getSpecies());
    }
  ListOf* l = pSBMLReaction->getListOfReactants();
  for (counter = l->size(); counter > 0;--counter)
    {
      if (usedReferences.find(static_cast<SimpleSpeciesReference*>(l->get(counter - 1))->getSpecies()) == usedReferences.end())
        {
          l->remove(counter - 1);
        }
    }
  /* Add all products */
  usedReferences.clear();
  for (counter = 0; counter < chemicalEquation.getProducts().size(); counter++)
    {
      const CChemEqElement* element = chemicalEquation.getProducts()[counter];
      const CMetab* pMetabolite = element->getMetabolite();
      assert(pMetabolite);
      SpeciesReference* sRef = NULL;
      if (!(sRef = pSBMLReaction->getProduct(pMetabolite->getSBMLId())))
        {
          sRef = pSBMLReaction->createProduct();
          sRef->setSpecies(pMetabolite->getSBMLId().c_str());
        }
      sRef->setStoichiometry(element->getMultiplicity());
      sRef->setDenominator(1);
      usedReferences.insert(sRef->getSpecies());
    }
  l = pSBMLReaction->getListOfProducts();
  for (counter = l->size(); counter > 0;--counter)
    {
      if (usedReferences.find(static_cast<SimpleSpeciesReference*>(l->get(counter - 1))->getSpecies()) == usedReferences.end())
        {
          l->remove(counter - 1);
        }
    }
  /* Add all modifiers */
  usedReferences.clear();
  for (counter = 0; counter < chemicalEquation.getModifiers().size(); counter++)
    {
      const CChemEqElement* element = chemicalEquation.getModifiers()[counter];
      const CMetab* pMetabolite = element->getMetabolite(); assert(pMetabolite);
      ModifierSpeciesReference* sRef = NULL;
      if (!(sRef = pSBMLReaction->getModifier(pMetabolite->getSBMLId())))
        {
          sRef = pSBMLReaction->createModifier();
          sRef->setSpecies(pMetabolite->getSBMLId().c_str());
        }
      usedReferences.insert(sRef->getSpecies());
    }
  l = pSBMLReaction->getListOfModifiers();
  for (counter = l->size(); counter > 0;--counter)
    {
      if (usedReferences.find(static_cast<SimpleSpeciesReference*>(l->get(counter - 1))->getSpecies()) == usedReferences.end())
        {
          l->remove(counter - 1);
        }
    }
  /* create the kinetic law */
  /* if there is one on copasi */
  if ((reaction.getFunction()) != dataModel.mpUndefined)
    {
      // create an expression from the function call and the parameters and
      // check the expression
      std::vector<SBMLIncompatibility> result;
      CEvaluationNode* pExpressionNode = CSBMLExporter::createExpressionTree(reaction.getFunction(), reaction.getParameterMappings(), dataModel);
      CExpression* pExpression = new CExpression();
      pExpression->setRoot(pExpressionNode);
      pExpression->compile();
      CSBMLExporter::isExpressionSBMLCompatible(*pExpression, dataModel, this->mSBMLLevel, this->mSBMLVersion, result);
      delete pExpression;
      if (result.empty())
        {
          KineticLaw* kLaw = this->createKineticLaw(reaction, dataModel);
          if (kLaw != NULL)
            {
              pSBMLReaction->setKineticLaw(kLaw);
              delete kLaw;
            }
          else
            {
              // create an error message and abort
              // maybe only abort if incomplete export is not wanted
              if (this->mIncompleteExport != true)
                {
                  CCopasiMessage::CCopasiMessage(CCopasiMessage::EXCEPTION, MCSBML + 8, reaction.getObjectName().c_str());
                }
              else
                {
                  pSBMLReaction->unsetKineticLaw();
                }
            }
        }
      else
        {
          // create an error messages
          //  abort the export if
          // incomplete export is not wanted
          // else set the kinetic law to undefined
          std::vector<SBMLIncompatibility>::iterator it = result.begin(), endit = result.end();
          while (it != endit)
            {
              this->mIncompatibilities.push_back(*it);
              ++it;
            }
          if (this->mIncompleteExport != true)
            {
              CCopasiMessage::CCopasiMessage(CCopasiMessage::EXCEPTION, MCSBML + 8, reaction.getObjectName().c_str());
            }
          else
            {
              pSBMLReaction->unsetKineticLaw();
            }
        }
    }
  else
    {
      pSBMLReaction->unsetKineticLaw();
    }
}

/**
 * Creates the initial assignments for the model.
 */
void CSBMLExporter::createInitialAssignments(CCopasiDataModel& dataModel)
{
  // TODO make sure the mInitialAssignmentVector has been filled already

  // create the initial assignments
  unsigned int i, iMax = this->mInitialAssignmentVector.size();
  const CModelEntity* pME = NULL;
  for (i = 0;i < iMax;++i)
    {
      pME = mInitialAssignmentVector[i];
      assert(pME != NULL);
      if (pME != NULL)
        {
          createInitialAssignment(*pME, dataModel);
        }
    }
}

/**
 * Creates the initial assignment for the given COPASI model entity.
 */
void CSBMLExporter::createInitialAssignment(const CModelEntity& modelEntity, CCopasiDataModel& dataModel)
{
  // check the expression
  std::vector<SBMLIncompatibility> result;
  CSBMLExporter::isExpressionSBMLCompatible(*modelEntity.getInitialExpressionPtr(), dataModel, this->mSBMLLevel, this->mSBMLVersion, result);
  // collect directly used functions
  if (result.empty())
    {
      std::set<std::string> directlyUsedFunctionNames;
      CSBMLExporter::findDirectlyUsedFunctions(modelEntity.getInitialExpressionPtr()->getRoot(), directlyUsedFunctionNames);
      std::set<CFunction*> usedFunctions = CSBMLExporter::createFunctionSetFromFunctionNames(directlyUsedFunctionNames, dataModel.getFunctionList());
      this->mUsedFunctions.insert(usedFunctions.begin(), usedFunctions.end());
      // create the actual initial assignment
      InitialAssignment* pInitialAssignment = this->mpSBMLDocument->getModel()->getInitialAssignment(modelEntity.getSBMLId());
      if (pInitialAssignment == NULL)
        {
          pInitialAssignment = this->mpSBMLDocument->getModel()->createInitialAssignment();
          pInitialAssignment->setSymbol(modelEntity.getSBMLId());
        }
      // set the math
      ASTNode* pNode = modelEntity.getInitialExpressionPtr()->getRoot()->toAST();
      if (pNode != NULL)
        {
          pInitialAssignment->setMath(pNode);
          delete pNode;
        }
      else
        {
          if (this->mIncompleteExport == true)
            {
              // remove the initial assignment from the SBML model
              unsigned int i = 0, iMax = this->mpSBMLDocument->getModel()->getNumInitialAssignments();
              while (i < iMax)
                {
                  if (this->mpSBMLDocument->getModel()->getInitialAssignment(i)->getSymbol() == modelEntity.getSBMLId())
                    {
                      this->mpSBMLDocument->getModel()->getListOfInitialAssignments()->remove(i);
                      break;
                    }
                  ++i;
                }
            }
          else
            {
              CCopasiMessage::CCopasiMessage(CCopasiMessage::EXCEPTION, MCSBML + 60, "initial assignment", modelEntity.getObjectType().c_str(), modelEntity.getObjectName().c_str());
            }
        }
    }
  else
    {
      if (this->mIncompleteExport)
        {
          this->mIncompatibilities.insert(this->mIncompatibilities.end(), result.begin(), result.end());
        }
      else
        {
          CCopasiMessage::CCopasiMessage(CCopasiMessage::EXCEPTION, MCSBML + 60, "initial assignment", modelEntity.getObjectType().c_str(), modelEntity.getObjectName().c_str());
        }
    }
}

/**
 * Creates the rules for the model.
 */
void CSBMLExporter::createRules(CCopasiDataModel& dataModel)
{
  // TODO make sure the mAssignmentVector has been filled already
  // order the rules for Level 1 export
  // rules in Level 2 are not ordered.
  if (this->mSBMLLevel == 1 || (this->mSBMLLevel == 2 && this->mSBMLVersion == 1))
    {
      orderRules(dataModel);
    }

  // create the rules
  unsigned int i, iMax = mAssignmentVector.size();
  const CModelEntity* pME = NULL;
  for (i = 0;i < iMax;++i)
    {
      pME = mAssignmentVector[i];
      assert(pME != NULL);
      if (pME != NULL)
        {
          createRule(*pME, dataModel);
        }
    }
}

/**
 * Creates the rule for the given COPASI model entity.
 */
void CSBMLExporter::createRule(const CModelEntity& modelEntity, CCopasiDataModel& dataModel)
{
  // check the expression
  std::vector<SBMLIncompatibility> result;
  CSBMLExporter::isExpressionSBMLCompatible(*modelEntity.getExpressionPtr(), dataModel, this->mSBMLLevel, this->mSBMLVersion, result);
  // collect directly used functions
  if (result.empty())
    {
      std::set<std::string> directlyUsedFunctionNames;
      CSBMLExporter::findDirectlyUsedFunctions(modelEntity.getExpressionPtr()->getRoot(), directlyUsedFunctionNames);
      std::set<CFunction*> usedFunctions = CSBMLExporter::createFunctionSetFromFunctionNames(directlyUsedFunctionNames, dataModel.getFunctionList());
      this->mUsedFunctions.insert(usedFunctions.begin(), usedFunctions.end());
      // create the actual initial assignment
      Rule* pRule = this->mpSBMLDocument->getModel()->getRule(modelEntity.getSBMLId());
      if (pRule == NULL)
        {
          if (modelEntity.getStatus() == CModelEntity::ASSIGNMENT)
            {
              pRule = this->mpSBMLDocument->getModel()->createAssignmentRule();
            }
          else
            {
              pRule = this->mpSBMLDocument->getModel()->createRateRule();
            }
          pRule->setVariable(modelEntity.getSBMLId());
        }
      // set the math
      ASTNode* pNode = modelEntity.getExpressionPtr()->getRoot()->toAST();
      if (pNode != NULL)
        {
          pRule->setMath(pNode);
          delete pNode;
        }
      else
        {
          if (this->mIncompleteExport == true)
            {
              // remove the rule from the SBML model
              unsigned int i = 0, iMax = this->mpSBMLDocument->getModel()->getNumRules();
              while (i < iMax)
                {
                  if (this->mpSBMLDocument->getModel()->getRule(i)->getVariable() == modelEntity.getSBMLId())
                    {
                      this->mpSBMLDocument->getModel()->getListOfRules()->remove(i);
                      break;
                    }
                  ++i;
                }
            }
          else
            {
              CCopasiMessage::CCopasiMessage(CCopasiMessage::EXCEPTION, MCSBML + 60, "rule", modelEntity.getObjectType().c_str(), modelEntity.getObjectName().c_str());
            }
        }
    }
  else
    {
      if (this->mIncompleteExport)
        {
          this->mIncompatibilities.insert(this->mIncompatibilities.end(), result.begin(), result.end());
        }
      else
        {
          CCopasiMessage::CCopasiMessage(CCopasiMessage::EXCEPTION, MCSBML + 60, "rule", modelEntity.getObjectType().c_str(), modelEntity.getObjectName().c_str());
        }
    }
}

const std::map<std::string, const SBase*> CSBMLExporter::createIdMap(const Model& sbmlModel)
{
  // go through all function definitions, compartments, species, reactions,
  // parameters and events and create a set with all used ids.
  std::map<std::string, const SBase*> idMap;
  unsigned int i, iMax;
  if (sbmlModel.isSetId())
    {
      idMap.insert(std::make_pair(sbmlModel.getId(), &sbmlModel));
    }
  if (sbmlModel.getListOfFunctionDefinitions()->isSetId())
    {
      idMap.insert(std::make_pair(sbmlModel.getListOfFunctionDefinitions()->getId(), sbmlModel.getListOfFunctionDefinitions()));
    }
  iMax = sbmlModel.getNumFunctionDefinitions();
  for (i = 0; i < iMax;++i)
    {
      idMap.insert(std::make_pair(sbmlModel.getFunctionDefinition(i)->getId(), sbmlModel.getFunctionDefinition(i)));
    }
  if (sbmlModel.getListOfCompartments()->isSetId())
    {
      idMap.insert(std::make_pair(sbmlModel.getListOfCompartments()->getId(), sbmlModel.getListOfCompartments()));
    }
  iMax = sbmlModel.getNumCompartments();
  for (i = 0; i < iMax;++i)
    {
      idMap.insert(std::make_pair(sbmlModel.getCompartment(i)->getId(), sbmlModel.getCompartment(i)));
    }
  if (sbmlModel.getListOfSpecies()->isSetId())
    {
      idMap.insert(std::make_pair(sbmlModel.getListOfSpecies()->getId(), sbmlModel.getListOfSpecies()));
    }
  iMax = sbmlModel.getNumSpecies();
  for (i = 0; i < iMax;++i)
    {
      idMap.insert(std::make_pair(sbmlModel.getSpecies(i)->getId(), sbmlModel.getSpecies(i)));
    }
  if (sbmlModel.getListOfParameters()->isSetId())
    {
      idMap.insert(std::make_pair(sbmlModel.getListOfParameters()->getId(), sbmlModel.getListOfParameters()));
    }
  iMax = sbmlModel.getNumParameters();
  for (i = 0; i < iMax;++i)
    {
      idMap.insert(std::make_pair(sbmlModel.getParameter(i)->getId(), sbmlModel.getParameter(i)));
    }
  if (sbmlModel.getListOfReactions()->isSetId())
    {
      idMap.insert(std::make_pair(sbmlModel.getListOfReactions()->getId(), sbmlModel.getListOfReactions()));
    }
  iMax = sbmlModel.getNumReactions();
  for (i = 0; i < iMax;++i)
    {
      const Reaction* pReaction = sbmlModel.getReaction(i);
      if (pReaction != NULL)
        {
          idMap.insert(std::make_pair(pReaction->getId(), pReaction));
          if (pReaction->getListOfReactants()->isSetId())
            {
              idMap.insert(std::make_pair(pReaction->getListOfReactants()->getId(), pReaction->getListOfReactants()));
            }
          unsigned int j, jMax = pReaction->getNumReactants();
          const SpeciesReference* pSRef = NULL;
          for (j = 0;j < jMax;++j)
            {
              pSRef = pReaction->getReactant(j);
              if (pSRef->isSetId())
                {
                  idMap.insert(std::make_pair(pSRef->getId(), pSRef));
                }
            }
          if (pReaction->getListOfProducts()->isSetId())
            {
              idMap.insert(std::make_pair(pReaction->getListOfProducts()->getId(), pReaction->getListOfProducts()));
            }
          jMax = pReaction->getNumProducts();
          for (j = 0;j < jMax;++j)
            {
              pSRef = pReaction->getProduct(j);
              if (pSRef->isSetId())
                {
                  idMap.insert(std::make_pair(pSRef->getId(), pSRef));
                }
            }
          if (pReaction->getListOfModifiers()->isSetId())
            {
              idMap.insert(std::make_pair(pReaction->getListOfModifiers()->getId(), pReaction->getListOfModifiers()));
            }
          jMax = pReaction->getNumModifiers();
          const ModifierSpeciesReference* pModSRef;
          for (j = 0;j < jMax;++j)
            {
              pModSRef = pReaction->getModifier(j);
              if (pModSRef->isSetId())
                {
                  idMap.insert(std::make_pair(pModSRef->getId(), pModSRef));
                }
            }
        }
    }
  if (sbmlModel.getListOfEvents()->isSetId())
    {
      idMap.insert(std::make_pair(sbmlModel.getListOfEvents()->getId(), sbmlModel.getListOfEvents()));
    }
  iMax = sbmlModel.getNumEvents();
  for (i = 0; i < iMax;++i)
    {
      idMap.insert(std::make_pair(sbmlModel.getEvent(i)->getId(), sbmlModel.getEvent(i)));
    }
  // if COPASI is compiled with layout, we have to add those ids as well
#ifdef WITH_LAYOUT
  if (sbmlModel.getListOfLayouts()->isSetId())
    {
      idMap.insert(std::make_pair(sbmlModel.getListOfLayouts()->getId(), sbmlModel.getListOfLayouts()));
    }
  iMax = sbmlModel->getNumLayouts();
  for (i = 0;i < iMax;++i)
    {
      const Layout* pLayout = sbmlModel.getLayout(i);
      const SBase* pSBase = NULL;
      if (pLayout != NULL)
        {
          if (pLayout->isSetId())
            {
              idMap.insert(std::make_pair(pLayout->getId(), pLayout));
            }
        }
    }
#endif // WITH_LAYOUT
  return idMap;
}

/**
 * Create a unique id for an SBML object.
 * I can't just take the Copasi key of the object since this might conflict
 * with an already existing sbml id which came from the sbmlid attribute in a
 * copasi file or directly by importing an SBML file.
 */
const std::string CSBMLExporter::createUniqueId(const std::map<std::string, const SBase*>& idMap, const std::string& prefix)
{
  unsigned int i = 0;
  std::ostringstream numberStream;
  numberStream << i;
  while (idMap.find(numberStream.str()) != idMap.end())
    {
      ++i;
      numberStream.str(prefix);
      numberStream.str("");
      numberStream << i;
    }
  return numberStream.str();
}

/**
 * Checks all assignments (initial and transient) for references to objects
 * that can not be exported to SBML.
 */
void CSBMLExporter::checkForUnsupportedObjectReferences(const CCopasiDataModel& dataModel, unsigned int sbmlLevel, unsigned int sbmlVersion, std::vector<SBMLIncompatibility>& result)
{
  // check all metabolites,parameters and compartments
  const CModel* pModel = dataModel.getModel();
  assert(pModel);
  if (pModel == NULL) return;
  unsigned int i, iMax = this->mAssignmentVector.size();
  const CModelEntity* pME;
  for (i = 0;i < iMax;++i)
    {
      pME = this->mAssignmentVector[i];
      assert(pME != NULL);
      if (pME != NULL)
        {
          checkForUnsupportedObjectReferences(*pME->getExpressionPtr(), dataModel, sbmlLevel, sbmlVersion, result);
        }
    }
  // check initial assignments
  iMax = this->mInitialAssignmentVector.size();
  for (i = 0;i < iMax;++i)
    {
      pME = this->mInitialAssignmentVector[i];
      assert(pME != NULL);
      if (pME != NULL)
        {
          checkForUnsupportedObjectReferences(*pME->getInitialExpressionPtr(), dataModel, sbmlLevel, sbmlVersion, result);
        }
    }
}

void checkForUnsupportedObjectReferences(const CExpression* pExpr, const CCopasiDataModel& dataModel, unsigned int sbmlLevel, unsigned int sbmlVersion, std::vector<SBMLIncompatibility>& result)
{
  // SBML Level 1 and Level 2 Version 1 can have references to transient values of
  // compartments, metabolites and global parameters as well as time (model)

  // SBML Level 2 Version 2 and above can have the same references as level 1 plus references to
  // reaction fluxes.
  if (pExpr == NULL) return;
  const std::vector<CEvaluationNode*>& objectNodes = pExpr->getNodeList();
  unsigned j, jMax = objectNodes.size();
  for (j = 0;j < jMax;++j)
    {
      if (CEvaluationNode::type(objectNodes[j]->getType()) == CEvaluationNode::OBJECT)
        {
          const CEvaluationNodeObject* pObjectNode = dynamic_cast<const CEvaluationNodeObject*>(objectNodes[j]);
          assert(pObjectNode);
          if (pObjectNode == NULL) continue;
          std::vector<CCopasiContainer*> containers;
          containers.push_back(const_cast<CModel*>(dataModel.getModel()));
          const CCopasiObject* pObject = CCopasiContainer::ObjectFromName(containers, pObjectNode->getObjectCN());
          assert(pObject);
          if (pObject->isReference())
            {
              const CCopasiObject* pObjectParent = pObject->getObjectParent();
              assert(pObjectParent);
              std::string typeString = pObjectParent->getObjectType();
              if (typeString == "Compartment")
                {
                  // must be a reference to the (transient) or initial volume
                  if (pObject->getObjectName() != "Volume")
                    {
                      result.push_back(SBMLIncompatibility(1, pObject->getObjectName().c_str(), "compartment", pObjectParent->getObjectName().c_str()));
                    }
                }
              else if (typeString == "Metabolite")
                {
                  // must be a reference to the transient or initial concentration
                  if (pObject->getObjectName() != "Concentration")
                    {
                      result.push_back(SBMLIncompatibility(1, pObject->getObjectName().c_str(), "metabolite", pObjectParent->getObjectName().c_str()));
                    }
                }
              else if (typeString == "ModelValue")
                {
                  // must be a reference to the transient or initial value
                  if (pObject->getObjectName() != "Value")
                    {
                      result.push_back(SBMLIncompatibility(1, pObject->getObjectName().c_str(), "parameter", pObjectParent->getObjectName().c_str()));
                    }
                }
              else if (typeString == "Model")
                {
                  // must be a reference to the model time
                  if (pObject->getObjectName() != "Time")
                    {
                      result.push_back(SBMLIncompatibility(1, pObject->getObjectName().c_str(), "model", pObjectParent->getObjectName().c_str()));
                    }
                }
              else
                {
                  if (sbmlLevel == 1 || (sbmlLevel == 2 && sbmlVersion == 1))
                    {
                      if (typeString == "Reaction")
                        {
                          if (pObject->getObjectName() != "Flux")
                            {
                              result.push_back(SBMLIncompatibility(1, pObject->getObjectName().c_str(), "reaction", pObjectParent->getObjectName().c_str()));
                            }
                        }
                    }
                  else
                    {
                      result.push_back(SBMLIncompatibility(1, pObject->getObjectName().c_str(), typeString.c_str(), pObjectParent->getObjectName().c_str()));
                    }
                }
            }
          else
            {
              result.push_back(SBMLIncompatibility(1, "value", pObject->getObjectType().c_str() , pObject->getObjectName().c_str()));
            }
        }
    }
}

/**
 * Checks whether the given data model can be exported to SBML Level1
 * If it can be exported, the result vector will be empty, otherwise it will
 * contain a number of messages that specify why it can't be exported.
 */
void CSBMLExporter::isModelSBMLL1Compatible(const CCopasiDataModel& dataModel, std::vector<SBMLIncompatibility>& result)
{
  // check for piecewise functions
  checkForPiecewiseFunctions(dataModel, result);

  // check for initial assignments
  checkForInitialAssignments(dataModel, result);

  // check for event
  checkForEvents(dataModel, result);
}

/**
 * Checks whether the given data model can be exported to SBML Level2 Version1.
 * If it can be exported, the result vector will be empty, otherwise it will
 * contain a number of messages that specify why it can't be exported.
 */
void CSBMLExporter::isModelSBMLL2V1Compatible(const CCopasiDataModel& dataModel, std::vector<SBMLIncompatibility>& result)
{
  // check for initial assignments
  CSBMLExporter::checkForInitialAssignments(dataModel, result);
}

/**
 * Checks whether the given data model can be exported to SBML Level2 Version3.
 * If it can be exported, the result vector will be empty, otherwise it will
 * contain a number of messages that specify why it can't be exported.
 */
void CSBMLExporter::isModelSBMLL2V3Compatible(const CCopasiDataModel& /*dataModel*/, std::vector<SBMLIncompatibility>& /*result*/)
{
  // there are no special checks for the Level and Version yet
}

/**
 * Checks whether the model contains a metabolite that is defined by an ODE
 * expression and that is located in a variable volume. Since COPASI
 * interprets the expression differntly from SBML, we can not correctly
 * export this yet. See Bug 903.
 */
void CSBMLExporter::checkForODESpeciesInNonfixedCompartment(const CCopasiDataModel& dataModel, std::vector<SBMLIncompatibility> result)
{
  const CModel* pModel = dataModel.getModel();
  const CCopasiVector<CMetab>& metabolites = pModel->getMetabolites();
  CCopasiVector<CMetab>::const_iterator it = metabolites.begin(), endit = metabolites.end();
  while (it != endit)
    {
      if ((*it)->getStatus() == CModelValue::ODE)
        {
          const CCompartment* pCompartment = (*it)->getCompartment();
          assert(pCompartment != NULL);
          if (pCompartment->getStatus() != CModelValue::FIXED)
            {
              result.push_back(SBMLIncompatibility(3, (*it)->getObjectName().c_str(), pCompartment->getObjectName().c_str()));
              //CCopasiMessage::CCopasiMessage(CCopasiMessage::ERROR, MCSBML + 52, (*it)->getObjectName().c_str());
            }
        }
      ++it;
    }
}

/**
 * Checks whether the rule in the given model entity can be exported to
 * the specified version of SBML.
 * If it can be exported, the result vector will be empty, otherwise it will
 * contain a number of messages that specify why it can't be exported.
 */
void CSBMLExporter::isExpressionSBMLCompatible(const CEvaluationTree& expr, const CCopasiDataModel& dataModel, int sbmlLevel, int sbmlVersion, std::vector<SBMLIncompatibility>& result)
{
  checkForUnsupportedObjectReferences(expr, dataModel, sbmlLevel, sbmlVersion, result);
  std::set<CEvaluationNodeFunction::SubType> unsupportedFunctionTypes = CSBMLExporter::createUnsupportedFunctionTypeSet(sbmlLevel);
  checkForUnsupportedFunctionCalls(expr, unsupportedFunctionTypes, result);
}

/**
 * This static methods checks, whether the model uses any function calls
 * that can not be expressed in SBML like the random distribution
 * functions.
 */
void CSBMLExporter::checkForUnsupportedFunctionCalls(const CCopasiDataModel& dataModel,
    unsigned int sbmlLevel, unsigned int /*sbmlVersion*/, std::vector<SBMLIncompatibility>& result)
{
  // Fill the set of unsupported functions depending on the level and
  // version
  std::set<CEvaluationNodeFunction::SubType> unsupportedFunctionTypes = CSBMLExporter::createUnsupportedFunctionTypeSet(sbmlLevel);
  // check all metabolites,parameters and compartments
  // TODO make sure the list of assignments and initial assignments is filled
  // before this function is called
  const CModel* pModel = dataModel.getModel();
  assert(pModel);
  unsigned int i, iMax = mAssignmentVector.size();
  const CModelEntity* pME = NULL;
  for (i = 0;i < iMax;++i)
    {
      pME = mAssignmentVector[i];
      assert(pME != NULL);
      if (pME != NULL)
        {
          checkForUnsupportedFunctionCalls(*pME->getExpressionPtr(), unsupportedFunctionTypes, result);
        }
    }
  // check initial assignments
  iMax = mInitialAssignmentVector.size();
  for (i = 0;i < iMax;++i)
    {
      pME = mInitialAssignmentVector[i];
      assert(pME != NULL);
      if (pME != NULL)
        {
          checkForUnsupportedFunctionCalls(*pME->getInitialExpressionPtr(), unsupportedFunctionTypes, result);
        }
    }
  // if we already have a list of used functions, we can go through this here
  // TODO make sure the list of used function has been filled before this is
  // called
  iMax = mUsedFunctions.size();
  std::set<CFunction*>::iterator it = this->mUsedFunctions.begin(), endit = this->mUsedFunctions.end();
  while (it != endit)
    {
      assert(*it != NULL);
      if (*it != NULL)
        {
          checkForUnsupportedFunctionCalls(**it, unsupportedFunctionTypes, result);
        }
      ++it;
    }
}

/**
 * This static methods checks recursively, whether the given CEvaluationNode constains any function calls
 * that can not be expressed in SBML like the random distribution
 * functions.
 */
void CSBMLExporter::checkForUnsupportedFunctionCalls(const CEvaluationNode& node,
    const std::set<CEvaluationNodeFunction::SubType>& unsupportedFunctions,
    std::vector<SBMLIncompatibility>& result, const std::string& objectName, const std::string& objectType)
{
  if (CEvaluationNode::type(node.getType()) == CEvaluationNode::FUNCTION)
    {
      CEvaluationNodeFunction::SubType subtype = (CEvaluationNodeFunction::SubType)CEvaluationNode::subType(node.getType());
      if (unsupportedFunctions.find(subtype) != unsupportedFunctions.end())
        {
          result.push_back(SBMLIncompatibility(2, node.getData().c_str(), objectType.c_str(), objectName.c_str()));
        }
    }
  const CEvaluationNode* pChild = dynamic_cast<const CEvaluationNode*>(node.getChild());
  while (pChild != NULL)
    {
      checkForUnsupportedFunctionCalls(*pChild, unsupportedFunctions, result, objectType, objectName);
      pChild = dynamic_cast<const CEvaluationNode*>(pChild->getSibling());
    }
}

/**
 * This method checks whether the given model contains any initial assignments.
 */
void CSBMLExporter::checkForInitialAssignments(const CCopasiDataModel& dataModel, std::vector<SBMLIncompatibility>& result)
{
  const CModel* pModel = dataModel.getModel();
  if (pModel != NULL)
    {
      // check for rules
      const CCopasiVectorNS<CCompartment>& compartments = pModel->getCompartments();
      CCopasiVectorNS<CCompartment>::const_iterator compIt = compartments.begin(), compEndit = compartments.end();
      while (compIt != compEndit)
        {
          if ((*compIt)->getInitialExpression() == "")
            {
              result.push_back(SBMLIncompatibility(5, "Compartment", (*compIt)->getObjectName().c_str()));
            }
          ++compIt;
        }

      const CCopasiVector<CMetab>& metabs = pModel->getMetabolites();
      CCopasiVector<CMetab>::const_iterator metabIt = metabs.begin(), metabEndit = metabs.end();
      while (metabIt != metabEndit)
        {
          if ((*metabIt)->getInitialExpression() == "")
            {
              result.push_back(SBMLIncompatibility(5, "Metabolite", (*metabIt)->getObjectName().c_str()));
            }
          ++metabIt;
        }

      const CCopasiVectorN<CModelValue>& parameters = pModel->getModelValues();
      CCopasiVectorN<CModelValue>::const_iterator mvIt = parameters.begin(), mvEndit = parameters.end();
      while (mvIt != mvEndit)
        {
          if ((*mvIt)->getInitialExpression() == "")
            {
              result.push_back(SBMLIncompatibility(5, "Parameter", (*mvIt)->getObjectName().c_str()));
            }
          ++mvIt;
        }
    }
}

/**
 * Create all function definitions.
 */
void CSBMLExporter::createFunctionDefinitions(CCopasiDataModel& dataModel)
{
  // TODO make sure the list of used functions is filled before this is
  // called
  // TODO make sure the mCOPASI2SBMLMap is up to date

  // find all indirectly called functions
  std::vector<CFunction*> usedFunctions = findUsedFunctions(this->mUsedFunctions, dataModel.getFunctionList());
  this->mUsedFunctions.clear();
  this->mUsedFunctions.insert(usedFunctions.begin(), usedFunctions.end());

  // remove the function calls that have been created by copasi solely for use
  // as a kinetic law term if it is no longer needed
  Model* pModel = this->mpSBMLDocument->getModel();
  assert(pModel);
  if (pModel == NULL) fatalError();
  std::map<SBase*, const CCopasiObject*> sbml2copasiMap;
  std::map<const CCopasiObject*, SBase*>::iterator mapIt = this->mCOPASI2SBMLMap.begin();
  std::map<const CCopasiObject*, SBase*>::iterator mapEndit = this->mCOPASI2SBMLMap.end();
  while (mapIt != mapEndit)
    {
      sbml2copasiMap.insert(std::make_pair(mapIt->second, mapIt->first));
      ++mapIt;
    }
  std::set<CFunction*> unusedFunctions;
  unsigned int i = i, iMax = pModel->getNumFunctionDefinitions();
  std::map<SBase*, const CCopasiObject*>::iterator mapPos;
  std::set<std::string> toRemove;
  while (i < iMax)
    {
      FunctionDefinition* pFunDef = pModel->getFunctionDefinition(i);
      if (pFunDef != NULL)
        {
          mapPos = sbml2copasiMap.find(pFunDef);
          if (mapPos != sbml2copasiMap.end())
            {
              CFunction* pFun = dynamic_cast<CFunction*>(const_cast<CCopasiObject*>(mapPos->second));
              if (pFun != NULL && this->mUsedFunctions.find(pFun) == this->mUsedFunctions.end())
                {
                  // the function exists in the model, but it is not used in any
                  // expression
                  if (pFun->getObjectName().find("function_4_") == 0)
                    {
                      // store the function definition that is to be removed
                      toRemove.insert(pFunDef->getId());
                    }
                  else
                    {
                      // those need to be stored in a separate list since we also
                      // need to find indirectly called functions
                      unusedFunctions.insert(pFun);
                    }
                }
            }
        }
      ++i;
    }
  // find all indirectly called functions for the unused functions
  std::vector<CFunction*> functionsVect = findUsedFunctions(unusedFunctions, dataModel.getFunctionList());
  usedFunctions.insert(usedFunctions.end(), functionsVect.begin(), functionsVect.end());
  // reset the used functions set
  this->mUsedFunctions.clear();
  this->mUsedFunctions.insert(usedFunctions.begin(), usedFunctions.end());

  // now we remove the function definitions from the SBML model
  std::set<std::string>::iterator toRemoveIt = toRemove.begin();
  std::set<std::string>::iterator toRemoveEndit = toRemove.end();
  while (toRemoveIt != toRemoveEndit)
    {
      FunctionDefinition* pFunDef = pModel->getFunctionDefinition(*toRemoveIt);
      if (pFunDef != NULL)
        {
          mapPos = sbml2copasiMap.find(pFunDef);
          if (mapPos != sbml2copasiMap.end() &&
              this->mUsedFunctions.find(dynamic_cast<CFunction*>(const_cast<CCopasiObject*>(mapPos->second))) == this->mUsedFunctions.end())
            {
              pModel->getListOfFunctionDefinitions()->remove(*toRemoveIt);
            }
        }
      ++toRemoveIt;
    }

  // order the remaining function definitions
  // remove duplicates from the vector, always keep the last one
  std::vector<CFunction*>::reverse_iterator reverseIt = usedFunctions.rbegin(), reverseEndit = usedFunctions.rend();
  functionsVect.clear();
  while (reverseIt != reverseEndit)
    {
      if (std::find(functionsVect.begin(), functionsVect.end(), *reverseIt) == functionsVect.end())
        {
          functionsVect.insert(functionsVect.begin(), *reverseIt);
        }
      ++reverseIt;
    }

  // remove all existing function definitions from the list
  while (pModel->getNumFunctionDefinitions() != 0)
    {
      pModel->getListOfFunctionDefinitions()->remove(0);
    }

  std::vector<CFunction*>::iterator it = functionsVect.begin(), endit = functionsVect.end();
  while (it != endit)
    {
      if (*it != NULL)
        {
          createFunctionDefinition(**it, dataModel);
        }
    }
}

/**
 * Create the SBML function definition from the given COPASI function.
 */
void CSBMLExporter::createFunctionDefinition(CFunction& function, const CCopasiDataModel& dataModel)
{
  // check the expression
  std::map<const CCopasiObject*, SBase*>::iterator pos = this->mCOPASI2SBMLMap.find(&function);
  FunctionDefinition* pFunDef = NULL;
  if (pos != this->mCOPASI2SBMLMap.end())
    {
      pFunDef = dynamic_cast<FunctionDefinition*>(pos->second);
      assert(pFunDef);
    }
  else
    {
      pFunDef = this->mpSBMLDocument->getModel()->createFunctionDefinition();
      pFunDef->setName(function.getObjectName());
      std::string id = CSBMLExporter::createUniqueId(this->mIdMap, "function_");
      this->mIdMap.insert(std::make_pair(id, pFunDef));
      pFunDef->setId(id);
      function.setSBMLId(id);
      this->mCOPASI2SBMLMap[&function] = pFunDef;
    }
  if (function.getRoot() == NULL)
    {
      std::string errorMessage = std::string("Can not export function");
      errorMessage += function.getObjectName();
      errorMessage += std::string(". Function does not have a valid root node.");
      CCopasiMessage(CCopasiMessage::EXCEPTION, errorMessage.c_str());
    }
  else
    {
      std::vector<SBMLIncompatibility> result;
      CSBMLExporter::isExpressionSBMLCompatible(function, dataModel, this->mSBMLLevel, this->mSBMLVersion, result);
      if (result.empty())
        {
          ASTNode* pFunNode = function.getRoot()->toAST();
          // go through the AST tree and replace all function call nodes with with a call to the sbml id
          ASTNode* pLambda = new ASTNode(AST_LAMBDA);
          // add the parameters to the function definition
          const CFunctionParameters& funParams = function.getVariables();
          unsigned int i, iMax = funParams.size();
          ASTNode* pParamNode = NULL;
          for (i = 0; i < iMax;++i)
            {
              pParamNode = new ASTNode(AST_NAME);
              pParamNode->setName(funParams[i]->getObjectName().c_str());
              pLambda->addChild(pParamNode);
            }
          pLambda->addChild(pFunNode);
          pFunDef->setMath(pLambda);
        }
      else
        {
          this->mIncompatibilities.insert(this->mIncompatibilities.end(), result.begin(), result.end());
          CCopasiMessage::CCopasiMessage(CCopasiMessage::EXCEPTION, MCSBML + 28, function.getObjectName().c_str());
        }
    }
}

/**
 * Export the model to SBML.
 * The SBML model is returned as a string. In case of an error, an
 * empty string is returned.
 */
const std::string CSBMLExporter::exportModelToString(CCopasiDataModel& dataModel, unsigned int sbmlLevel, unsigned int sbmlVersion)
{
  this->mSBMLLevel = sbmlLevel;
  this->mSBMLVersion = sbmlVersion;
  mHandledSBMLObjects.clear();
  createSBMLDocument(dataModel);
  // export the model to a string
  if (this->mpSBMLDocument == NULL) return std::string();
  removeUnusedObjects(dataModel);
  SBMLWriter* writer = new SBMLWriter();

  writer->setProgramName("COPASI");
  writer->setProgramVersion(dataModel.getVersion()->getVersion().c_str());

  char* d = writer->writeToString(this->mpSBMLDocument);
  std::string returnValue = d;
  if (d) free(d);
  pdelete(writer);
  return returnValue;
}

void CSBMLExporter::createSBMLDocument(CCopasiDataModel& dataModel)
{
  const SBMLDocument* pOldSBMLDocument = dataModel.getCurrentSBMLDocument();
  if (pOldSBMLDocument == NULL)
    {
      this->mpSBMLDocument = new SBMLDocument();
    }
  else
    {
      this->mpSBMLDocument = dynamic_cast<SBMLDocument*>(pOldSBMLDocument->clone());
    }
  if (this->mpSBMLDocument == NULL) fatalError();
  // update the copasi2sbmlmap
  updateCOPASI2SBMLMap(dataModel);
  // create units, compartments, species, parameters, reactions, initial
  // assignment, assignments, (event) and function definitions
  createUnits(dataModel);
  createCompartments(dataModel);
  createMetabolites(dataModel);
  createParameters(dataModel);
  // only export initial assignments for Level 2 Version 2 and above
  if (this->mSBMLLevel != 1 && !(this->mSBMLLevel == 2 && this->mSBMLVersion == 1))
    {
      createInitialAssignments(dataModel);
    }
  else
    {
      checkForInitialAssignments(dataModel, this->mIncompatibilities);
    }
  createRules(dataModel);
  createReactions(dataModel);
  createEvents(dataModel);
  // find all used functions
  if (this->mSBMLLevel != 1)
    {
      createFunctionDefinitions(dataModel);
    }
  else
    {
      // do sbml Level1 Voodoo
      // e.g. replace some of the unsupported nodes with workarounds
      convertToLevel1();
    }
  this->mpSBMLDocument->setLevelAndVersion(this->mSBMLLevel, this->mSBMLVersion);
}

const std::set<CEvaluationNodeFunction::SubType> CSBMLExporter::createUnsupportedFunctionTypeSet(unsigned int sbmlLevel)
{
  std::set<CEvaluationNodeFunction::SubType> unsupportedFunctionTypes;
  if (sbmlLevel == 1)
    {
      // TODO check what to do with all the functions that are not supported by
      // TODO SBML Level 1
      // TODO Also take care of other unsupported nodes in L1
      // TODO (inf,nan,pi,exponentiale)
      // TODO root has to be converted to pow
      // TODO do we actually have root???
      unsupportedFunctionTypes.insert(CEvaluationNodeFunction::SEC);
      unsupportedFunctionTypes.insert(CEvaluationNodeFunction::CSC);
      unsupportedFunctionTypes.insert(CEvaluationNodeFunction::COT);
      unsupportedFunctionTypes.insert(CEvaluationNodeFunction::SINH);
      unsupportedFunctionTypes.insert(CEvaluationNodeFunction::COSH);
      unsupportedFunctionTypes.insert(CEvaluationNodeFunction::TANH);
      unsupportedFunctionTypes.insert(CEvaluationNodeFunction::SECH);
      unsupportedFunctionTypes.insert(CEvaluationNodeFunction::CSCH);
      unsupportedFunctionTypes.insert(CEvaluationNodeFunction::COTH);
      unsupportedFunctionTypes.insert(CEvaluationNodeFunction::ARCSINH);
      unsupportedFunctionTypes.insert(CEvaluationNodeFunction::ARCCOSH);
      unsupportedFunctionTypes.insert(CEvaluationNodeFunction::ARCTANH);
      unsupportedFunctionTypes.insert(CEvaluationNodeFunction::ARCSECH);
      unsupportedFunctionTypes.insert(CEvaluationNodeFunction::ARCCSCH);
      unsupportedFunctionTypes.insert(CEvaluationNodeFunction::ARCCOTH);
      unsupportedFunctionTypes.insert(CEvaluationNodeFunction::RNORMAL);
      unsupportedFunctionTypes.insert(CEvaluationNodeFunction::RUNIFORM);
    }
  else
    {
      unsupportedFunctionTypes.insert(CEvaluationNodeFunction::RNORMAL);
      unsupportedFunctionTypes.insert(CEvaluationNodeFunction::RUNIFORM);
    }
  return unsupportedFunctionTypes;
}

/**
 * Export the model to SBML.
 * The model is written to the file given by filename.
 * If the export fails, false is returned.
 */
bool CSBMLExporter::exportModel(CCopasiDataModel& dataModel, const std::string& filename, unsigned int sbmlLevel, unsigned int sbmlVersion, bool overwrite)
{
  bool success = true;
  /* create a string that represents the SBMLDocument */
  std::string str = this->exportModelToString(dataModel, sbmlLevel, sbmlVersion);
  if (!str.empty())
    {
      /* check if the file already exisits.
         If yes, write if overwrite is true,
         else create an appropriate  CCopasiMessage. */
      std::ifstream testInfile(utf8ToLocale(filename).c_str(), std::ios::in);
      if (testInfile && !overwrite)
        {
          // create a CCopasiMessage with the appropriate error
          CCopasiMessage(CCopasiMessage::ERROR, MCDirEntry + 1, filename.c_str());
          return false;
        }
      /* write the document to a file */
      std::ofstream outfile(utf8ToLocale(filename).c_str(), std::ios::out | std::ios::trunc);
      outfile << str;
      outfile.close();
    }
  else
    {
      /* if no SBMLDocument could be created return false */
      success = false;
    }
  return success;
}

/**
 * Checks whether the given data model can be exported to a certain version of SBML.
 * If it can be exported, the result vector will be empty, otherwise it will
 * contain a number of messages that specify why it can't be exported.
 */
const std::vector<SBMLIncompatibility> CSBMLExporter::isModelSBMLCompatible(const CCopasiDataModel& dataModel, int sbmlLevel, int sbmlVersion)
{
  const CModel* pModel = dataModel.getModel();
  std::vector<SBMLIncompatibility> result;
  if (pModel == NULL) return result;
  // general checks
  // check if there is a species with an ode rule that is in a nonfixed
  // compartment
  checkForODESpeciesInNonfixedCompartment(dataModel, result);

  // check if the model contains references to model entities that can not be
  // represented in SBML like the inital value of something as opposed to the
  // transient value
  // check if the model contains calls to functions that are not supported in
  // the given version of SBML
  CModelEntity::Status status;
  const CExpression* pExpression = NULL;
  std::set<std::string> usedFunctionNames;
  CCopasiVectorNS<CCompartment>::const_iterator compIt = pModel->getCompartments().begin();
  CCopasiVectorNS<CCompartment>::const_iterator compEndit = pModel->getCompartments().end();
  while (compIt != compEndit)
    {
      status = (*compIt)->getStatus();
      if (status == CModelEntity::ODE || status == CModelEntity::ASSIGNMENT)
        {
          pExpression = (*compIt)->getExpressionPtr();
          assert(pExpression != NULL);
          if (pExpression != NULL)
            {
              // check for unsupported object references and unsupported function
              // calls
              CSBMLExporter::isExpressionSBMLCompatible(*pExpression, dataModel, sbmlLevel, sbmlVersion, result);
              CSBMLExporter::findDirectlyUsedFunctions(pExpression->getRoot(), usedFunctionNames);
            }
        }
      pExpression = (*compIt)->getInitialExpressionPtr();
      if (pExpression != NULL)
        {
          // check for unsupported object references and unsupported function
          // calls
          CSBMLExporter::isExpressionSBMLCompatible(*pExpression, dataModel, sbmlLevel, sbmlVersion, result);
          CSBMLExporter::findDirectlyUsedFunctions(pExpression->getRoot(), usedFunctionNames);
        }
      ++compIt;
    }

  CCopasiVector<CMetab>::const_iterator metabIt = pModel->getMetabolites().begin();
  CCopasiVector<CMetab>::const_iterator metabEndit = pModel->getMetabolites().end();
  while (metabIt != metabEndit)
    {
      status = (*metabIt)->getStatus();
      if (status == CModelEntity::ODE || status == CModelEntity::ASSIGNMENT)
        {
          pExpression = (*metabIt)->getExpressionPtr();
          assert(pExpression != NULL);
          if (pExpression != NULL)
            {
              // check for unsupported object references and unsupported function
              // calls
              CSBMLExporter::isExpressionSBMLCompatible(*pExpression, dataModel, sbmlLevel, sbmlVersion, result);
              CSBMLExporter::findDirectlyUsedFunctions(pExpression->getRoot(), usedFunctionNames);
            }
        }
      pExpression = (*metabIt)->getInitialExpressionPtr();
      if (pExpression != NULL)
        {
          // check for unsupported object references and unsupported function
          // calls
          CSBMLExporter::isExpressionSBMLCompatible(*pExpression, dataModel, sbmlLevel, sbmlVersion, result);
          CSBMLExporter::findDirectlyUsedFunctions(pExpression->getRoot(), usedFunctionNames);
        }
      ++metabIt;
    }

  CCopasiVectorN<CModelValue>::const_iterator mvIt = pModel->getModelValues().begin();
  CCopasiVectorN<CModelValue>::const_iterator mvEndit = pModel->getModelValues().end();
  while (mvIt != mvEndit)
    {
      status = (*mvIt)->getStatus();
      if (status == CModelEntity::ODE || status == CModelEntity::ASSIGNMENT)
        {
          pExpression = (*mvIt)->getExpressionPtr();
          assert(pExpression != NULL);
          if (pExpression != NULL)
            {
              // check for unsupported object references and unsupported function
              // calls
              CSBMLExporter::isExpressionSBMLCompatible(*pExpression, dataModel, sbmlLevel, sbmlVersion, result);
              CSBMLExporter::findDirectlyUsedFunctions(pExpression->getRoot(), usedFunctionNames);
            }
        }
      pExpression = (*mvIt)->getInitialExpressionPtr();
      if (pExpression != NULL)
        {
          // check for unsupported object references and unsupported function
          // calls
          CSBMLExporter::isExpressionSBMLCompatible(*pExpression, dataModel, sbmlLevel, sbmlVersion, result);
          CSBMLExporter::findDirectlyUsedFunctions(pExpression->getRoot(), usedFunctionNames);
        }
      ++mvIt;
    }

  // since all kinetic laws in COPASI are simple function calls and the
  // arguments to the call can only be parameters or species, the expressions
  // in kinetic laws are always automatically valid for SBML export
  // However the functions that are called must not be valid

  // check all functions that are used if they contain invalid function calls

  // level dependent checks
  CCopasiVectorN<CEvent>::const_iterator eventIt, eventEndit;
  switch (sbmlLevel)
    {
    case 2:
      // check all events
      eventIt = pModel->getEvents().begin();
      eventEndit = pModel->getEvents().end();
      while (eventIt != eventEndit)
        {
          // TODO add code to check event expressions
          ++eventIt;
        }
      switch (sbmlVersion)
        {
        case 1:
          CSBMLExporter::isModelSBMLL2V1Compatible(dataModel, result);
          break;
        case 2:
        case 3:
          CSBMLExporter::isModelSBMLL2V3Compatible(dataModel, result);
          break;
        default:
          result.push_back(SBMLIncompatibility(6, sbmlLevel, sbmlVersion));
          break;
        }
      break;
    case 1:
      CSBMLExporter::isModelSBMLL1Compatible(dataModel, result);
      break;
    default:
      result.push_back(SBMLIncompatibility(6, sbmlLevel, sbmlVersion));
      break;
    }
  return result;
}

void CSBMLExporter::createEvents(CCopasiDataModel& dataModel)
{
  if (this->mSBMLLevel == 1)
    {
      CSBMLExporter::checkForEvents(dataModel, this->mIncompatibilities);
    }
  else
    {
      if (dataModel.getModel() == 0) return;
      const CCopasiVectorN<CEvent>& events = dataModel.getModel()->getEvents();
      CCopasiVectorN<CEvent>::const_iterator it = events.begin(), endit = events.end();
      while (it != endit)
        {
          createEvent(**it);
          ++it;
        }
    }
}

void CSBMLExporter::createEvent(CEvent& /*event*/)
{
  // TODO once events are functional, we create them here
}

void CSBMLExporter::checkForEvents(const CCopasiDataModel& dataModel, std::vector<SBMLIncompatibility>& result)
{
  if (dataModel.getModel() != NULL && dataModel.getModel()->getEvents().size() > 0)
    {
      result.push_back(SBMLIncompatibility(7));
    }
}

void CSBMLExporter::updateCOPASI2SBMLMap(const CCopasiDataModel& dataModel)
{
  // TODO make sure the idMap is already uptodate
  // go through the existing map and create a new one with all SBML
  // objects updated with objects from the copied
  // model
  this->mCOPASI2SBMLMap.clear();
  std::map<CCopasiObject*, SBase*>::const_iterator it = dataModel.getCopasi2SBMLMap().begin();
  std::map<CCopasiObject*, SBase*>::const_iterator endit = dataModel.getCopasi2SBMLMap().end();
  while (it != endit)
    {
      std::map<std::string, const SBase*>::iterator pos = this->mIdMap.find(it->second->getId());
      if (pos != this->mIdMap.end())
        {
          this->mCOPASI2SBMLMap.insert(std::make_pair(it->first, const_cast<SBase*>(pos->second)));
        }
      ++it;
    }
}

KineticLaw* CSBMLExporter::createKineticLaw(const CReaction& reaction, CCopasiDataModel& /*dataModel*/)
{
  KineticLaw* pKLaw = NULL;
  if (!pKLaw)
    {
      /* create a new KineticLaw */
      pKLaw = new KineticLaw();
    }
  /* if the copasi CFunction specifies a mass-action kinetic */
  ASTNode* pNode = NULL;
  if (reaction.getFunction()->getType() == CFunction::MassAction)
    {
      const CMassAction cMassAction = *static_cast<const CMassAction*>(reaction.getFunction());
      /* create the ASTNode that multiplies all substrates with the first
      ** kinetic constant.
      */
      pNode = new ASTNode(AST_TIMES);

      ASTNode* parameterNode1 = new ASTNode(AST_NAME);
      std::string parameterName1;
      if (reaction.isLocalParameter(0))
        {
          parameterName1 = cMassAction.getVariables()[0]->getObjectName();
        }
      else
        {
          parameterName1 = reaction.getParameterMappings()[0][0];
          const CModelValue* pMV = dynamic_cast<const CModelValue*>(GlobalKeys.get(parameterName1));
          if (!pMV) fatalError();
          parameterName1 = pMV->getSBMLId();
        }
      parameterNode1->setName(parameterName1.c_str());
      pNode->addChild(parameterNode1);
      pNode->addChild(CSBMLExporter::createTimesTree(reaction.getChemEq().getSubstrates()));
      /* if the reaction is reversible, create the ASTNode tree that
      ** multiplies all products with the second kinetic constant and
      ** subtract this tree from the tree of the forward reaction.
      */
      if (cMassAction.isReversible() == TriTrue)
        {
          ASTNode* pBackwardNode = new ASTNode(AST_TIMES);

          ASTNode* pParameterNode2 = new ASTNode(AST_NAME);
          std::string parameterName2;
          if (reaction.isLocalParameter(2))
            {
              parameterName2 = cMassAction.getVariables()[2]->getObjectName();
            }
          else
            {
              parameterName2 = reaction.getParameterMappings()[2][0];
              const CModelValue* pMV = dynamic_cast<const CModelValue*>(GlobalKeys.get(parameterName2));
              if (!pMV) fatalError();
              parameterName2 = pMV->getSBMLId();
            }
          pParameterNode2->setName(parameterName2.c_str());
          pBackwardNode->addChild(pParameterNode2);

          pBackwardNode->addChild(CSBMLExporter::createTimesTree(reaction.getChemEq().getProducts()));
          ASTNode* pTempNode = new ASTNode(AST_MINUS);
          pTempNode->addChild(pNode);
          pTempNode->addChild(pBackwardNode);
          pNode = pTempNode;
        }
    }
  /* if the copasi CFunction does not specify a mass-action kinetic, it is a
  ** CKinFunction */
  else
    {
      CEvaluationTree* pTree = new CEvaluationTree("NoName", NULL, CEvaluationTree::Expression);
      CEvaluationNodeCall* pTmpRoot = new CEvaluationNodeCall(CEvaluationNodeCall::FUNCTION, reaction.getFunction()->getObjectName());
      const std::vector< std::vector<std::string> >& parameterMappings = reaction.getParameterMappings();
      unsigned int i, iMax = parameterMappings.size();
      for (i = 0;i < iMax;++i)
        {
          CEvaluationNodeObject* pObjectNode = new CEvaluationNodeObject(CEvaluationNodeObject::ANY, "<" + GlobalKeys.get(parameterMappings[i][0])->getCN() + ">");
          pTmpRoot->addChild(pObjectNode);
        }
      pTree->setRoot(pTmpRoot);
      pTree->compile();
      // in copasi kinetic laws are always function calls and the function
      // arguments are either references to species or parameters.
      // therefore we don't need to look for directly called functions here
      this->mUsedFunctions.insert(const_cast<CFunction*>(reaction.getFunction()));
      pNode = pTmpRoot->toAST();
      pdelete(pTree);
    }

  /*
  ** If the reaction takes place in a single compartment, the rate law has
  ** to be converted from concentration/time to substance/time by
  ** multiplying the rate law with the volume of the compartment.
  */
  if (reaction.getCompartmentNumber() == 1)
    {
      const CCompartment& compartment = reaction.getLargestCompartment();
      // check if the importer has added a division by the volume
      // if so remove it instead of multiplying again
      ASTNode* pTNode = CSBMLExporter::isDividedByVolume(pNode, compartment.getSBMLId());
      if (pTNode)
        {
          if (pTNode->getNumChildren() == 0)
            {
              fatalError();
            }
          if (pTNode->getNumChildren() == 1)
            {
              ASTNode* pTmp = static_cast<ConverterASTNode*>(pTNode)->removeChild(0);
              delete pTNode;
              pTNode = pTmp;
            }
          delete pNode;
          pNode = pTNode;
        }
      else
        {
          pTNode = new ASTNode(AST_TIMES);
          ASTNode* pVNode = new ASTNode(AST_NAME);
          pVNode->setName(compartment.getSBMLId().c_str());
          pTNode->addChild(pVNode);
          pTNode->addChild(pNode);
          pNode = pTNode;
        }
    }

  pKLaw->setMath(pNode);
  delete pNode;
  /* add the parameters */
  unsigned int counter;
  for (counter = 0; counter < reaction.getFunctionParameters().size(); counter++)
    {
      const CFunctionParameter* para = reaction.getFunctionParameters()[counter];
      if (para->getUsage() == CFunctionParameter::PARAMETER)
        {
          // only create a parameter if it is a local parameter,
          // otherwise the parameter already has been created
          if (reaction.isLocalParameter(counter))
            {
              Parameter sbmlPara = Parameter();

              std::string parameterKey = reaction.getParameterMappings()[counter][0];

              sbmlPara.setId(para->getObjectName().c_str());
              double value = reaction.getParameterValue(para->getObjectName());
              // if the value is NaN, leave the parameter value unset.
              if (!isnan(value))
                {
                  sbmlPara.setValue(value);
                }
              pKLaw->addParameter(&sbmlPara);
            }
        }
    }
  return pKLaw;
}

ASTNode* CSBMLExporter::isDividedByVolume(const ASTNode* pRootNode, const std::string& compartmentId)
{
  ASTNode* pResult = NULL;
  if (pRootNode->getType() == AST_DIVIDE || pRootNode->getType() == AST_TIMES)
    {
      ASTNode* pTmpResultNode = new ConverterASTNode(pRootNode->getType());
      unsigned int i, iMax = pRootNode->getNumChildren();
      bool found = false;
      for (i = 0; i < iMax;++i)
        {
          const ASTNode* pChild = pRootNode->getChild(i);
          if (pRootNode->getType() == AST_DIVIDE && pChild->getType() == AST_NAME && pChild->getName() == compartmentId)
            {
              found = true;
            }
          else if ((!found) && (pChild->getType() == AST_DIVIDE || pChild->getType() == AST_TIMES))
            {
              ASTNode* pSubResult = CSBMLExporter::isDividedByVolume(pChild, compartmentId);
              if (pSubResult != NULL)
                {
                  found = true;
                  if (pSubResult->getNumChildren() > 1)
                    {
                      pTmpResultNode->addChild(pSubResult);
                    }
                  else if (pSubResult->getNumChildren() == 1)
                    {
                      pTmpResultNode->addChild(static_cast<ASTNode*>(static_cast<ConverterASTNode*>(pSubResult)->removeChild(0)));
                      delete pSubResult;
                    }
                  else
                    {
                      delete pSubResult;
                    }
                }
              else
                {
                  pTmpResultNode->addChild(new ConverterASTNode(*pChild));
                }
            }
          else
            {
              pTmpResultNode->addChild(new ConverterASTNode(*pChild));
            }
        }
      if (found)
        {
          pResult = pTmpResultNode;
        }
      else
        {
          delete pTmpResultNode;
        }
    }
  return pResult;
}

CEvaluationNode* CSBMLExporter::createExpressionTree(const CFunction* const pFun, const std::vector<std::vector<std::string> >& arguments, const CCopasiDataModel& dataModel)
{
  if (!pFun || pFun->getVariables().size() != arguments.size()) fatalError();
  std::map< std::string, std::string > parameterMap;
  unsigned int i, iMax = arguments.size();
  for (i = 0;i < iMax;++i)
    {
      if (arguments[i].size() != 1) fatalError(); // we can't have arrays here.
      const CCopasiObject* pObject = GlobalKeys.get(arguments[i][0]);
      if (!pObject) fatalError();
      parameterMap[pFun->getVariables()[i]->getObjectName()] = "<" + pObject->getCN() + ">";
    }
  return CSBMLExporter::createExpressionTree(pFun->getRoot(), parameterMap, dataModel);
}

CEvaluationNode* CSBMLExporter::createExpressionTree(const CEvaluationNode* const pNode, const std::map<std::string, std::string>& parameterMap, const CCopasiDataModel& dataModel)
{
  if (dataModel.getModel() == NULL) return NULL;
  if (!pNode) fatalError();
  CEvaluationNode* pResultNode = NULL;
  const CFunction* pFun = NULL;
  std::map<std::string, std::string>::const_iterator pos;
  std::vector<std::vector<std::string> > arguments;
  const CEvaluationNode* pChildNode = NULL;
  std::vector<const CCopasiContainer*> containerList;
  containerList.push_back(dataModel.getModel());
  //const CCopasiObject* pObject = NULL;
  switch (CEvaluationNode::type(pNode->getType()))
    {
    case CEvaluationNode::CALL:
      pFun = dynamic_cast<const CFunction*>(const_cast<CFunctionDB*>(dataModel.getFunctionList())->findFunction(pNode->getData()));
      assert(pFun);
      pResultNode = CSBMLExporter::createExpressionTree(pFun->getRoot(), parameterMap, dataModel);
      break;
    case CEvaluationNode::VARIABLE:
      // replace the variable node with an object node
      pos = parameterMap.find(pNode->getData());
      if (pos == parameterMap.end()) fatalError();
      pResultNode = new CEvaluationNodeObject(CEvaluationNodeObject::ANY, pos->second);
      break;
    case CEvaluationNode::NUMBER:
      pResultNode = new CEvaluationNodeNumber((CEvaluationNodeNumber::SubType)CEvaluationNode::subType(pNode->getType()), pNode->getData());
      break;
    case CEvaluationNode::CONSTANT:
      pResultNode = new CEvaluationNodeConstant((CEvaluationNodeConstant::SubType)CEvaluationNode::subType(pNode->getType()), pNode->getData());
      break;
    case CEvaluationNode::OPERATOR:
      pResultNode = new CEvaluationNodeOperator((CEvaluationNodeOperator::SubType)CEvaluationNode::subType(pNode->getType()), pNode->getData());
      break;
    case CEvaluationNode::CHOICE:
      pResultNode = new CEvaluationNodeChoice((CEvaluationNodeChoice::SubType)CEvaluationNode::subType(pNode->getType()), pNode->getData());
      break;
    case CEvaluationNode::LOGICAL:
      pResultNode = new CEvaluationNodeLogical((CEvaluationNodeLogical::SubType)CEvaluationNode::subType(pNode->getType()), pNode->getData());
      break;
    default:
      fatalError();
      break;
    }
  pChildNode = static_cast<const CEvaluationNode*>(pNode->getChild());
  while (pChildNode)
    {
      pResultNode->addChild(CSBMLExporter::createExpressionTree(pChildNode, parameterMap, dataModel));
      pChildNode = static_cast<const CEvaluationNode*>(pChildNode->getSibling());
    }
  assert(pResultNode);
  return pResultNode;
}

/**
 * This method creates an ASTNode tree where all the species specified in
 * the given vector are multiplied. This is used to create the mass action
 * kinetic law.
 */
ASTNode* CSBMLExporter::createTimesTree(const CCopasiVector<CChemEqElement >& vect, unsigned int pos)
{
  ASTNode* pNode = NULL;
  double multiplicity = vect[pos]->getMultiplicity();
  if (pos == vect.size() - 1)
    {
      pNode = new ASTNode(AST_NAME);
      const CMetab* pMetab = vect[pos]->getMetabolite();
      assert(pMetab);

      pNode->setName(pMetab->getSBMLId().c_str());
      /* if the stoichiometry is not 1.0, we have to add it to the exponent */
      if (multiplicity != 1.0)
        {
          ASTNode* pTmpNode1 = new ASTNode(AST_POWER);
          ASTNode* pTmpNode2 = new ASTNode(AST_REAL);
          pTmpNode2->setValue(multiplicity);
          pTmpNode1->addChild(pNode);
          pTmpNode1->addChild(pTmpNode2);
          pNode = pTmpNode1;
        }
    }
  else
    {
      pNode = new ASTNode(AST_TIMES);
      ASTNode* pChild = new ASTNode(AST_NAME);
      const CMetab* pMetab = vect[pos]->getMetabolite();
      assert(pMetab);

      pChild->setName(pMetab->getSBMLId().c_str());

      /* if the stoichiometry is not 1.0, we have to add it to the exponent */
      if (multiplicity != 1.0)
        {
          ASTNode* pTmpNode1 = new ASTNode(AST_POWER);
          ASTNode* pTmpNode2 = new ASTNode(AST_REAL);
          pTmpNode2->setValue(multiplicity);
          pTmpNode1->addChild(pChild);
          pTmpNode1->addChild(pTmpNode2);
          pChild = pTmpNode1;
        }

      pNode->addChild(pChild);
      pNode->addChild(CSBMLExporter::createTimesTree(vect, pos + 1));
    }
  return pNode;
}

/**
 * Go through a CEvaluationNode base tree and return a list of
 * functions directly called in this tree.
 */
void CSBMLExporter::findDirectlyUsedFunctions(const CEvaluationNode* pRootNode, std::set<std::string>& result)
{
  if (pRootNode == NULL) return;
  if (CEvaluationNode::type(pRootNode->getType()) == CEvaluationNode::CALL)
    {
      result.insert(pRootNode->getData());
    }
  const CEvaluationNode* pChild = dynamic_cast<const CEvaluationNode*>(pRootNode->getChild());
  while (pChild != NULL)
    {
      CSBMLExporter::findDirectlyUsedFunctions(pChild, result);
      pChild = dynamic_cast<const CEvaluationNode*>(pChild->getSibling());
    }
}

const std::vector<CFunction*> CSBMLExporter::findUsedFunctions(std::set<CFunction*>& functions, CFunctionDB* pFunctionDB)
{
  std::vector<CFunction*> result;
  std::set<CFunction*>::iterator it = functions.begin();
  std::set<CFunction*>::iterator endit = functions.end();
  std::set<CFunction*> chain;
  while (it != endit)
    {
      chain.insert(*it);
      findUsedFunctions(*it, pFunctionDB, chain, result);
      chain.clear();
      ++it;
    }
  return result;
}

void CSBMLExporter::findUsedFunctions(const CFunction* pFunction, CFunctionDB *pFunDB, std::set<CFunction*>& chain, std::vector<CFunction*>& result)
{
  if (pFunction == NULL || pFunDB == NULL) return;
  std::set<std::string> usedFunctionNames;
  CSBMLExporter::findDirectlyUsedFunctions(pFunction->getRoot(), usedFunctionNames);
  std::set<CFunction*> usedFunctions = CSBMLExporter::createFunctionSetFromFunctionNames(usedFunctionNames, pFunDB);
  std::set<CFunction*>::iterator it = usedFunctions.begin();
  std::set<CFunction*>::iterator endit = usedFunctions.end();
  while (it != endit)
    {
      if (chain.find(*it) != chain.end())
        {
          // we have a loop
          CCopasiMessage::CCopasiMessage(CCopasiMessage::ERROR, MCSBML + 16);
        }
      else
        {
          result.push_back(*it);
          chain.insert(*it);
          findUsedFunctions(*it, pFunDB, chain, result);
          chain.erase(chain.find(*it));
        }
      ++it;
    }
}

const std::set<CFunction*> CSBMLExporter::createFunctionSetFromFunctionNames(const std::set<std::string>& names, CFunctionDB* pFunDB)
{
  std::set<CFunction*> result;
  if (pFunDB != NULL)
    {
      std::set<std::string>::const_iterator it = names.begin();
      std::set<std::string>::const_iterator endit = names.end();
      CFunction* pFun = NULL;
      while (it != endit)
        {
          pFun = dynamic_cast<CFunction*>(pFunDB->findFunction(*it));
          if (pFun == NULL)
            {
              CCopasiMessage::CCopasiMessage(CCopasiMessage::ERROR, MCSBML + 15, (*it).c_str());
            }
          else
            {
              result.insert(pFun);
            }
          ++it;
        }
    }
  return result;
}