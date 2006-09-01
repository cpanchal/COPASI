/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/CopasiDataModel/CCopasiDataModel.cpp,v $
   $Revision: 1.71 $
   $Name:  $
   $Author: shoops $
   $Date: 2006/09/01 12:18:09 $
   End CVS Header */

// Copyright � 2005 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#include "copasi.h"
#include "copasiversion.h"
#include "CCopasiDataModel.h"

#include "commandline/COptions.h"
#include "commandline/CConfigurationFile.h"
#include "function/CFunctionDB.h"
#include "model/CModel.h"
#include "optimization/COptTask.h"
#include "parameterFitting/CFitTask.h"
#include "plot/COutputDefinitionVector.h"
#include "report/CKeyFactory.h"
#include "report/CReportDefinitionVector.h"
#include "sbml/SBMLExporter.h"
#include "sbml/SBMLImporter.h"
#include "scan/CScanTask.h"
#include "elementaryFluxModes/CEFMTask.h"
//#include "steadystate/CMCAMethod.h"
#include "steadystate/CMCATask.h"
#include "steadystate/CMCAProblem.h"
#include "steadystate/CSteadyStateTask.h"
#include "trajectory/CTrajectoryTask.h"
#ifdef COPASI_TSS
# include "tss/CTSSTask.h"
#endif
#ifdef COPASI_SENS
# include "sensitivities/CSensTask.h"
#endif
#include "lyap/CLyapTask.h"
#include "tss/MMASCIIExporter.h"
#include "utilities/CCopasiException.h"
#include "utilities/CCopasiProblem.h"
#include "utilities/CCopasiTask.h"
#include "utilities/CCopasiVector.h"
#include "utilities/CDirEntry.h"
#include "xml/CCopasiXML.h"

CDataModelRenameHandler::CDataModelRenameHandler(CCopasiDataModel* dm)
    : mpDataModel(dm)
{}

bool CDataModelRenameHandler::handle(const std::string & oldCN, const std::string & newCN) const
  {
    //std::cout << "CDataModelRenameHandler::handle()" << std::endl;
    //std::cout << " old: " << oldCN << std::endl;
    //std::cout << " new: " << newCN << std::endl;

    const std::set<CRegisteredObjectName*> nameSet = CRegisteredObjectName::getSet();

    //std::cout << " ************ " << nameSet.size() << std::endl;
    std::set<CRegisteredObjectName*>::const_iterator it, itEnd = nameSet.end();
    for (it = nameSet.begin(); it != itEnd; ++it)
      {
        if (oldCN.compare(0, oldCN.size(), **it, 0, oldCN.size()) == 0)
          {
            //std::cout << "   match:   " << **it << std::endl;
            (**it).replace(0, oldCN.size(), newCN);
            //std::cout << "     -->    " << **it << std::endl;
          }
        else
          {
            //std::cout << "            " << **it << std::endl;
          }
      }

    //std::cout << " " << std::endl;
    return true;
  }

//********************************************************************

CCopasiDataModel * CCopasiDataModel::Global = NULL;

CCopasiDataModel::CCopasiDataModel(const bool withGUI):
    COutputHandler(),
    mpVersion(new CVersion),
    mpFunctionList(new CFunctionDB),
    mpModel(NULL),
    mpTaskList(NULL),
    mpReportDefinitionList(NULL),
    mpPlotDefinitionList(NULL),
    mWithGUI(withGUI),
    mpGUI(NULL),
    mpConfiguration(new CConfigurationFile),
    mChanged(false),
    mAutoSaveNeeded(false),
    mRenameHandler(this),
    mpCurrentSBMLDocument(NULL),
    mSBMLFileName(""),
    pOldMetabolites(new CCopasiVectorS < CMetabOld >)
{
  mpVersion->setVersion(COPASI_VERSION_MAJOR,
                        COPASI_VERSION_MINOR,
                        COPASI_VERSION_BUILD);

  mpUndefined = new CFunction("undefined");
  mpUndefined->setInfix("nan");
  mpUndefined->compile();

  GlobalKeys.remove(mpUndefined->getKey());
  GlobalKeys.addFix("UndefinedFunction_0", mpUndefined);

  mpFunctionList->load();
  newModel();
  CCopasiObject::setRenameHandler(&mRenameHandler); //TODO where in the contructor should this be called?

  mpConfiguration->load();
}

CCopasiDataModel::~CCopasiDataModel()
{
  CCopasiObject::setRenameHandler(NULL);
  if (mpConfiguration != NULL)
    mpConfiguration->save();
  pdelete(mpConfiguration);

  pdelete(mpVersion);
  pdelete(mpFunctionList);
  pdelete(mpModel);
  pdelete(mpTaskList);
  pdelete(mpReportDefinitionList);
  pdelete(mpPlotDefinitionList);
  pdelete(mpGUI);
  pdelete(mpCurrentSBMLDocument);
  pdelete(pOldMetabolites);
}

bool CCopasiDataModel::loadModel(const std::string & fileName)
{
  CCopasiMessage::clearDeque();

  std::string PWD;
  COptions::getValue("PWD", PWD);

  std::string FileName = fileName;

  if (CDirEntry::isRelativePath(FileName) &&
      !CDirEntry::makePathAbsolute(FileName, PWD))
    FileName = CDirEntry::fileName(FileName);

  std::ifstream File(utf8ToLocale(FileName).c_str());

  if (File.fail())
    {
      CCopasiMessage Message(CCopasiMessage::RAW,
                             "File error when opening '%s'.",
                             FileName.c_str());
      return false;
    }

  std::string Line;
  File >> Line;

  if (!Line.compare(0, 8, "Version="))
    {
      File.close();
      CReadConfig inbuf(FileName.c_str());
      if (inbuf.getVersion() >= "4")
        {
          CCopasiMessage(CCopasiMessage::ERROR,
                         "Can't handle Gepasi Files with Version>=4.");
          return false;
        }

      newModel();
      mpModel->load(inbuf);

      dynamic_cast<CSteadyStateTask *>((*mpTaskList)["Steady-State"])->load(inbuf);
      dynamic_cast<CTrajectoryTask *>((*mpTaskList)["Time-Course"])->load(inbuf);

      mSaveFileName = CDirEntry::dirName(FileName)
                      + CDirEntry::Separator
                      + CDirEntry::baseName(FileName);

      std::string Suffix = CDirEntry::suffix(FileName);
      if (strcasecmp(Suffix.c_str(), ".gps") != 0)
        mSaveFileName += Suffix;

      mSaveFileName += ".cps";
      mSBMLFileName = "";
    }
  else if (!Line.compare(0, 5, "<?xml"))
    {
      //std::cout << "XML Format" << std::endl;
      File.seekg(0, std::ios_base::beg);

      CCopasiXML XML;

      XML.setFunctionList(mpFunctionList->loadedFunctions());

      SCopasiXMLGUI *pGUI = NULL;
      std::string SBMLFileNameBkp = mSBMLFileName;

      if (mWithGUI)
        {
          pGUI = new SCopasiXMLGUI;
          XML.setGUI(*pGUI);
        }

      if (!XML.load(File))
        {
          XML.freeModel();
          XML.freeTaskList();
          XML.freeReportList();
          XML.freePlotList();
          XML.freeGUI();

          mSBMLFileName = SBMLFileNameBkp;
          return false;
        }

      newModel(XML.getModel());

      if (XML.getTaskList())
        {
          pdelete(mpTaskList);
          mpTaskList = XML.getTaskList();
          mpTaskList->setObjectName("TaskList");
          CCopasiContainer::Root->add(mpTaskList, true);
          addDefaultTasks();
        }

      if (XML.getReportList())
        {
          pdelete(mpReportDefinitionList);
          mpReportDefinitionList = XML.getReportList();
          addDefaultReports();
        }

      if (XML.getPlotList())
        {
          pdelete(mpPlotDefinitionList);
          mpPlotDefinitionList = XML.getPlotList();
        }

      if (mWithGUI)
        {
          pdelete(mpGUI);
          mpGUI = pGUI;
        }

      mSaveFileName = FileName;
    }

  if (mpModel) mpModel->setCompileFlag();
  changed(false);
  return true;
}

bool CCopasiDataModel::saveModel(const std::string & fileName, bool overwriteFile,
                                 const bool & autoSave)
{
  CCopasiMessage::clearDeque();

  std::string FileName = (fileName != "") ? fileName : mSaveFileName;

  std::string PWD;
  COptions::getValue("PWD", PWD);

  if (CDirEntry::isRelativePath(FileName) &&
      !CDirEntry::makePathAbsolute(FileName, PWD))
    FileName = CDirEntry::fileName(FileName);

  if (CDirEntry::exist(FileName))
    {
      if (!overwriteFile)
        {
          CCopasiMessage(CCopasiMessage::ERROR,
                         MCDirEntry + 1,
                         FileName.c_str());
          return false;
        }

      if (!CDirEntry::isWritable(FileName))
        {
          CCopasiMessage(CCopasiMessage::ERROR,
                         MCDirEntry + 2,
                         FileName.c_str());
          return false;
        }
    }

  try
    {
      if (!mpModel->compileIfNecessary())
        return false;
    }

  catch (...)
    {
      return false;
    }

  CCopasiXML XML;

  XML.setModel(*mpModel);
  XML.setTaskList(*mpTaskList);
  XML.setReportList(*mpReportDefinitionList);
  XML.setPlotList(*mpPlotDefinitionList);
  XML.setGUI(*mpGUI);

  // We are first writing to a temporary stream to prevent accidental
  // destruction of an existing file in case the save command fails.

  if (!XML.CCopasiXMLInterface::save(FileName)) return false;

  if (!autoSave)
    {
      changed(false);
      mSaveFileName = FileName;
    }

  return true;
}

bool CCopasiDataModel::autoSave()
{
  if (!mAutoSaveNeeded) return true;

  std::string AutoSave;

  COptions::getValue("Tmp", AutoSave);
  if (AutoSave == "") return false;

  AutoSave += CDirEntry::Separator + "tmp_";

  if (mSaveFileName != "")
    AutoSave += CDirEntry::baseName(mSaveFileName);
  else
    AutoSave += "untitled";

  AutoSave += ".cps";

  try
    {
      if (!saveModel(AutoSave, true, true)) return false;
    }

  catch (...)
    {
      return false;
    }

  mAutoSaveNeeded = false;
  return true;
}

bool CCopasiDataModel::newModel(CModel * pModel)
{
  pdelete(mpModel);

  if (pModel)
    mpModel = pModel;
  else
    {
      CCopasiMessage::clearDeque();

      mpModel = new CModel();
      mSaveFileName = "";
      mSBMLFileName = "";

      pdelete(mpCurrentSBMLDocument);

      this->mCopasi2SBMLMap.clear();
    }

  pdelete(mpTaskList);
  mpTaskList = new CCopasiVectorN< CCopasiTask >("TaskList", CCopasiContainer::Root);

  // We have at least one task of every type
  addDefaultTasks();

  pdelete(mpReportDefinitionList);
  mpReportDefinitionList = new CReportDefinitionVector;
  addDefaultReports();

  pdelete(mpPlotDefinitionList);
  mpPlotDefinitionList = new COutputDefinitionVector;

  if (mWithGUI)
    {
      pdelete(mpGUI);
      mpGUI = new SCopasiXMLGUI;
    }

  changed(false);

  return true;
}

bool CCopasiDataModel::importSBML(const std::string & fileName, CProcessReport* pImportHandler)
{
  CCopasiMessage::clearDeque();

  std::string PWD;
  COptions::getValue("PWD", PWD);

  std::string FileName = fileName;

  if (CDirEntry::isRelativePath(FileName) &&
      !CDirEntry::makePathAbsolute(FileName, PWD))
    FileName = CDirEntry::fileName(FileName);

  SBMLImporter importer;
  importer.setImportHandler(pImportHandler);
  mCopasi2SBMLMap.clear();
  CModel* pModel = NULL;

  SBMLDocument * pSBMLDocument = NULL;
  std::map<CCopasiObject*, SBase*> Copasi2SBMLMap;

  try
    {
      pModel = importer.readSBML(FileName, mpFunctionList, pSBMLDocument, Copasi2SBMLMap);
    }
  catch (CCopasiException except)
    {
      importer.restoreFunctionDB();
      throw except;
    }

  if (pModel == NULL)
    {
      importer.restoreFunctionDB();
      return false;
    }

  mSaveFileName = CDirEntry::dirName(FileName)
                  + CDirEntry::Separator
                  + CDirEntry::baseName(FileName);

  std::string Suffix = CDirEntry::suffix(FileName);
  if (strcasecmp(Suffix.c_str(), ".xml") != 0)
    mSaveFileName += Suffix;

  mSaveFileName += ".cps";
  mSBMLFileName = FileName;

  pdelete(mpCurrentSBMLDocument);

  mpCurrentSBMLDocument = pSBMLDocument;
  mCopasi2SBMLMap = Copasi2SBMLMap;

  return newModel(pModel);
}

bool CCopasiDataModel::exportSBML(const std::string & fileName, bool overwriteFile, CProcessReport* pExportHandler)
{
  CCopasiMessage::clearDeque();

  if (fileName == "") return false;

  std::string PWD;
  COptions::getValue("PWD", PWD);

  std::string FileName = fileName;

  if (CDirEntry::isRelativePath(FileName) &&
      !CDirEntry::makePathAbsolute(FileName, PWD))
    FileName = CDirEntry::fileName(FileName);

  if (CDirEntry::exist(FileName))
    {
      if (!overwriteFile)
        {
          CCopasiMessage(CCopasiMessage::ERROR,
                         MCDirEntry + 1,
                         FileName.c_str());
          return false;
        }

      if (!CDirEntry::isWritable(FileName))
        {
          CCopasiMessage(CCopasiMessage::ERROR,
                         MCDirEntry + 2,
                         FileName.c_str());
          return false;
        }
    }

  try
    {
      if (!mpModel->compileIfNecessary())
        return false;
    }

  catch (...)
    {
      return false;
    }

  SBMLExporter exporter;
  exporter.setExportHandler(pExportHandler);
  if (!exporter.exportSBML(mpModel, FileName.c_str(), overwriteFile)) return false;

  if (mpCurrentSBMLDocument != exporter.getSBMLDocument())
    pdelete(mpCurrentSBMLDocument);

  mpCurrentSBMLDocument = exporter.getSBMLDocument();
  mSBMLFileName = FileName;

  return true;
}

bool CCopasiDataModel::exportMathModel(const std::string & fileName, const std::string & filter, bool overwriteFile)
{
  CCopasiMessage::clearDeque();

  if (fileName == "") return false;

  if (CDirEntry::exist(fileName))
    {
      if (!overwriteFile)
        {
          CCopasiMessage(CCopasiMessage::ERROR,
                         MCDirEntry + 1,
                         fileName.c_str());
          return false;
        }

      if (!CDirEntry::isWritable(fileName))
        {
          CCopasiMessage(CCopasiMessage::ERROR,
                         MCDirEntry + 2,
                         fileName.c_str());
          return false;
        }
    }

  MMASCIIExporter exporter;

  try
    {
      if (!mpModel->compileIfNecessary())
        return false;
    }

  catch (...)
    {
      return false;
    }

  return exporter.exportMathModel(mpModel, fileName.c_str(), filter.c_str(), overwriteFile);
}

CModel * CCopasiDataModel::getModel()
{return mpModel;}

CCopasiVectorN< CCopasiTask > * CCopasiDataModel::getTaskList()
{return mpTaskList;}

// static
CCopasiTask * CCopasiDataModel::addTask(const CCopasiTask::Type & taskType)
{
  CCopasiTask * pTask = NULL;

  switch (taskType)
    {
    case CCopasiTask::steadyState:
      pTask = new CSteadyStateTask(mpTaskList);
      break;

    case CCopasiTask::timeCourse:
      pTask = new CTrajectoryTask(mpTaskList);
      break;

    case CCopasiTask::scan:
      pTask = new CScanTask(mpTaskList);
      break;

    case CCopasiTask::fluxMode:
      pTask = new CEFMTask(mpTaskList);
      break;

    case CCopasiTask::optimization:
      pTask = new COptTask(taskType, mpTaskList);
      break;

    case CCopasiTask::parameterFitting:
      pTask = new CFitTask(taskType, mpTaskList);
      break;

    case CCopasiTask::mca:
      pTask = new CMCATask(mpTaskList);
      static_cast< CMCAProblem * >(pTask->getProblem())->setSteadyStateRequested(true);
      break;

    case CCopasiTask::lyap:
      pTask = new CLyapTask(mpTaskList);
      break;

#ifdef COPASI_TSS
    case CCopasiTask::tss:
      pTask = new CTSSTask(mpTaskList);
      break;
#endif

#ifdef COPASI_SENS
    case CCopasiTask::sens:
      pTask = new CSensTask(mpTaskList);
      break;
#endif

    default:
      return pTask;
    }

  pTask->getProblem()->setModel(mpModel);
  mpTaskList->add(pTask);

  return pTask;
}

bool CCopasiDataModel::addDefaultTasks()
{
  unsigned C_INT32 i;
  for (i = 0; CCopasiTask::TypeName[i] != ""; i++)
    if (mpTaskList->getIndex(CCopasiTask::TypeName[i]) == C_INVALID_INDEX)
      addTask((CCopasiTask::Type) i);

  return true;
}

std::set<std::string> CCopasiDataModel::listTaskDependentOnReport(const std::string & key)
{
  std::set<std::string> TaskKeys;

  CReportDefinition * pReportDefinition
  = dynamic_cast< CReportDefinition * >(GlobalKeys.get(key));
  if (!pReportDefinition) return TaskKeys;

  unsigned C_INT32 i, imax = mpTaskList->size();
  for (i = 0; i < imax; i++)
    if (pReportDefinition == (*mpTaskList)[i]->getReport().getReportDefinition())
      TaskKeys.insert((*mpTaskList)[i]->getKey());

  return TaskKeys;
}

CReportDefinition * CCopasiDataModel::addReport(const CCopasiTask::Type & taskType)
{
  CReportDefinition * pReport = NULL;

  switch (taskType)
    {
    case CCopasiTask::steadyState:
      pReport = new CReportDefinition(CCopasiTask::TypeName[taskType]);
      pReport->setTaskType(taskType);
      pReport->setComment("Automatically generated report.");
      pReport->setIsTable(false);
      pReport->setSeparator(CCopasiReportSeparator("\t"));
      pReport->getFooterAddr()->push_back(CCopasiObjectName("CN=Root,Vector=TaskList[Steady-State]"));
      break;

    case CCopasiTask::timeCourse:
      // No default report available.
      break;

    case CCopasiTask::scan:
      // No default report available.
      break;

    case CCopasiTask::fluxMode:
      pReport = new CReportDefinition(CCopasiTask::TypeName[taskType]);
      pReport->setTaskType(taskType);
      pReport->setComment("Automatically generated report.");
      pReport->setIsTable(false);
      pReport->setSeparator(CCopasiReportSeparator("\t"));
      pReport->getFooterAddr()->push_back(CCopasiObjectName("CN=Root,Vector=TaskList[Elementary Flux Modes],Object=Result"));
      break;

    case CCopasiTask::optimization:
      pReport = new CReportDefinition(CCopasiTask::TypeName[taskType]);
      pReport->setTaskType(taskType);
      pReport->setComment("Automatically generated report.");
      pReport->setIsTable(false);
      pReport->setTitle(false);
      pReport->setSeparator(CCopasiReportSeparator("\t"));

      // Header
      pReport->getHeaderAddr()->push_back(CCopasiObjectName("CN=Root,Vector=TaskList[Optimization],Object=Description"));
      pReport->getHeaderAddr()->push_back(CCopasiObjectName("String=\\[Function Evaluations\\]"));
      pReport->getHeaderAddr()->push_back(CCopasiObjectName("Separator=\t"));
      pReport->getHeaderAddr()->push_back(CCopasiObjectName("String=\\[Best Value\\]"));
      pReport->getHeaderAddr()->push_back(CCopasiObjectName("Separator=\t"));
      pReport->getHeaderAddr()->push_back(CCopasiObjectName("String=\\[Best Parameters\\]"));

      // Body
      pReport->getBodyAddr()->push_back(CCopasiObjectName("CN=Root,Vector=TaskList[Optimization],Problem=Optimization,Reference=Function Evaluations"));
      pReport->getBodyAddr()->push_back(CCopasiObjectName("Separator=\t"));
      pReport->getBodyAddr()->push_back(CCopasiObjectName("CN=Root,Vector=TaskList[Optimization],Problem=Optimization,Reference=Best Value"));
      pReport->getBodyAddr()->push_back(CCopasiObjectName("Separator=\t"));
      pReport->getBodyAddr()->push_back(CCopasiObjectName("CN=Root,Vector=TaskList[Optimization],Problem=Optimization,Reference=Best Parameters"));

      // Footer
      pReport->getFooterAddr()->push_back(CCopasiObjectName("String=\n"));
      pReport->getFooterAddr()->push_back(CCopasiObjectName("CN=Root,Vector=TaskList[Optimization],Object=Result"));
      break;

      //**************************************************************************
    case CCopasiTask::parameterFitting:
      pReport = new CReportDefinition(CCopasiTask::TypeName[taskType]);
      pReport->setTaskType(taskType);
      pReport->setComment("Automatically generated report.");
      pReport->setIsTable(false);
      pReport->setTitle(false);
      pReport->setSeparator(CCopasiReportSeparator("\t"));

      // Header
      pReport->getHeaderAddr()->push_back(CCopasiObjectName("CN=Root,Vector=TaskList[Parameter Estimation],Object=Description"));
      pReport->getHeaderAddr()->push_back(CCopasiObjectName("String=\\[Function Evaluations\\]"));
      pReport->getHeaderAddr()->push_back(CCopasiObjectName("Separator=\t"));
      pReport->getHeaderAddr()->push_back(CCopasiObjectName("String=\\[Best Value\\]"));
      pReport->getHeaderAddr()->push_back(CCopasiObjectName("Separator=\t"));
      pReport->getHeaderAddr()->push_back(CCopasiObjectName("String=\\[Best Parameters\\]"));

      // Body
      pReport->getBodyAddr()->push_back(CCopasiObjectName("CN=Root,Vector=TaskList[Parameter Estimation],Problem=Parameter Estimation,Reference=Function Evaluations"));
      pReport->getBodyAddr()->push_back(CCopasiObjectName("Separator=\t"));
      pReport->getBodyAddr()->push_back(CCopasiObjectName("CN=Root,Vector=TaskList[Parameter Estimation],Problem=Parameter Estimation,Reference=Best Value"));
      pReport->getBodyAddr()->push_back(CCopasiObjectName("Separator=\t"));
      pReport->getBodyAddr()->push_back(CCopasiObjectName("CN=Root,Vector=TaskList[Parameter Estimation],Problem=Parameter Estimation,Reference=Best Parameters"));

      // Footer
      pReport->getFooterAddr()->push_back(CCopasiObjectName("String=\n"));
      pReport->getFooterAddr()->push_back(CCopasiObjectName("CN=Root,Vector=TaskList[Parameter Estimation],Object=Result"));
      break;

      //**************************************************************************
    case CCopasiTask::lyap:
      pReport = new CReportDefinition(CCopasiTask::TypeName[taskType]);
      pReport->setTaskType(taskType);
      pReport->setComment("Automatically generated report.");
      pReport->setIsTable(false);
      pReport->setTitle(false);
      pReport->setSeparator(CCopasiReportSeparator("\t"));

      // Header
      pReport->getHeaderAddr()->push_back(CCopasiObjectName("CN=Root,Vector=TaskList[Lyapunov Exponents],Object=Description"));

      // Footer
      pReport->getFooterAddr()->push_back(CCopasiObjectName("String=\n"));
      pReport->getFooterAddr()->push_back(CCopasiObjectName("CN=Root,Vector=TaskList[Lyapunov Exponents],Object=Result"));
      break;

      //**************************************************************************
    case CCopasiTask::mca:
      pReport = new CReportDefinition(CCopasiTask::TypeName[taskType]);
      pReport->setTaskType(taskType);
      pReport->setComment("Automatically generated report.");
      pReport->setIsTable(false);
      pReport->setTitle(false);
      pReport->setSeparator(CCopasiReportSeparator("\t"));

      // Header
      pReport->getHeaderAddr()->push_back(CCopasiObjectName("CN=Root,Vector=TaskList[Metabolic Control Analysis],Object=Description"));

      // Footer
      pReport->getFooterAddr()->push_back(CCopasiObjectName("String=\n"));
      pReport->getFooterAddr()->push_back(CCopasiObjectName("CN=Root,Vector=TaskList[Metabolic Control Analysis],Object=Result"));
      break;

    default:
      return pReport;
    }

  if (pReport) mpReportDefinitionList->add(pReport, true);

  return pReport;
}

bool CCopasiDataModel::addDefaultReports()
{
  unsigned C_INT32 i;
  for (i = 0; CCopasiTask::TypeName[i] != ""; i++)
    {
      //try to create the report if it doesn't exist
      if (mpReportDefinitionList->getIndex(CCopasiTask::TypeName[i]) == C_INVALID_INDEX)
        {
          addReport((CCopasiTask::Type) i);
        }

      //see if the report exists now
      CReportDefinition* pReportDef = NULL;
      if (mpReportDefinitionList->getIndex(CCopasiTask::TypeName[i]) != C_INVALID_INDEX)
        pReportDef = (*mpReportDefinitionList)[CCopasiTask::TypeName[i]];

      //see if the task exists
      CCopasiTask* pTask = NULL;
      if (mpTaskList->getIndex(CCopasiTask::TypeName[i]) != C_INVALID_INDEX)
        pTask = (*mpTaskList)[CCopasiTask::TypeName[i]];

      if (pTask && pReportDef) //task and report definition exist
        {
          //if there is no report definition set the default
          if (!pTask->getReport().getReportDefinition())
            {
              pTask->getReport().setReportDefinition(pReportDef);
            }

          //TODO: also set the default report if no target file is set
          //even if a report is already set?
        }
    }
  return true;
}

CReportDefinitionVector * CCopasiDataModel::getReportDefinitionList()
{return mpReportDefinitionList;}

COutputDefinitionVector * CCopasiDataModel::getPlotDefinitionList()
{return mpPlotDefinitionList;}

CFunctionDB * CCopasiDataModel::getFunctionList()
{return mpFunctionList;}

SCopasiXMLGUI * CCopasiDataModel::getGUI()
{return mpGUI;}

CConfigurationFile * CCopasiDataModel::getConfiguration()
{return mpConfiguration;}

CVersion * CCopasiDataModel::getVersion()
{return mpVersion;}

const std::string & CCopasiDataModel::getFileName() const
  {return mSaveFileName;}

bool CCopasiDataModel::isChanged() const
  {return mChanged;}

void CCopasiDataModel::changed(const bool & changed)
{
  mChanged = changed;
  mAutoSaveNeeded = changed;
}

SBMLDocument* CCopasiDataModel::getCurrentSBMLDocument()
{
  return this->mpCurrentSBMLDocument;
}

bool CCopasiDataModel::setSBMLFileName(const std::string & fileName)
{
  mSBMLFileName = fileName;

  if (CDirEntry::isRelativePath(mSBMLFileName) &&
      !CDirEntry::makePathAbsolute(mSBMLFileName, mSaveFileName))
    mSBMLFileName = CDirEntry::fileName(mSBMLFileName);

  return true;
}

const std::string & CCopasiDataModel::getSBMLFileName() const
{return mSBMLFileName;}

std::map<CCopasiObject*, SBase*>& CCopasiDataModel::getCopasi2SBMLMap()
{
  return this->mCopasi2SBMLMap;
}

void CCopasiDataModel::removeSBMLIdFromFunctions()
{
  CFunctionDB* pFunDB = CCopasiDataModel::Global->getFunctionList();
  unsigned int i, iMax = pFunDB->loadedFunctions().size();
  for (i = 0;i < iMax;++i)
    {
      pFunDB->loadedFunctions()[i]->setSBMLId("");
    }
}
