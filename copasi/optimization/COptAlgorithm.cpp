/**
 *  File name: COptAlgorithm.cpp
 *
 *  Programmer: Yongqun He 
 *  Contact email: yohe@vt.edu
 *  Purpose: This is the implementation (.cpp file) of the COptAlgorithm class. 
 *           It contains the designed abstract interaction between a simulation
 *           subsystem and optimization algorithms. This base class defines
 *           methods by which the simulation subsystem accesses several 
 *           optimization algorithms. 
 */
#include <math.h>
#include "COptAlgorithm.h"


// Default constructor
COptAlgorithm::COptAlgorithm()
{
  mOptProblem = NULL;
  mParameters = NULL; 
  mParameterNum = 0;        // the number of parameters
  mParameterMin = NULL;     // the minimum values of parameters
  mParameterMax = NULL;     // the maximum values of parameters
  //if (mOptAlgmParams != ; //vector of COptAlgorithmParameter object
 
  mMethodVersion = "0";
  mMethodName = "Unknown";
  mBounds = false;
}

//YOHE: seems "virtual" cannot be outside of class declaration
COptAlgorithm::~COptAlgorithm()
{ }

// Initialization of private variables
bool COptAlgorithm::initialize(void)
{
  
  if (!mOptAlgmParams.empty())
    mOptAlgmParams.clear();

  for (int i=0; i<mOptAlgmParams.size(); i++)
    {
      //    mOptAlgmParams[i] =  
    }

  return true;
}

//clean up mem
int COptAlgorithm::cleanup(void)
{
  mOptProblem = NULL;
  mParameters = NULL; 
  mParameterMin = NULL;    
  mParameterMax = NULL;

  return 0;
}


// set the number of method parameters
void COptAlgorithm::setMethodParameterNumber(int aNum)
{
  mParameterNum = aNum;
}

// get method parameter number
int COptAlgorithm::getMethodParameterNumber(void)
{
  return mParameterNum;
}


// get method parameters
vector <COptAlgorithmParameter> * COptAlgorithm::getMethodParameters()
{
  return &mOptAlgmParams;
}


// set method parameter value
void COptAlgorithm::setMethodParameterValue(int i, double value)
{
  mOptAlgmParams[i].setValue(value);
}

// get method parameter value
double COptAlgorithm::getMethodParameterValue(int i)
{
  return mOptAlgmParams[i].getValue();
}

// get method parameter name
string COptAlgorithm::getMethodParameterName(int i)
{
  return mOptAlgmParams[i].getName();
}

// get method name
string COptAlgorithm::getMethodName(void)
{
  return mMethodName;
}

// get method version
string COptAlgorithm::getMethodVersion(void)
{
  return mMethodVersion;
}

// Returns True if this method is capable of handling adjustable parameter boundary
// constraints, False otherwise
bool COptAlgorithm::isBounded( void )
{
  return mBounds;

}


// YOHE: declare it as a virtual function in COptAlgorithm.h file
//
// Execute the optimization algorithm calling simulation routine (passed as argument)
// when needed. It is noted that this procedure can give feedback of its progress by 
// the callback function set with SetCallback
//virtual int COptAlgorithm::Optimise( double (*func) (void) )
int COptAlgorithm::optimise(void)
{  
  return 0;
}



















