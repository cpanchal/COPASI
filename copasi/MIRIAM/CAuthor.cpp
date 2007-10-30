// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/MIRIAM/Attic/CAuthor.cpp,v $
//   $Revision: 1.1 $
//   $Name:  $
//   $Author: aekamal $
//   $Date: 2007/10/27 01:45:50 $
// End CVS Header

// Copyright (C) 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#include "CAuthor.h"

CAuthor::CAuthor(const std::string & familyName, const std::string & givenName) :
    mEmail(""), mURL("")
{
}

const std::string & CAuthor::getFamilyName() const
  {return mFamilyName;}

const std::string & CAuthor::getGivenName() const
  {return mGivenName;}

const std::string & CAuthor::getEmail() const
  {return mEmail;}

const std::string & CAuthor::getURL() const
  {return mURL;}

std::string CAuthor::getFullName() const
  {return mGivenName + " " + mFamilyName;}