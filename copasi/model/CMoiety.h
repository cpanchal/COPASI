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

/**
 *  CMoiety
 *
 *  New class created for Copasi by Stefan Hoops
 *  (C) Stefan Hoops 2001
 */

#ifndef COPASI_CMoiety
#define COPASI_CMoiety

#include <string>
#include <vector>

#include "copasi/model/CChemEqElement.h"
#include "copasi/utilities/CCopasiVector.h"
#include "copasi/report/CCopasiObjectReference.h"
class CMetab;

class CTotalNumberReference : public CCopasiObjectReference< C_FLOAT64 >
{
  /**
   * Hidden default constructor
   */
  CTotalNumberReference();

public:
  /**
   * Specific constructor
   * @param const std::string & name
   * @param const CCopasiContainer * pParent,
   * @param C_FLOAT64 & reference,
   */
  CTotalNumberReference(const std::string & name,
                        const CCopasiContainer * pParent,
                        C_FLOAT64 & reference);

  /**
   * Copy constructor
   * @param const CTotalNumberReference & src
   * @param const CCopasiContainer * pParent,
   */
  CTotalNumberReference(const CTotalNumberReference & src,
                        const CCopasiContainer * pParent);

  /**
   * Destructor
   */
  ~CTotalNumberReference();

  /**
   * Check whether a given object is a prerequisite for a context.
   * @param const CObjectInterface * pObject
   * @param const CMath::SimulationContextFlag & context
   * @param const CObjectInterface::ObjectSet & changedObjects
   * @return bool isPrerequisiteForContext
   */
  virtual bool isPrerequisiteForContext(const CObjectInterface * pObject,
                                        const CMath::SimulationContextFlag & context,
                                        const CObjectInterface::ObjectSet & changedObjects) const;
};

class CDependentNumberReference : public CCopasiObjectReference< C_FLOAT64 >
{
  /**
   * Hidden default constructor
   */
  CDependentNumberReference();

public:
  /**
   * Specific constructor
   * @param const std::string & name
   * @param const CCopasiContainer * pParent,
   * @param C_FLOAT64 & reference,
   */
  CDependentNumberReference(const std::string & name,
                            const CCopasiContainer * pParent,
                            C_FLOAT64 & reference);

  /**
   * Copy constructor
   * @param const CDependentNumberReference & src
   * @param const CCopasiContainer * pParent,
   */
  CDependentNumberReference(const CDependentNumberReference & src,
                            const CCopasiContainer * pParent);

  /**
   * Destructor
   */
  ~CDependentNumberReference();

  /**
   * Check whether a given object is a prerequisite for a context.
   * @param const CObjectInterface * pObject
   * @param const CMath::SimulationContextFlag & context
   * @param const CObjectInterface::ObjectSet & changedObjects
   * @return bool isPrerequisiteForContext
   */
  virtual bool isPrerequisiteForContext(const CObjectInterface * pObject,
                                        const CMath::SimulationContextFlag & context,
                                        const CObjectInterface::ObjectSet & changedObjects) const;
};

class CMoiety : public CCopasiContainer
{
  // Attributes
private:
  /**
   * The default conversion factor used if the moiety is not part of a model
   */
  static const C_FLOAT64 DefaultFactor;

  /**
   * The key of the moiety
   */
  std::string mKey; //By G

  /**
   *  Number of Particles of Moiety.
   */
  C_FLOAT64 mNumber;

  /**
   *  Initial Number of Particles of Moiety.
   */
  C_FLOAT64 mINumber;

  /**
   *  The total Amount of the Moiety.
   */
  C_FLOAT64 mIAmount;

  /**
   *  Vector of linear dependent CChemEqElement
   * @supplierCardinality 0..*
   */
  /** @dia:route 7,3; h,41.0337,110.831,46.5202,117.862,52.0066 */
  // CCopasiVector < CChemEqElement > mEquation;
  std::vector<std::pair< C_FLOAT64, CMetab * > > mEquation;

  /**
   * A pointer to the object for the initial total particle number
   */
  CTotalNumberReference *mpINumberReference;

  /**
   * A pointer to the object for the total particle number
   * This is used during events
   */
  CTotalNumberReference *mpNumberReference;

  /**
   * A pointer to the object for the dependent particle number
   */
  CDependentNumberReference *mpDNumberReference;

  /**
   * A pointer to the conversion factor between the particle number and the amount.
   */
  const C_FLOAT64 * mpConversionFactor;

  // Operations
public:
  /**
   * Default constructor
   * @param const std::string & name (default: "NoName")
   * @param const CCopasiContainer * pParent (default: NULL)
   */
  CMoiety(const std::string & name = "NoName",
          const CCopasiContainer * pParent = NULL);

  /**
   * Copy constructor
   * @param "const CMoiety &" src
   * @param const CCopasiContainer * pParent (default: NULL)
   */
  CMoiety(const CMoiety & src,
          const CCopasiContainer * pParent = NULL);

  /**
   *  Destructor
   */
  ~CMoiety();

  /**
   * Add a metabolite to a moiety
   * @param C_FLOAT64 value
   * @param CMetab * metabolite
   */
  void add(C_FLOAT64 value, CMetab * metabolite);

  /**
   *
   */
  void cleanup();

  /**
   * Refresh the initial total particle number
   */
  void refreshInitialValue();

  /**
   * Retrieve the object for the total particle number
   * @return CCopasiObject * initialValueReference
   */
  CCopasiObject * getInitialValueReference() const;

  /**
   * Refresh the total particle number
   */
  void refreshValue();

  /**
   * Retrieve the object for the total particle number
   * @return CCopasiObject * valueReference
   */
  CCopasiObject * getValueReference() const;

  /**
   * get the string representation of the moiety using the CMetabNameInterface
   */
  std::string getDescription(const CModel* model) const;

  /**
   * Retrieve and refresh the dependent number;
   * @return const C_FLOAT64 & dependentNumber
   */
  const C_FLOAT64 & dependentNumber();

  /**
   * Retrieve the dependent number;
   * @return const C_FLOAT64 & dependentNumber
   */
  const C_FLOAT64 & getDependentNumber() const;

  /**
   * Retrieve the object for the dependent particle number
   * @return CCopasiObject * totalNumberReference
   */
  CCopasiObject * getTotalNumberReference() const;

  /**
   * Retrieve the object for the dependent particle number
   * @return CCopasiObject * dependentNumberReference
   */
  CCopasiObject * getDependentNumberReference() const;

  /**
   *
   */
  C_FLOAT64 getNumber() const;

  /**
   *  Returns a string with the name of this compartment.
   *  @return std::string key
   */
  virtual const std::string & getKey() const; //By G

  /**
   * Sets the parent of the moiety;
   * @param const CCopasiContainer * pParent
   * @return bool success
   */
  virtual bool setObjectParent(const CCopasiContainer * pParent);

  /**
   * Refreshes the value of the dependent number
   */
  void refreshDependentNumber();

  /**
   * Retrieve the infix expression, which can be used to calculate the
   * total amount.
   * @return std::string expression
   */
  std::string getExpression() const;

  /**
   * Retrieve the total amount
   * @return const C_FLOAT64 & amount
   */
  const C_FLOAT64 & getAmount() const;

  /**
   * Refresh the total amount
   */
  void refreshAmount();

  /**
   * Retrieve the components of the total mass equation
   * @return const std::vector<std::pair< C_FLOAT64, CMetab * > > & equation
   */
  const std::vector<std::pair< C_FLOAT64, CMetab * > > & getEquation() const;

private:
  /**
   * Initialize the contained CCopasiObjects
   */
  void initObjects();

  /**
   * Initialize the number to amount conversion factor
   */
  void initConversionFactor();
};

#endif // COPASI_CMoiety
