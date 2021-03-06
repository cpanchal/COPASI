// Copyright (C) 2010 - 2014 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

/**
 * CCrossSectionTask class.
 *
 * This class implements a cross section task.
 */

#include <string>
#include <sstream>
#include <cmath>

#include "copasi.h"
#include <model/CEvent.h>

#include "CCrossSectionTask.h"
#include "CCrossSectionProblem.h"
//#include "CCrossSectionMethod.h"
#include "model/CModel.h"
#include "model/CMathModel.h"
#include "model/CState.h"
#include "report/CKeyFactory.h"
#include "report/CReport.h"
#include "utilities/CProcessReport.h"
#include "utilities/CCopasiException.h"
#include  "CopasiDataModel/CCopasiDataModel.h"

const unsigned int CCrossSectionTask::ValidMethods[] =
{
  CCopasiMethod::deterministic,
  CCopasiMethod::unset
};

CCrossSectionTask::CCrossSectionTask(const CCopasiContainer * pParent):
  CCopasiTask(CCopasiTask::crosssection, pParent),
  mTimeSeriesRequested(true),
  mTimeSeries(),
  mpCrossSectionProblem(NULL),
  mpTrajectoryMethod(NULL),
  mUpdateMoieties(false),
  mpCurrentState(NULL),
  mpCurrentTime(NULL),
  mOutputStartTime(0.0),
  mStartTime(0.0),
  mNumCrossings(0),
  mOutputStartNumCrossings(0),
  mMaxNumCrossings(std::numeric_limits< size_t >::max()),
  mhProgress(C_INVALID_INDEX),
  mProgressMax(1),
  mProgressValue(0),
  mProgressFactor(1),
  mpEvent(NULL),
  mState(CCrossSectionTask::TRANSIENT),
  mStatesRing(),
  mStatesRingCounter(0),
  mPreviousCrossingTime(std::numeric_limits< C_FLOAT64 >::quiet_NaN()),
  mPeriod(std::numeric_limits< C_FLOAT64 >::quiet_NaN()),
  mAveragePeriod(std::numeric_limits< C_FLOAT64 >::quiet_NaN()),
  mLastPeriod(std::numeric_limits< C_FLOAT64 >::quiet_NaN()),
  mPeriodicity(-1),
  mLastFreq(std::numeric_limits< C_FLOAT64 >::quiet_NaN()),
  mFreq(std::numeric_limits< C_FLOAT64 >::quiet_NaN()),
  mAverageFreq(std::numeric_limits< C_FLOAT64 >::quiet_NaN())
{
  initObjects();
  mpProblem = new CCrossSectionProblem(this);
  mpMethod = createMethod(CCopasiMethod::deterministic);
  this->add(mpMethod, true);
}

CCrossSectionTask::CCrossSectionTask(const CCrossSectionTask & src,
                                     const CCopasiContainer * pParent):
  CCopasiTask(src, pParent),
  mTimeSeriesRequested(src.mTimeSeriesRequested),
  mTimeSeries(),
  mpCrossSectionProblem(NULL),
  mpTrajectoryMethod(NULL),
  mUpdateMoieties(false),
  mpCurrentState(NULL),
  mpCurrentTime(NULL),
  mOutputStartTime(0.0),
  mStartTime(0.0),
  mNumCrossings(0),
  mOutputStartNumCrossings(0),
  mMaxNumCrossings(std::numeric_limits< size_t >::max()),
  mhProgress(C_INVALID_INDEX),
  mProgressMax(1),
  mProgressValue(0),
  mProgressFactor(1),
  mpEvent(NULL),
  mState(CCrossSectionTask::TRANSIENT),
  mStatesRing(),
  mStatesRingCounter(0),
  mPreviousCrossingTime(std::numeric_limits< C_FLOAT64 >::quiet_NaN()),
  mPeriod(std::numeric_limits< C_FLOAT64 >::quiet_NaN()),
  mAveragePeriod(std::numeric_limits< C_FLOAT64 >::quiet_NaN()),
  mLastPeriod(std::numeric_limits< C_FLOAT64 >::quiet_NaN()),
  mPeriodicity(-1),
  mLastFreq(std::numeric_limits< C_FLOAT64 >::quiet_NaN()),
  mFreq(std::numeric_limits< C_FLOAT64 >::quiet_NaN()),
  mAverageFreq(std::numeric_limits< C_FLOAT64 >::quiet_NaN())
{
  initObjects();
  mpProblem =
    new CCrossSectionProblem(*static_cast< CCrossSectionProblem * >(src.mpProblem), this);

  mpMethod = createMethod(src.mpMethod->getSubType());
  * mpMethod = * src.mpMethod;

  mpMethod->elevateChildren();

  this->add(mpMethod, true);
}

CCrossSectionTask::~CCrossSectionTask()
{
  cleanup();
}

void CCrossSectionTask::cleanup()
{
  pdelete(mpCurrentState);
}

void CCrossSectionTask::initObjects()
{
  addObjectReference("Period", mPeriod, CCopasiObject::ValueDbl);
  addObjectReference("Average Period", mAveragePeriod, CCopasiObject::ValueDbl);
  addObjectReference("Last Period", mLastPeriod, CCopasiObject::ValueDbl);
  addObjectReference("Periodicity", mPeriodicity, CCopasiObject::ValueInt);
  addObjectReference("Last Frequency", mLastFreq, CCopasiObject::ValueDbl);
  addObjectReference("Frequency", mFreq, CCopasiObject::ValueDbl);
  addObjectReference("Average Frequency", mAverageFreq, CCopasiObject::ValueDbl);
}

#define RING_SIZE 16

bool CCrossSectionTask::initialize(const OutputFlag & of,
                                   COutputHandler * pOutputHandler,
                                   std::ostream * pOstream)
{
  assert(mpProblem && mpMethod);

  mpCrossSectionProblem = dynamic_cast<CCrossSectionProblem *>(mpProblem);
  assert(mpCrossSectionProblem);

  mpTrajectoryMethod = dynamic_cast<CTrajectoryMethod *>(mpMethod);
  assert(mpTrajectoryMethod);

  mpTrajectoryMethod->setProblem(mpCrossSectionProblem);

  bool success = mpMethod->isValidProblem(mpProblem);

  CCopasiParameter * pParameter = mpMethod->getParameter("Integrate Reduced Model");

  if (pParameter != NULL)
    mUpdateMoieties = *pParameter->getValue().pBOOL;
  else
    mUpdateMoieties = false;

  pdelete(mpCurrentState);
  mpCurrentState = new CState(mpCrossSectionProblem->getModel()->getState());
  mpCurrentTime = &mpCurrentState->getTime();

  //init the ring buffer for the states
  mStatesRing.resize(RING_SIZE);
  mStatesRingCounter = 0;

  // Handle the time series as a regular output.
  mTimeSeriesRequested = true;//mpCrossSectionProblem->timeSeriesRequested();

  if ((pOutputHandler != NULL) &&
      mTimeSeriesRequested &&
      (of & CCopasiTask::TIME_SERIES))
    {
      mTimeSeries.allocate(20);
      pOutputHandler->addInterface(&mTimeSeries);
    }
  else
    {
      mTimeSeries.clear();
    }

  if (!CCopasiTask::initialize(of, pOutputHandler, pOstream)) success = false;

  return success;
}

void CCrossSectionTask::createEvent()
{
  if (mpEvent != NULL) return;

  CModel* pModel = mpCrossSectionProblem->getModel();

  if (!mpCrossSectionProblem->getSingleObjectCN().empty())
    {
      int count = 0;
      std::string name = "__cutplane";

      while (pModel->getEvents().getIndex(name) != C_INVALID_INDEX)
        {
          std::stringstream str;
          str << "__cutplane" << ++count;
          name = str.str();
        }

      mpEvent = pModel->createEvent(name);
      mpEvent->setType(CEvent::CutPlane);

      std::stringstream expression;
      expression << "<" << mpCrossSectionProblem->getSingleObjectCN() << "> "
                 << (mpCrossSectionProblem->isPositiveDirection() ? std::string(" > ") : std::string(" < "))
                 << mpCrossSectionProblem->getThreshold();

      mpEvent ->setTriggerExpression(expression.str());
    }

  pModel->setCompileFlag();
  pModel->compileIfNecessary(NULL);
}

void CCrossSectionTask::removeEvent()
{
  // TODO: remove event
  if (mpEvent != NULL)
    {
      mpCrossSectionProblem->getModel()->removeEvent(mpEvent);
      mpEvent = NULL;
    }
}

bool CCrossSectionTask::process(const bool & useInitialValues)
{
  createEvent();

  processStart(useInitialValues);

  //this instructs the process queue to call back whenever an event is
  //executed
  mpCrossSectionProblem->getModel()->getMathModel()->getProcessQueue().setEventCallBack(this, &EventCallBack);

  mPreviousCrossingTime = std::numeric_limits< C_FLOAT64 >::quiet_NaN();
  mPeriod = std::numeric_limits< C_FLOAT64 >::quiet_NaN();
  mAveragePeriod = std::numeric_limits< C_FLOAT64 >::quiet_NaN();
  mLastPeriod = std::numeric_limits< C_FLOAT64 >::quiet_NaN();
  mPeriodicity = -1;
  mLastFreq = std::numeric_limits< C_FLOAT64 >::quiet_NaN();
  mFreq = std::numeric_limits< C_FLOAT64 >::quiet_NaN();
  mAverageFreq = std::numeric_limits< C_FLOAT64 >::quiet_NaN();

  C_FLOAT64 MaxDuration = mpCrossSectionProblem->getDuration();

  //the output starts only after "outputStartTime" has passed
  if (mpCrossSectionProblem->getFlagLimitOutTime())
    {
      mOutputStartTime = *mpCurrentTime + mpCrossSectionProblem->getOutputStartTime();
      MaxDuration += mpCrossSectionProblem->getOutputStartTime();
    }
  else
    {
      mOutputStartTime = *mpCurrentTime;
    }

  const C_FLOAT64 EndTime = *mpCurrentTime + MaxDuration;

  mStartTime = *mpCurrentTime;

  // It suffices to reach the end time within machine precision
  C_FLOAT64 CompareEndTime = mOutputStartTime - 100.0 * (fabs(EndTime) * std::numeric_limits< C_FLOAT64 >::epsilon() + std::numeric_limits< C_FLOAT64 >::min());

  if (mpCrossSectionProblem->getFlagLimitCrossings())
    mMaxNumCrossings = mpCrossSectionProblem->getCrossingsLimit();
  else
    mMaxNumCrossings = 0;

  if (mpCrossSectionProblem->getFlagLimitOutCrossings())
    mOutputStartNumCrossings = mpCrossSectionProblem->getOutCrossingsLimit();
  else
    mOutputStartNumCrossings = 0;

  output(COutputInterface::BEFORE);

  bool flagProceed = true;
  mProgressFactor = 100.0 / (MaxDuration + mpCrossSectionProblem->getOutputStartTime());
  mProgressValue = 0;

  if (mpCallBack != NULL)
    {
      mpCallBack->setName("performing simulation...");
      mProgressMax = 100.0;
      mhProgress = mpCallBack->addItem("Completion",
                                       mProgressValue,
                                       &mProgressMax);
    }

  mState = TRANSIENT;
  mStatesRingCounter = 0;

  mNumCrossings = 0;

  try
    {
      do
        {
          flagProceed &= processStep(EndTime);
        }
      while ((*mpCurrentTime < CompareEndTime) && flagProceed);
    }

  catch (int)
    {
      mpCrossSectionProblem->getModel()->setState(*mpCurrentState);
      mpCrossSectionProblem->getModel()->updateSimulatedValues(mUpdateMoieties);
      finish();
      CCopasiMessage(CCopasiMessage::EXCEPTION, MCTrajectoryMethod + 16);
    }

  catch (CCopasiException & Exception)
    {
      mpCrossSectionProblem->getModel()->setState(*mpCurrentState);
      mpCrossSectionProblem->getModel()->updateSimulatedValues(mUpdateMoieties);
      finish();
      throw CCopasiException(Exception.getMessage());
    }

  finish();

  return true;
}

void CCrossSectionTask::processStart(const bool & useInitialValues)
{
  if (useInitialValues)
    mpCrossSectionProblem->getModel()->applyInitialValues();

  *mpCurrentState = mpCrossSectionProblem->getModel()->getState();

  mpTrajectoryMethod->setCurrentState(mpCurrentState);
  mpTrajectoryMethod->start(mpCurrentState);

  return;
}

bool CCrossSectionTask::processStep(const C_FLOAT64 & endTime)
{
  CModel * pModel = mpCrossSectionProblem->getModel();
  bool StateChanged = false;
  bool proceed = true;

  C_FLOAT64 Tolerance = 100.0 * (fabs(endTime) * std::numeric_limits< C_FLOAT64 >::epsilon() + std::numeric_limits< C_FLOAT64 >::min());
  C_FLOAT64 NextTime = endTime;

  while (proceed)
    {
      // TODO Provide a call back method for resolving simultaneous assignments.
      //pModel->getMathModel()->getProcessQueue().printDebug();

      //execute events for inequalities
      StateChanged |= pModel->processQueue(*mpCurrentTime, false, NULL);

      if (StateChanged)
        {
          *mpCurrentState = pModel->getState();
          mpTrajectoryMethod->stateChanged();
          StateChanged = false;
        }

      // std::min suffices since events are only supported in forward integration.
      NextTime = std::min(endTime, pModel->getProcessQueueExecutionTime());

      switch (mpTrajectoryMethod->step(NextTime - *mpCurrentTime))
        {
          case CTrajectoryMethod::NORMAL:
            pModel->setState(*mpCurrentState);
            pModel->updateSimulatedValues(mUpdateMoieties);

            // TODO Provide a call back method for resolving simultaneous assignments.

            //execute events for equalities
            StateChanged |= pModel->processQueue(*mpCurrentTime, true, NULL);

            // If the state change happens to coincide with end of the step we have to return and
            // inform the integrator of eventual state changes.
            if (fabs(*mpCurrentTime - endTime) < Tolerance)
              {
                if (StateChanged)
                  {
                    *mpCurrentState = pModel->getState();
                    mpTrajectoryMethod->stateChanged();
                    StateChanged = false;
                  }

                return true;
              }

            break;

          case CTrajectoryMethod::ROOT:
            //we arrive here whenever the integrator finds a root
            //this does not necessarily mean an event fires
            pModel->setState(*mpCurrentState);
            pModel->updateSimulatedValues(mUpdateMoieties);

            //this checks whether equality events are triggered
            pModel->processRoots(*mpCurrentTime, true, true, mpTrajectoryMethod->getRoots());

            // TODO Provide a call back method for resolving simultaneous assignments.

            //execute scheduled events for equalities
            StateChanged |= pModel->processQueue(*mpCurrentTime, true, NULL);

            //this checks whether inequality events are triggered
            pModel->processRoots(*mpCurrentTime, false, true, mpTrajectoryMethod->getRoots());

            // If the root happens to coincide with end of the step we have to return and
            // inform the integrator of eventual state changes.
            if (fabs(*mpCurrentTime - endTime) < Tolerance)
              {
                if (StateChanged)
                  {
                    *mpCurrentState = pModel->getState();
                    mpTrajectoryMethod->stateChanged();
                    StateChanged = false;
                  }

                return true;
              }

            break;

          case CTrajectoryMethod::FAILURE:
            finish();
            CCopasiMessage(CCopasiMessage::EXCEPTION, MCTrajectoryMethod + 12);
            return false;
            break;
        }

      proceed = mpCallBack == NULL || mpCallBack->proceed();

      if (mState == FINISH)
        proceed = false;
    }

  return proceed;
}

void CCrossSectionTask::finish()
{
  //reset call back
  mpCrossSectionProblem->getModel()->getMathModel()->getProcessQueue().setEventCallBack(NULL, NULL);

  if (mpCallBack != NULL) mpCallBack->finishItem(mhProgress);

  output(COutputInterface::AFTER);
}

bool CCrossSectionTask::restore()
{
  bool success = CCopasiTask::restore();
  CModel * pModel = mpProblem->getModel();

  removeEvent();

  if (mUpdateModel)
    {

      pModel->setState(*mpCurrentState);
      pModel->updateSimulatedValues(mUpdateMoieties);
      pModel->setInitialState(pModel->getState());
      pModel->updateInitialValues();
    }

  //reset call back
  mpCrossSectionProblem->getModel()->getMathModel()->getProcessQueue().setEventCallBack(NULL, NULL);

  return success;
}

bool CCrossSectionTask::setMethodType(const int & type)
{
  CCopasiMethod::SubType Type = (CCopasiMethod::SubType) type;

  if (!isValidMethod(Type, ValidMethods)) return false;

  if (mpMethod->getSubType() == Type) return true;

  pdelete(mpMethod);
  mpMethod = createMethod(Type);
  this->add(mpMethod, true);

  CCopasiParameter * pParameter = mpMethod->getParameter("Integrate Reduced Model");

  if (pParameter != NULL)
    mUpdateMoieties = *pParameter->getValue().pBOOL;
  else
    mUpdateMoieties = false;

  return true;
}

// virtual
CCopasiMethod * CCrossSectionTask::createMethod(const int & type) const
{
  CCopasiMethod::SubType Type = (CCopasiMethod::SubType) type;

  return CTrajectoryMethod::createMethod(Type);
}

CState * CCrossSectionTask::getState()
{return mpCurrentState;}

const CTimeSeries & CCrossSectionTask::getTimeSeries() const
{return mTimeSeries;}

//static
void CCrossSectionTask::EventCallBack(void* pCSTask, CEvent::Type type)
{static_cast<CCrossSectionTask *>(pCSTask)->eventCallBack(type);}

void CCrossSectionTask::eventCallBack(CEvent::Type type)
{
  //  std::cout << "event call back: " << type << std::endl;

  //do progress reporting
  if (mpCallBack != NULL)
    {
      mProgressValue = (*mpCurrentTime - mStartTime) * mProgressFactor;

      if (!mpCallBack->progressItem(mhProgress))
        {
          mState = FINISH;
        }
    }

  //do nothing else if the event is not representing a cut plane
  if (type != CEvent::CutPlane)
    return;

  mpProblem->getModel()->setState(*mpCurrentState);
  mpProblem->getModel()->updateSimulatedValues(mUpdateMoieties);

  // count the crossings
  ++mNumCrossings;

  // now check if we can transition to the main state
  if (mState == TRANSIENT)
    {
      // if output is not delayed by time and not delayed by number of crossings
      // and also not delayed by convergence
      // output is started immediately
      if (!mpCrossSectionProblem->getFlagLimitOutCrossings() &&
          !mpCrossSectionProblem->getFlagLimitOutTime() &&
          !mpCrossSectionProblem->getFlagLimitOutConvergence())
        {
          mState = MAIN;
        }
      else if (mpCrossSectionProblem->getFlagLimitOutTime() && *mpCurrentTime >= mOutputStartTime)
        {
          mState = MAIN;
        }
      else if (mpCrossSectionProblem->getFlagLimitOutCrossings() && mNumCrossings >= mOutputStartNumCrossings)
        {
          mState = MAIN;
        }
      else if (mpCrossSectionProblem->getFlagLimitOutConvergence() &&
               mStatesRingCounter > 0)
        {
          int i;

          for (i = mStatesRingCounter - 1; i >= 0 && i >= ((int) mStatesRingCounter) - RING_SIZE; --i)
            {
              C_FLOAT64 tmp = relativeDifferenceOfStates(&mStatesRing[i % RING_SIZE],
                              mpCurrentState);

              if (tmp < mpCrossSectionProblem->getConvergenceOutTolerance())
                {
                  mPeriodicity = mStatesRingCounter - i;
                  mPeriod = *mpCurrentTime - mStatesRing[i % RING_SIZE].getTime();
                  mFreq = 1 / mPeriod;
                  mAveragePeriod = mPeriod / ((C_FLOAT64)mPeriodicity);
                  mAverageFreq = 1 / mAveragePeriod;

                  mState = MAIN;
                  break;
                }

              //std::cout << i << "  " << tmp     << std::endl;
            }
        }

      if (mState == MAIN)
        {
          mStatesRingCounter = 0;
          mNumCrossings = 1;
        }
    }

  if (mState == MAIN)
    {
      //check for convergence (actually for the occurrence of similar states)
      if (mStatesRingCounter > 0)
        {
          int i;

          for (i = mStatesRingCounter - 1; i >= 0 && i >= ((int) mStatesRingCounter) - RING_SIZE; --i)
            {
              C_FLOAT64 tmp = relativeDifferenceOfStates(&mStatesRing[i % RING_SIZE],
                              mpCurrentState);

              if (tmp < mpCrossSectionProblem->getConvergenceTolerance())
                {
                  mPeriodicity = mStatesRingCounter - i;
                  mPeriod = *mpCurrentTime - mStatesRing[i % RING_SIZE].getTime();
                  mFreq = 1 / mPeriod;
                  mAveragePeriod = mPeriod / ((C_FLOAT64)mPeriodicity);
                  mAverageFreq = 1 / mAveragePeriod;

                  if (mpCrossSectionProblem->getFlagLimitConvergence())
                    mState = FINISH;

                  break;
                }

              //std::cout << "MAIN" << i << "  " << tmp     << std::endl;
            }
        }

      if (!isnan(mPreviousCrossingTime))
        {
          mLastPeriod = *mpCurrentTime - mPreviousCrossingTime;
        }

      mLastFreq = 1 / mLastPeriod;

      output(COutputInterface::DURING);

      //check if the conditions for stopping are met
      //we don't have to check for maximum duration, this is done elsewhere
      if (mMaxNumCrossings > 0 && mNumCrossings >= mMaxNumCrossings)
        mState = FINISH;
    }

  //store state in ring buffer
  mStatesRing[mStatesRingCounter % RING_SIZE] = *mpCurrentState;
  mPreviousCrossingTime = *mpCurrentTime;
  ++mStatesRingCounter;
}

//static
C_FLOAT64 CCrossSectionTask::relativeDifferenceOfStates(CState* s1, CState* s2)
{
  if (!s1 || !s2)
    {
      return std::numeric_limits< C_FLOAT64 >::quiet_NaN();
    }

  if (s1->endIndependent() - s1->beginIndependent()
      != s2->endIndependent() - s2->beginIndependent())
    {
      return std::numeric_limits< C_FLOAT64 >::quiet_NaN();
    }

  C_FLOAT64 ret = 0;

  const C_FLOAT64 * p1 = s1->beginIndependent();
  const C_FLOAT64 * p1End = s1->endIndependent();
  const C_FLOAT64 * p2 = s2->beginIndependent();

  for (; p1 != p1End; ++p1, ++p2)
    {
      ret += (*p1 != *p2) ? pow((*p1 - *p2) / (fabs(*p1) + fabs(*p2)), 2) : 0.0;
    }

  return 2.0 * sqrt(ret);
}
