/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/CopasiUI/Attic/TimeSeriesSubwidget.ui.h,v $
   $Revision: 1.1.1.1 $
   $Name:  $
   $Author: anuragr $ 
   $Date: 2004/10/26 15:17:51 $
   End CVS Header */

/****************************************************************************
 ** ui.h extension file, included from the uic-generated form implementation.
 **
 ** If you want to add, delete, or rename functions or slots, use
 ** Qt Designer to update this file, preserving your code.
 **
 ** You should not define a constructor or destructor in this file.
 ** Instead, write your code in functions called init() and destroy().
 ** These will automatically be called by the form's constructor and
 ** destructor.
 *****************************************************************************/

void TimeSeriesSubWidget::saveDataToFile()
{
  const std::string& fileName = QFileDialog::getSaveFileName(QString::null, QString::null, this, 0, "Save to").latin1();
  const CTimeSeries* timeSeries = this->table()->getTimeSeries();
  int failed = 0;
  if (timeSeries)
    {
      failed = timeSeries->save(fileName, !(this->table()->doShowConcentrations()), "\t");
    }
  if (failed)
    {
      std::string s = "Could not save data to ";
      s += fileName;
      QMessageBox::critical(this, "Save Error", s.c_str(), QMessageBox::Ok, QMessageBox::Cancel);
    }
}

void TimeSeriesSubWidget::toggleView()
{
  if (comboBox->currentItem() == 0)
    dataTable->showConcentrations(true);
  else
    dataTable->showConcentrations(false);
}

void TimeSeriesSubWidget::init()
{
  dataTable->setNumRows(10);
}

CTimeSeriesTable* TimeSeriesSubWidget::table()
{
  return dataTable;
}
