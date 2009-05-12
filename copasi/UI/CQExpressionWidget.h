// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/UI/CQExpressionWidget.h,v $
//   $Revision: 1.15 $
//   $Name:  $
//   $Author: shoops $
//   $Date: 2009/05/12 16:46:57 $
// End CVS Header

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

// Copyright (C) 2001 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#ifndef CQEXPRESSION
#define CQEXPRESSION

#include <q3textedit.h>
#include <q3syntaxhighlighter.h>
//Added by qt3to4:
#include <QKeyEvent>

#include <vector>

#include "UI/CQValidator.h"
#include "UI/CCopasiSimpleSelectionTree.h"

#include "function/CExpression.h"

class CQExpressionWidget;
class CCopasiObject;

/*!
    \brief The class for highlighting the expression syntax
 */
class CQExpressionHighlighter: public Q3SyntaxHighlighter
{
public:
  CQExpressionHighlighter(CQExpressionWidget* ew);
  ~CQExpressionHighlighter() {};

  virtual int highlightParagraph(const QString & text, int endStateOfLastPara);
};

/*!
    \brief The class for checking the validity of a given mathematical expression
 */
class CQValidatorExpression: public CQValidator< Q3TextEdit >
{
public:
  CQValidatorExpression(Q3TextEdit * parent, const char * name = 0, bool isBoolean = false);

  /**
   * Function to validate a string input
   */
  virtual State validate(QString & input, int & pos) const;

  /**
   * Function to get CExpression object
   */
  virtual CExpression *getExpression();

  void setBoolean(bool isBoolean) {mExpression.setBoolean(isBoolean);};

protected:
  CExpression mExpression;
};

/*!
    \brief The class for writing/editing a mathematical expression
 */
class CQExpressionWidget: public Q3TextEdit
{
  Q_OBJECT
public:
  CQExpressionWidget(QWidget * parent = 0, const char * name = 0, bool isBoolean = false);

  CQValidatorExpression * mpValidator;

protected:
  int mOldPar;
  int mOldPos;

  int mOldPar1;
  int mOldPos1;
  int mOldPar2;
  int mOldPos2;

  /**
   * Function to indicate whether we are dealing with an INITIAL or TRANSIENT expression
   */
  CCopasiSimpleSelectionTree::ObjectClasses mObjectClasses;

  std::map< std::string, const CCopasiObject * > mParseList;
  const CCopasiObject * mpCurrentObject;
  QString mNewName;

  QColor mSavedColor;
  QColor mChangedColor;

  virtual void keyPressEvent(QKeyEvent * e);

  bool isInObject();
  bool isInObject(int par, int pos);

  /**
   * returns true if (par/pos) is right of (parold/posold)
   */
  bool compareCursorPositions(int parold, int posold, int par, int pos);

public:
  enum ExpressionType
  {
    InitialExpression = CCopasiSimpleSelectionTree::InitialTime |
    CCopasiSimpleSelectionTree::Parameters |
    CCopasiSimpleSelectionTree::ObservedConstants,
    TransientExpression = CCopasiSimpleSelectionTree::InitialTime |
    CCopasiSimpleSelectionTree::Parameters |
    CCopasiSimpleSelectionTree::ObservedConstants |
    CCopasiSimpleSelectionTree::Time |
    CCopasiSimpleSelectionTree::Variables |
    CCopasiSimpleSelectionTree::ObservedValues,
    ObjectiveFunctions = CCopasiSimpleSelectionTree::InitialTime |
    CCopasiSimpleSelectionTree::Parameters |
    CCopasiSimpleSelectionTree::ObservedConstants |
    CCopasiSimpleSelectionTree::Time |
    CCopasiSimpleSelectionTree::Variables |
    CCopasiSimpleSelectionTree::ObservedValues |
    CCopasiSimpleSelectionTree::Results
  };
  /**
   * Set the expression for the widget
   * @param const std::string & expression
   */
  void setExpression(const std::string & expression);

  /**
   * Retrieve the expression from the widget in string format
   * @return std::string expression
   */
  std::string getExpression() const;

  /**
   * Set the type of the current expression
   * @param const ExpressionType & expressionType
   */
  void setExpressionType(const ExpressionType & expressionType);

  void setBoolean(bool isBoolean);

  bool isValid();

protected slots:
  void slotCursorPositionChanged(int para, int pos);
  void slotSelectionChanged();

  /**
   * Slot for being activated whenever the text on Expression Widget is changed
   */
  void slotTextChanged();

public slots:
  void doKeyboardAction(Q3TextEdit::KeyboardAction action);
  void slotSelectObject();

signals:
  void valid(bool valid);
};

#endif
