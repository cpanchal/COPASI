// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/UI/CQGlobalQuantitiesWidget.cpp,v $
//   $Revision: 1.2 $
//   $Name:  $
//   $Author: aekamal $
//   $Date: 2009/06/12 19:58:25 $
// End CVS Header

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

#include <QHeaderView>

#include "model/CModel.h"
#include "CopasiDataModel/CCopasiDataModel.h"
#include "report/CCopasiRootContainer.h"

#include "CQGlobalQuantitiesWidget.h"
#include "qtUtilities.h"
#include "copasi.h"
#include "CQMessageBox.h"

/*
 *  Constructs a CQGlobalQuantitiesWidget which is a child of 'parent', with the
 *  name 'name'.'
 */
CQGlobalQuantitiesWidget::CQGlobalQuantitiesWidget(QWidget* parent, const char* name)
    : CopasiWidget(parent, name)
{
  setupUi(this);

  //Create Source Data Model.
  mpGlobalQuantityDM = new CQGlobalQuantityDM(this);

  //Create the Proxy Model for sorting/filtering and set its properties.
  mpProxyModel = new CQSortFilterProxyModel();
  mpProxyModel->setDynamicSortFilter(true);
  mpProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
  mpProxyModel->setFilterKeyColumn(COL_NAME_GQ);

  //Setting values for Types comboBox
  mpTypeDelegate = new CQIndexComboDelegate(&mpGlobalQuantityDM->getTypes(), this);
  mpTblGlobalQuantities->setItemDelegateForColumn(COL_TYPE_GQ, mpTypeDelegate);

  mpTblGlobalQuantities->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  mpTblGlobalQuantities->verticalHeader()->hide();
  mpTblGlobalQuantities->sortByColumn(COL_ROW_NUMBER, Qt::AscendingOrder);

  // Connect the table widget
  connect(mpGlobalQuantityDM, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
          this, SLOT(dataChanged(const QModelIndex&, const QModelIndex&)));
  connect(mpLEFilter, SIGNAL(textChanged(const QString &)),
          this, SLOT(slotFilterChanged()));
}

/*
 *  Destroys the object and frees any allocated resources
 */
CQGlobalQuantitiesWidget::~CQGlobalQuantitiesWidget()
{
  pdelete(mpTypeDelegate);
  pdelete(mpProxyModel);
  pdelete(mpGlobalQuantityDM);
  // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void CQGlobalQuantitiesWidget::languageChange()
{
  retranslateUi(this);
}

void CQGlobalQuantitiesWidget::slotBtnDeleteClicked()
{
  if (mpTblGlobalQuantities->hasFocus())
    {deleteSelectedGlobalQuantities();}
}

void CQGlobalQuantitiesWidget::deleteSelectedGlobalQuantities()
{
  QModelIndexList selRows = mpTblGlobalQuantities->selectionModel()->selectedRows(0);

  if (selRows.empty())
    {return;}

  QModelIndexList mappedSelRows;
  QModelIndexList::const_iterator i;

  for (i = selRows.begin(); i != selRows.end(); ++i)
    {mappedSelRows.append(mpProxyModel->mapToSource(*i));}

  if (mpGlobalQuantityDM->removeRows(mappedSelRows))
    protectedNotify(ListViews::MODELVALUE, ListViews::DELETE, "");
}

void CQGlobalQuantitiesWidget::slotBtnClearClicked()
{

  int ret = QMessageBox::question(this, tr("Confirm Delete"), "Delete all Quantities?",
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

  if (ret == QMessageBox::Yes)
    {
      mpGlobalQuantityDM->clear();
      protectedNotify(ListViews::MODELVALUE, ListViews::DELETE, "");
    }
}

bool CQGlobalQuantitiesWidget::update(ListViews::ObjectType C_UNUSED(objectType), ListViews::Action C_UNUSED(action), const std::string & C_UNUSED(key))
{
  return true;
}

bool CQGlobalQuantitiesWidget::leave()
{
  return true;
}

bool CQGlobalQuantitiesWidget::enter(const std::string & C_UNUSED(key))
{
  mpProxyModel->setSourceModel(mpGlobalQuantityDM);
  //Set Model for the TableView
  mpTblGlobalQuantities->setModel(NULL);
  mpTblGlobalQuantities->setModel(mpProxyModel);
  mpTblGlobalQuantities->resizeColumnsToContents();

  return true;
}

void CQGlobalQuantitiesWidget::dataChanged(const QModelIndex& C_UNUSED(topLeft),
    const QModelIndex& C_UNUSED(bottomRight))
{
  mpTblGlobalQuantities->resizeColumnsToContents();
  protectedNotify(ListViews::MODELVALUE, ListViews::CHANGE, "");
  protectedNotify(ListViews::MODEL, ListViews::CHANGE, "");
}

void CQGlobalQuantitiesWidget::slotDoubleClicked(const QModelIndex proxyIndex)
{
  QModelIndex index = mpProxyModel->mapToSource(proxyIndex);

  if (mpGlobalQuantityDM->isDefaultRow(index))
    return;

  assert(CCopasiRootContainer::getDatamodelList()->size() > 0);
  CCopasiDataModel* pDataModel = (*CCopasiRootContainer::getDatamodelList())[0];
  assert(pDataModel != NULL);
  CModel * pModel = pDataModel->getModel();

  if (pModel == NULL)
    return;

  std::string key = pModel->getModelValues()[index.row()]->getKey();

  if (CCopasiRootContainer::getKeyFactory()->get(key))
    mpListView->switchToOtherWidget(0, key);
}

void CQGlobalQuantitiesWidget::keyPressEvent(QKeyEvent* ev)
{
  if (ev->key() == Qt::Key_Delete)
    slotBtnDeleteClicked();
}

void CQGlobalQuantitiesWidget::slotFilterChanged()
{
  QRegExp regExp(mpLEFilter->text() + "|No Name", Qt::CaseInsensitive, QRegExp::RegExp);
  mpProxyModel->setFilterRegExp(regExp);
}