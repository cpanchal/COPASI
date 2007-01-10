// Copyright © 2005 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

%{

#include "utilities/CCopasiMethod.h"  

%}


class CCopasiMethod : public CCopasiParameterGroup
{
  public:
    /**
     * Enumeration of the sub types of methods known to COPASI.
     */
    enum SubType
    {
      unset = 0,
      RandomSearch,
      RandomSearchMaster,
      SimulatedAnnealing,
      GeneticAlgorithm,
      EvolutionaryProgram,
      SteepestDescent,
      HybridGASA,
      GeneticAlgorithmSR,
      HookeJeeves,
      LevenbergMarquardt,
      NelderMead,
      SRES,
      Statistics,
      ParticleSwarm,
      Newton,
      deterministic,
      LSODAR,
      stochastic,
      hybrid,
      hybridLSODA,
      tauLeap,
      mcaMethodReder,
      scanMethod,
      lyapWolf,
      tssMethod,
      sensMethod,
#ifdef COPASI_SSA
      ssaMethod,
#endif // COPASI_SSA
#ifdef COPASI_EXTREMECURRENTS
      extremeCurrents,
#endif
      EFMAlgorithm
    };

    /**
     * Convert a SubTypeName to the matching enum value.
     * Returns CCopasiMethod::unset if no match is found.
     * @param (const std::string & subTypeName)
     * @return CCopasiMethod::SubType type
     */
    static
    CCopasiMethod::SubType TypeNameToEnum(const std::string & subTypeName);


    /**
     * Copy constructor
     * @param const CCopasiMethodr & src
     * @param const CCopasiContainer * pParent (default: NULL)
     */
    CCopasiMethod(const CCopasiMethod & src,
                  const CCopasiContainer * pParent = NULL);

    /**
     * Destructor
     */
    virtual ~CCopasiMethod();


    /**
     * Retrieve the type of the method
     * @return  const string & type
     */
    const CCopasiTask::Type & getType() const;

    /**
     * Retrieve the sub type of the method
     * @return CCopasiMethod::SubType & subType
     */
    const CCopasiMethod::SubType & getSubType() const;

%extend{
  static const std::string& getSubTypeName(const int& subType)
  {
    return CCopasiMethod::SubTypeName[subType];
  }
}

};


