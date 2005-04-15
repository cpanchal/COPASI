/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/CopasiUI/Attic/StateSubwidget.h,v $
   $Revision: 1.6 $
   $Name:  $
   $Author: ssahle $ 
   $Date: 2005/04/15 11:59:06 $
   End CVS Header */

/****************************************************************************
 ** Form interface generated from reading ui file 'StateSubwidget.ui'
 **
 ** Created: Fri Apr 15 13:53:25 2005
 **      by: The User Interface Compiler ($Id: StateSubwidget.h,v 1.6 2005/04/15 11:59:06 ssahle Exp $)
 **
 ** WARNING! All changes made in this file will be lost!
 ****************************************************************************/

#ifndef STATESUBWIDGET_H
#define STATESUBWIDGET_H

#include <qvariant.h>
#include <qwidget.h>
#include "model/CState.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QTabWidget;
class QTable;
class QSplitter;
class QTextEdit;
class CModel;
class CSteadyStateTask;

class StateSubwidget : public QWidget
  {
    Q_OBJECT

  public:
    StateSubwidget(QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
    ~StateSubwidget();

    QLabel* topLabel;
    QTabWidget* tabWidget;
    QWidget* tab;
    QTable* concentrationsTable;
    QWidget* tab_2;
    QTable* numbersTable;
    QWidget* TabPage;
    QTable* tableFlux;
    QWidget* TabPage_2;
    QSplitter* splitterJacobian;
    QTable* tableJacobian;
    QLabel* textLabelJacobian;
    QTable* tableEigenValues;
    QLabel* textLabelEigenvalues;
    QWidget* TabPage_3;
    QSplitter* splitterJacobianX;
    QTable* tableJacobianX;
    QLabel* textLabelJacobianX;
    QTable* tableEigenValuesX;
    QLabel* textLabelEigenvaluesX;
    QWidget* TabPage_4;
    QTextEdit* stabilityTextEdit;

    virtual bool loadMetabolites(const CModel * model);
    virtual bool loadReactions(const CModel * model);
    virtual bool loadAll(const CSteadyStateTask * task);

  public slots:
    virtual void loadJacobian(const CSteadyStateTask * task);
    virtual void showUnits();

  protected:
    QVBoxLayout* StateSubwidgetLayout;
    QVBoxLayout* tabLayout;
    QVBoxLayout* tabLayout_2;
    QVBoxLayout* TabPageLayout;
    QVBoxLayout* TabPageLayout_2;
    QGridLayout* layoutJacobian;
    QSpacerItem* spacer3;
    QGridLayout* layoutEigenvalues;
    QSpacerItem* spacer4;
    QVBoxLayout* TabPageLayout_3;
    QGridLayout* layoutJacobianX;
    QSpacerItem* spacer3_2;
    QGridLayout* layoutEigenvaluesX;
    QSpacerItem* spacer4_2;
    QVBoxLayout* TabPageLayout_4;

  protected slots:
    virtual void languageChange();

    virtual void init();
  };

#endif // STATESUBWIDGET_H
