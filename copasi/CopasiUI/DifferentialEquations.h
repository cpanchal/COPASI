/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/CopasiUI/Attic/DifferentialEquations.h,v $
   $Revision: 1.1.1.1 $
   $Name:  $
   $Author: anuragr $ 
   $Date: 2004/10/26 15:17:47 $
   End CVS Header */

/****************************************************************************
 **  $ CopasiUI/DifferentialEquations.h               
 **  $ Author  : Mudita Singhal
 **  
 ** This is the header file for the Differential Equations
 *****************************************************************************/

#ifndef DIFFERENTIAL_EQUATIONS_H
#define DIFFERENTIAL_EQUATIONS_H
#include <qpushbutton.h>
#include <qtextbrowser.h>

#include "copasi.h"
#include "copasiWidget.h"
class CMathModel;

class DifferentialEquations : public CopasiWidget
  {
    Q_OBJECT

  protected:
    QTextBrowser *textBrowser;
    //QPushButton *btnOK;
    //QPushButton *btnCancel;

  public:
    DifferentialEquations(QWidget *parent, const char * name = 0, WFlags f = 0);

    virtual bool update(ListViews::ObjectType objectType, ListViews::Action action, const std::string & key);
    virtual bool enter(const std::string & key = "");

  protected slots:
    virtual void slotBtnOKClicked();
    virtual void slotBtnCancelClicked();

  private:
    void loadDifferentialEquations(CMathModel * mathModel);
  };

#endif
