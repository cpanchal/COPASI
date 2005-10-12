/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/UI/CQFittingItemWidget.h,v $
   $Revision: 1.4 $
   $Name:  $
   $Author: shoops $ 
   $Date: 2005/10/12 13:26:30 $
   End CVS Header */

/****************************************************************************
 ** Form interface generated from reading ui file 'CQFittingItemWidget.ui'
 **
 ** Created: Tue Oct 11 17:57:42 2005
 **      by: The User Interface Compiler ($Id: CQFittingItemWidget.h,v 1.4 2005/10/12 13:26:30 shoops Exp $)
 **
 ** WARNING! All changes made in this file will be lost!
 ****************************************************************************/

#ifndef CQFITTINGITEMWIDGET_H
 #define CQFITTINGITEMWIDGET_H

#include <qvariant.h>
 #include <qwidget.h>
 #include <string>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QCheckBox;
class QLineEdit;
class QToolButton;
class QComboBox;
class CCopasiSelectionDialog;
class CFitItem;
class CCopasiObject;
class CCopasiObjectName;
class CQValidatorBound;
class CQValidatorNotEmpty;
class QColor;

class CQFittingItemWidget : public QWidget
  {
    Q_OBJECT

  public:
    CQFittingItemWidget(QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
    ~CQFittingItemWidget();

    QLabel* mpLblLower;
    QCheckBox* mpLowerInf;
    QLineEdit* mpEditLower;
    QToolButton* mpBtnLowerEdit;
    QLabel* mpLblUpper;
    QLineEdit* mpEditUpper;
    QToolButton* mpBtnUpperEdit;
    QCheckBox* mpUpperInf;
    QLabel* mpLblExperiments;
    QComboBox* mpBoxExperiments;
    QToolButton* mpBtnExperiments;
    QLabel* mpLblObject;
    QLineEdit* mpEditObject;
    QToolButton* mpBtnObject;

    bool loadFitItem(const CFitItem & item);
    bool saveFitItem(CFitItem & item);

  protected:
    QColor mChangedColor;
    QColor mSavedColor;
    bool mUpperInfChanged;
    bool mLowerInfChanged;
    CQValidatorBound * mpUpperValidator;
    CQValidatorBound * mpLowerValidator;
    CQValidatorNotEmpty * mpObjectValidator;
    const CCopasiObject* mpUpperObject;
    const CCopasiObject* mpLowerObject;
    CCopasiObjectName* mpObjectCN;
    CFitItem * mpFitItem;

    QGridLayout* CQFittingItemWidgetLayout;
    QSpacerItem* mpSpacerLeft;
    QHBoxLayout* mpLayoutLower;
    QHBoxLayout* mpLayoutUpper;
    QHBoxLayout* mpLayoutExperiments;
    QHBoxLayout* mpLayoutObject;

  protected slots:
    virtual void languageChange();

  private:
    void init();
    void destroy();

  private slots:
    void slotNegativeInfinity();
    void slotPositiveInfinity();
    void slotLowerEdit();
    void slotUpperEdit();
    void slotParamEdit();
    void slotExperiments();
  };

#endif // CQFITTINGITEMWIDGET_H
