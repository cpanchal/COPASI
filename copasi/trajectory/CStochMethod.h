/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/trajectory/CStochMethod.h,v $
   $Revision: 1.18 $
   $Name:  $
   $Author: ssahle $ 
   $Date: 2004/11/22 10:10:12 $
   End CVS Header */

#ifndef COPASI_CStochMethod
#define COPASI_CStochMethod

#define C_NUMBER long long int

#include <set>
#include <vector>
#include "CTrajectoryMethod.h"
#include "utilities/CDependencyGraph.h"

class CModel;
class CMetab;
class CTrajectoryProblem;

/**
 * Integer Balances  for internal use
 */
class CStochBalance
  {
  public:
    C_INT32 mIndex;
    C_INT32 mMultiplicity;
  };

class CRandom;

class CStochMethod : public CTrajectoryMethod
  {
    friend CTrajectoryMethod *
    CTrajectoryMethod::createTrajectoryMethod(CCopasiMethod::SubType subType,
        CTrajectoryProblem * pProblem);

  protected:

    /**
     * The propensities for the reactions
     */
    std::vector<C_FLOAT64> mAmu;

    /**
     * The sum of the propensities
     */
    C_FLOAT64 mA0;

    /**
     * The stored propensities for the reactions before the last update
     */
    std::vector<C_FLOAT64> mAmuOld;

    /**
     * The sum of the propensities before the last update
     */ 
    //C_FLOAT64 mA0Old;

    /**
     * Initialization.
     */
    virtual void initMethod(C_FLOAT64 start_time) = 0;

    /**
     * Do one iteration of the simulation
     * @return Current simulation time or -1 if error.
     */
    virtual C_FLOAT64 doSingleStep(C_FLOAT64 time, C_FLOAT64 endtime) = 0;

    /**
     * Calculate the propensities for all reactions
     * @return mFail
     * @see mFail
     */
    C_INT32 updatePropensities();

    /**
     * Calculate one of the propensities
     * @return mFail
     * @see mFail
     */
    C_INT32 calculateAmu(C_INT32 reaction_index);

    /**
     * Generate the index of a putative reaction.
     * @return The reaction index
     */
    C_INT32 generateReactionIndex();

    /**
     * Generate the putative time taken before any reaction takes place
     * @return The time before the reaction
     */
    C_FLOAT64 generateReactionTime();

    /**
     * Generate the putative time taken before a special reaction takes place
     * @return The time before the reaction
     */
    C_FLOAT64 generateReactionTime(C_INT32 reaction_index);

    /**
     * Update the particle numbers according to which reaction ocurred
     * @return mFail
     * @see mFail
     */
    C_INT32 updateSystemState(C_INT32 reaction_index);

    /**
    * Set up the dependency graph and the balances
    */
    void setupDependencyGraphAndBalances();

  private:

    /**
     *  This checks if a model is suitable for stochastic simulation.
     *  It returns a suggestion which method to use
     *  @param model The model to check
     *  @return 1: direct method, 2: next reaction method, -1: no stochastic simulation possible
     */
    static C_INT32 checkModel(CModel * pmodel);

    /**
     * Get the set of metabolites on which a given reaction depends.
     * @param reaction_index The index of the reaction being executed.
     * @return The set of metabolites depended on.
     */
    std::set<std::string> *getDependsOn(C_INT32 reaction_index);

    /**
     * Get the set of metabolites which change number when a given
     * reaction is executed.
     * @param reaction_index The index of the reaction being executed.
     * @return The set of affected metabolites.
     */
    std::set<std::string> *getAffects(C_INT32 reaction_index);

    /**
    * max number of single stochastic steps to do in one step()
    */
    C_INT32 mMaxSteps;

    /**
    * maximal increase of a particle number in one step.
    */
    C_INT32 mMaxBalance;

    /**
    * This is set to maxint - mMaxSteps*mMaxBalance
    */
    C_NUMBER mMaxIntBeforeStep;

  protected:
    /**
     *  Default constructor.
     * @param const CCopasiContainer * pParent (default: NULL)
     */
    CStochMethod(const CCopasiContainer * pParent = NULL);

  public:
    /**
     *  Copy constructor.
     *  @param "const CStochMethod &" src
     */
    CStochMethod(const CStochMethod & src);

    /**
     *  Destructor.
     */
    ~CStochMethod();

    /**
     *  Chooses a stochastic method adequate for the problem
     */
    static CStochMethod *
    createStochMethod(CTrajectoryProblem * pProblem = NULL);

    /**
     *  This instructs the method to calculate a time step of deltaT
     *  starting with the current state, i.e., the result of the previous
     *  step.
     *  The new state (after deltaT) is expected in the current state.
     *  The return value is the actual timestep taken.
     *  @param "const double &" deltaT
     *  @return "const double" actualDeltaT
     */
    const double step(const double & deltaT);

    /**
     *  This instructs the method to calculate a time step of deltaT
     *  starting with the initialState given.
     *  Also initialization of the method is done.
     *  The new state (after deltaT) is expected in the current state.
     *  The return value is the actual timestep taken.
     *  @param "double &" deltaT
     *  @param "const CState *" initialState
     *  @return "const double" actualDeltaT
     */
    const double step(const double & deltaT, const CState * initialState);

  protected:

    /**
     * The random number generator
     */
    CRandom *mRandomGenerator;

    /**
     * A pointer to the instance of CModel being used.
     */
    const CModel *mpModel;

    /**
     * The graph of reactions and their dependent reactions. When a reaction is
     * executed, the propensities for each of its dependents must be updated.
     */
    CDependencyGraph mDG;

    /**
     * The balances of the reactions as integers
     */
    std::vector < std::vector <CStochBalance> > mLocalBalances;

    /**
     * the substrates of each reaction with their multiplicities
     */
    std::vector < std::vector <CStochBalance> > mLocalSubstrates;

    /**
     * The particle numbers
     */
    std::vector <C_NUMBER> mNumbers;

    unsigned C_INT32 mNumReactions;
    unsigned C_INT32 mNumNumbers;
  };

//#include "CStochDirectMethod.h"
//#include "CStochNextReactionMethod.h"

#endif // COPASI_CStochMethod
