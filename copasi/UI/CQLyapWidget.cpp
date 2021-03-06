// Copyright (C) 2010 - 2013 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// Copyright (C) 2008 - 2009 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

// Copyright (C) 2006 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#include "CQLyapWidget.h"

#include <QtGui/QMessageBox>

#include "CQTaskBtnWidget.h"
#include "CQTaskHeaderWidget.h"
#include "CQTaskMethodWidget.h"
#include "CProgressBar.h"

#include "lyap/CLyapTask.h"
#include "lyap/CLyapProblem.h"
#include "model/CModel.h"
#include "report/CKeyFactory.h"
#include "utilities/CCopasiException.h"
#include "report/CCopasiRootContainer.h"

/*
 *  Constructs a CQLyapWidget which is a child of 'parent', with the
 *  name 'name'.'
 */
CQLyapWidget::CQLyapWidget(QWidget* parent, const char* name)
  : TaskWidget(parent, name)
{
  setupUi(this);

  init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
CQLyapWidget::~CQLyapWidget()
{
  // no need to delete child widgets, Qt does it all for us
}

bool CQLyapWidget::runTask()
{
  CLyapTask * pTask =
    dynamic_cast< CLyapTask * >(CCopasiRootContainer::getKeyFactory()->get(mKey));

  if (!pTask) return false;

  if (!commonBeforeRunTask()) return false;

  return commonRunTask();
}

bool CQLyapWidget::taskFinishedEvent()
{
  mpListView->switchToOtherWidget(261, ""); //change to the results window
  return true;
}

CCopasiMethod * CQLyapWidget::createMethod(const CCopasiMethod::SubType & type)
{return CLyapMethod::createMethod(type);}

bool CQLyapWidget::loadTask()
{
  CLyapTask * pTask = dynamic_cast< CLyapTask * >(mpTask);

  if (!pTask) return false;

  loadCommon();
  loadMethod();

  CLyapProblem* pProblem =
    dynamic_cast< CLyapProblem * >(mpTask->getProblem());

  if (!pProblem) return false;

  mpEditExponent->setText(QString::number(pProblem->getExponentNumber()));

  assert(CCopasiRootContainer::getDatamodelList()->size() > 0);
  bool enabled =
    ((*CCopasiRootContainer::getDatamodelList())[0]->getModel()->getInitialTime() != pProblem->getTransientTime());

  mpCheckDelay->setChecked(enabled);
  mpEditDelay->setEnabled(enabled);
  mpEditDelay->setText(QString::number(pProblem->getTransientTime()));

  mpCheckDivergence->setChecked(pProblem->divergenceRequested());

  mChanged = false;

  return true;
}

bool CQLyapWidget::saveTask()
{
  CLyapTask * pTask = dynamic_cast< CLyapTask * >(mpTask);

  if (!pTask) return false;

  saveCommon();
  saveMethod();

  CLyapProblem* pProblem =
    dynamic_cast< CLyapProblem * >(mpTask->getProblem());

  if (!pProblem) return false;

  if (QString::number(pProblem->getExponentNumber()) != mpEditExponent->text())
    {
      pProblem->setExponentNumber(std::max(0, mpEditExponent->text().toInt()));
      mChanged = true;
    }

  assert(CCopasiRootContainer::getDatamodelList()->size() > 0);
  bool enabled =
    ((*CCopasiRootContainer::getDatamodelList())[0]->getModel()->getInitialTime() != pProblem->getTransientTime());

  if (mpCheckDelay->isChecked() != enabled ||
      (mpCheckDelay->isChecked() &&
       mpEditDelay->text() != QString::number(pProblem->getTransientTime())))
    {
      if (mpCheckDelay->isChecked())
        pProblem->setTransientTime(mpEditDelay->text().toDouble());
      else
        pProblem->setTransientTime((*CCopasiRootContainer::getDatamodelList())[0]->getModel()->getInitialTime());

      mChanged = true;
    }

  if (mpCheckDivergence->isChecked() != pProblem->divergenceRequested())
    {
      pProblem->setDivergenceRequested(mpCheckDivergence->isChecked());
      mChanged = true;
    }

  if (mChanged)(*CCopasiRootContainer::getDatamodelList())[0]->changed();

  mChanged = false;
  return true;
}

void CQLyapWidget::init()
{
  mpHeaderWidget->setTaskName("Lyapunov Exponents");

  vboxLayout->insertWidget(0, mpHeaderWidget);  // header
  // vboxLayout->insertSpacing(1, 14);       // space between header and body

  mpMethodWidget->showMethodParameters(true);
  vboxLayout->addWidget(mpMethodWidget);

  vboxLayout->addWidget(mpBtnWidget);     // 'footer'
}

void CQLyapWidget::slotDelayChecked()
{
  mpEditDelay->setEnabled(mpCheckDelay->isChecked());
}
