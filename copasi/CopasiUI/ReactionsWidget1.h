/****************************************************************************
 **  $ CopasiUI/ReactionsWidget1.h               
 **  $ Author  : Mudita Singhal
 **  
 ** This is the header file for the Reactions Widget, i.e the second level 
 ** of Reactions.
 *****************************************************************************/

#ifndef REACTIONS_WIDGET1_H
#define REACTIONS_WIDGET1_H
#include <qsplitter.h>
#include <qvariant.h>
#include <qlayout.h>

class QAction;

class QActionGroup;

class QToolBar;

class QPopupMenu;

class QLabel;

class QCheckBox;

class QLineEdit;

class QComboBox;

class QListBoxItem;

class QFrame;

class QRect;
#include "qtable.h"
#include "MyTable.h"

class CModel;

class QPushButton;

class ReactionsWidget1 : public QWidget
  {
    Q_OBJECT

  protected:
    QPushButton* cancelChanges;
    QPushButton* commitChanges;
    QPushButton* newKinetics;
    CModel *mModel;
    QTable *table;

  public:
    ReactionsWidget1(QWidget *parent, const char * name = 0, WFlags f = 0);
    void loadName(QString setValue);
    int isName(QString setValue);
    void loadReactions(CModel *model);
    QString *Reaction1_Name;

    QFrame* Line1;
    QFrame* Line2;

    QLineEdit* LineEdit1;
    QLineEdit* LineEdit2;
    QLineEdit* LineEdit3;

    QLabel* TextLabel1;
    QLabel* TextLabel2;
    QLabel* TextLabel3;
    QLabel* TextLabel4;
    QLabel* TextLabel5;
    QLabel* TextLabel6;

    QFrame* Frame1;
    QFrame* Frame3;
    QFrame* Frame4a;
    QFrame* Frame4b;
    QFrame* Frame4c;
    QFrame* Frame4d;
    QFrame* Frame4e;
    QFrame* Frame4f;
    QFrame* Frame4g;
    QFrame* Frame4h;

    QCheckBox* checkBox;

    QComboBox* ComboBox1;

  protected slots:
    virtual void slotBtnCancelClicked();
    virtual void slotBtnOKClicked();

  signals:
    void signal_emitted(QString &);
  };

#endif
