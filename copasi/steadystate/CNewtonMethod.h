/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/steadystate/CNewtonMethod.h,v $
   $Revision: 1.1.1.1 $
   $Name:  $
   $Author: anuragr $ 
   $Date: 2004/10/26 15:18:02 $
   End CVS Header */

/**
 *  CNewtonMethod class.
 *  This class implements the enhanced Newton method which attempts to find the
 *  the steady state.
 *  
 *  Created for Copasi by Stefan Hoops 2002
 */

#ifndef COPASI_CNewtonMethod
#define COPASI_CNewtonMethod

#include "utilities/CMatrix.h"
#include "utilities/CVector.h"
#include "model/CState.h"

class CNewtonMethod : public CSteadyStateMethod
  {
    friend CSteadyStateMethod *
    CSteadyStateMethod::createSteadyStateMethod(CCopasiMethod::SubType subType);

    // Attributes
  private:
    enum NewtonReturnCode
    {
      found = 0,
      notFound,
      iterationLimitExceeded,
      dampingLimitExceeded,
      singularJacobian
    };

    bool mUseNewton;
    bool mUseIntegration;
    bool mUseBackIntegration;
    C_INT32 mIterationLimit;

    C_FLOAT64 mFactor;
    C_FLOAT64 mResolution;
    C_FLOAT64 mScaledResolution;
    C_INT mDimension;
    C_FLOAT64 mMaxrate;
    CVector< C_FLOAT64 > * mX;
    CVector< C_FLOAT64 > mH;
    CVector< C_FLOAT64 > mXold;
    CVector< C_FLOAT64 > mdxdt;
    //CMatrix< C_FLOAT64 > mJacobianX;
    C_INT * mIpiv;

    //CStateX mStateX;
    CStateX mInitialStateX;

    // Operations
  private:
    /**
     * Default constructor.
     * @param const CCopasiContainer * pParent (default: NULL)
     */
    CNewtonMethod(const CCopasiContainer * pParent = NULL);

  public:
    /**
     * Copy constructor.
     * @param "const CNewtonMethod &" src
     * @param const CCopasiContainer * pParent (default: NULL)
     */
    CNewtonMethod(const CNewtonMethod & src,
                  const CCopasiContainer * pParent = NULL);

    /**
     *  Destructor.
     */
    ~CNewtonMethod();

    /**
     * Load a list of parameters
     * @param "CReadConfig &" configBuffer
     * @param "CReadConfig::Mode" mode Default(CReadConfig::SEARCH)
     */
    virtual void load(CReadConfig & configBuffer,
                      CReadConfig::Mode mode = CReadConfig::SEARCH);

    /**
     * This instructs the method to calculate a the steady state
     * starting with the initialState given.
     * The steady state is returned in the object pointed to by steadyState.
     * @param CState & steadyState
     * @param const CState & initialState
     * @return CSteadyStateMethod::ReturnCode returnCode
     */
    virtual CSteadyStateMethod::ReturnCode
    processInternal();

  private:
    /**
     * This instructs the method to calculate a the steady state
     * starting with the initialState given.
     * The steady state is returned in the object pointed to by steadyState.
     * @param CState * steadyState
     * @param const CState * initialState
     * @return CNewtonMethod::NewtonReturnCode newtonReturnCode
     */
    CNewtonMethod::NewtonReturnCode processNewton();

    CNewtonMethod::NewtonReturnCode
    returnNewton(const CNewtonMethod::NewtonReturnCode & returnCode);

    bool isSteadyState();

    void cleanup();
  };
#endif // COPASI_CNewtonMethod
