// Copyright © 2005 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

%{

#include "report/CCopasiContainer.h"

%}



%ignore CCopasiContainer::CCopasiContainer(const std::string& name,
                                           const CCopasiContainer * pParent = NULL,
                                           const std::string & type = "CN",
                                           const unsigned C_INT32 & flag);
%immutable CCopasiContainer::Root;


%immutable CCopasiContainer::Root;

class CCopasiContainer: public CCopasiObject
{
  private:
    CCopasiContainer();

  public:
    static CCopasiContainer* Root;

    // we need to cast the object that is returned to the correct type.
    // I will have to look at the code in libsbml that does the same for the listOf class
    static CCopasiObject* ObjectFromName(const CCopasiObjectName& objName);
    static void init();

    CCopasiContainer(const std::string& name,
                     const CCopasiContainer * pParent = NULL,
                     const std::string & type = "CN",
                     const unsigned C_INT32 & flag = CCopasiObject::Container);

    virtual ~CCopasiContainer();
    

};


