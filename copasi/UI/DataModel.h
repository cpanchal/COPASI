/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/UI/Attic/DataModel.h,v $
   $Revision: 1.56 $
   $Name:  $
   $Author: ssahle $ 
   $Date: 2005/02/02 15:51:06 $
   End CVS Header */

/****************************************************************************
 **  $ CopasiUI/DataModel.h                 Modified on : 28th March, 2002
 **  $ Author  : Ankur Gupta
 *****************************************************************************/
#ifndef DATAMODEL_H
#define DATAMODEL_H

#include "copasi.h"

class CReadConfig;

class CModel;
class CSteadyStateTask;
class CTrajectoryTask;
class CScanTask;
class CMCATask;
class CReportDefinitionVector;
class CPlotSpec2Vector;
class COptFunction;

//******************************************************************************

class DataModel
  {
  protected:
    CModel * model;
    bool mChanged;

    CSteadyStateTask* steadystatetask;
    CTrajectoryTask* trajectorytask;
    CScanTask* scantask;
    CReportDefinitionVector* reportdefinitions;
    CPlotSpec2Vector* plotspecs;
    COptFunction* pOptFunction;
    CMCATask* mpCMCATask;

  public:
    DataModel();

    bool loadModel(const char* fileName);
    bool createModel();
    bool saveModel(const char* fileName);

    bool importSBML(const char* fileName);
    bool exportSBML(const char* fileName);

    CModel* getModel() {return model;}

    bool isChanged() const;
    void changed(const bool & changed = true);

    inline CSteadyStateTask* getSteadyStateTask() {return steadystatetask;}
    inline CTrajectoryTask* getTrajectoryTask() {return trajectorytask;}
    inline CMCATask* getMCATask() {return mpCMCATask;}
    inline CScanTask* getScanTask() {return scantask;}
    inline CReportDefinitionVector* getReportDefinitionVectorAddr(){return reportdefinitions;}
    inline CPlotSpec2Vector* getPlotSpecVectorAddr(){return plotspecs;}
  };

#endif
