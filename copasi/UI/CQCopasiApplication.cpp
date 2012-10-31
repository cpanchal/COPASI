// Copyright (C) 2012 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

#include <QFileOpenEvent>
#include <QString>

#include "CQCopasiApplication.h"
#include "copasiui3window.h"
#include "listviews.h"

#ifdef WIN32

#include <QWindowsVistaStyle>

bool IsWindows8()
{

  OSVERSIONINFOEX osvi = {0};
  DWORDLONG mask = 0;

  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  osvi.dwMajorVersion = 6;
  osvi.dwMinorVersion = 2;

  VER_SET_CONDITION(mask, VER_MAJORVERSION, VER_GREATER_EQUAL);
  VER_SET_CONDITION(mask, VER_MINORVERSION, VER_GREATER_EQUAL);

  return VerifyVersionInfo(&osvi,
                           VER_MAJORVERSION | VER_MINORVERSION,
                           mask
                          );
}

#endif

CQCopasiApplication::CQCopasiApplication(int & argc, char ** argv):
  QApplication(argc, argv),
  mpMainWindow(NULL),
  mFile(),
  mStarting(true)
{
#if WIN32

  if (IsWindows8())
    setStyle(new QWindowsVistaStyle);

#endif
}

CQCopasiApplication::~CQCopasiApplication()
{
}

// virtual
bool CQCopasiApplication::event(QEvent * pEvent)
{
  switch (pEvent->type())
    {
      case QEvent::FileOpen:

        if (mStarting)
          {
            mFile = static_cast<QFileOpenEvent *>(pEvent)->file();
          }
        else
          {
            // need to take the new file, otherwise whenever the application
            // is open we will re-open the first file that was supposed to be
            // opened.
            mFile = static_cast<QFileOpenEvent *>(pEvent)->file();
            mpMainWindow->slotFileOpen(mFile);
          }

        pEvent->accept();
        return true;

        break;

      default:
        break;
    }

  return QApplication::event(pEvent);
}

void CQCopasiApplication::setMainWindow(CopasiUI3Window * pMainWindow)
{
  mpMainWindow = pMainWindow;

  processEvents();

  mpMainWindow->openInitialDocument(mFile);
  mStarting = false;
}
