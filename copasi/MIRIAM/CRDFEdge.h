// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/MIRIAM/Attic/CRDFEdge.h,v $
//   $Revision: 1.2 $
//   $Name:  $
//   $Author: shoops $
//   $Date: 2007/11/21 16:15:07 $
// End CVS Header

// Copyright (C) 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#ifndef COPASI_CRDFEdge
#define COPASI_CRDFEdge

#include <string>

class CRDFNode;

class CRDFEdge
  {
    // Operations
  public:
    /**
     * Default Constructor
     */
    CRDFEdge();

    /**
     * Copy Constructor
     * @param const CRDFEdge & src
     */
    CRDFEdge(const CRDFEdge & src);

    /**
     * Specific Constructor
     * @param const std::string & predicate
     * @param CRDFNode * pPropertyNode
     */
    CRDFEdge(const std::string & predicate,
             CRDFNode * pPropertyNode);

    /**
     * Destructor
     */
    virtual ~CRDFEdge();

    /**
     * Set the URI describing the predicate of the edge
     * @param const std::string & predicate
     */
    void setPredicate(const std::string & predicate);

    /**
     * Retrieve the predicate of the of the edge
     * @return const std::string & predicate
     */
    const std::string & getPredicate() const;

    /**
     * Set the pointer to the property node
     * @param CRDFNode * pPropertyNode
     */
    void setProprtyNode(CRDFNode * pPropertyNode);

    /**
     * Retrieve the pointer to the property node
     * @return CRDFNode * pPropertyNode
     */
    CRDFNode * getPropertyNode();

    // Attributes
  private:
    /**
     * The predicate of the edge
     */
    std::string mPredicate;

    /**
     * A pointer to the property node
     */
    CRDFNode * mpPropertyNode;
  };

#endif // COPASI_CRDFEdge