/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/utilities/COutputHandler.cpp,v $
   $Revision: 1.1.1.1 $
   $Name:  $
   $Author: anuragr $ 
   $Date: 2004/10/26 15:18:04 $
   End CVS Header */

#include <iostream>
#include "copasi.h"
#include "COutputHandler.h"

bool CCallbackHandler::init()
{
#ifdef COPASI_DEBUG
  std::cout << "CallbackHandler: init" << std::endl;
#endif // COPASI_DEBUG
  return true;
}

bool CCallbackHandler::doOutput()
{
#ifdef COPASI_DEBUG
  std::cout << "CallbackHandler: do" << std::endl;
#endif // COPASI_DEBUG
  return true;
}

bool CCallbackHandler::finish()
{
#ifdef COPASI_DEBUG
  std::cout << "CallbackHandler: finish" << std::endl;
#endif // COPASI_DEBUG
  return true;
}

bool CCallbackHandler::init(C_INT32 maxSteps, const std::string & text, bool cancelButton)
{
#ifdef COPASI_DEBUG
  std::cout << "CallbackHandler: init" << std::endl;
#endif // COPASI_DEBUG
  return true;
}

bool CCallbackHandler::reInit(C_INT32 maxSteps, const std::string & text)
{
#ifdef COPASI_DEBUG
  std::cout << "CallbackHandler: reInit" << std::endl;
#endif // COPASI_DEBUG
  return true;
}

bool CCallbackHandler::progress(C_INT32 steps)
{
#ifdef COPASI_DEBUG
  std::cout << "CallbackHandler: progress" << std::endl;
#endif // COPASI_DEBUG
  return true;
}
