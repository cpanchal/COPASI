/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/model/CMoiety.h,v $
   $Revision: 1.1.1.1 $
   $Name:  $
   $Author: anuragr $ 
   $Date: 2004/10/26 15:17:58 $
   End CVS Header */

/**
 *  CMoiety
 *  
 *  New class created for Copasi by Stefan Hoops
 *  (C) Stefan Hoops 2001
 */

#ifndef COPASI_CMoiety
#define COPASI_CMoiety

#include <string>
#include <vector>

#include "CMetab.h"
#include "CChemEqElement.h"
#include "utilities/CCopasiVector.h"

/** @dia:pos 52.0066,117.162 */
class CMoiety : public CCopasiContainer
  {
    // Attributes
  private:
    /**
     * The key of the moiety
     */
    std::string mKey; //By G

    /**
     *  Number of Particles of Moietiy.
     */
    C_FLOAT64 mNumber;

    /**
     *  Initial Number of Particles of Moietiy.
     */
    C_FLOAT64 mINumber;

    /**
     *  Vector of linear dependent CChemEqElement
     * @supplierCardinality 0..*
     */
    /** @dia:route 7,3; h,41.0337,110.831,46.5202,117.862,52.0066 */
    CCopasiVector < CChemEqElement > mEquation;

    // Operations
  public:
    /**
     * Default constructor
     * @param const std::string & name (default: "NoName")
     * @param const CCopasiContainer * pParent (default: NULL)
     */
    CMoiety(const std::string & name = "NoName",
            const CCopasiContainer * pParent = NULL);

    /**
     * Copy constructor
     * @param "const CMoiety &" src
     * @param const CCopasiContainer * pParent (default: NULL)
     */
    CMoiety(const CMoiety & src,
            const CCopasiContainer * pParent = NULL);

    /**
     *  Destructor
     */
    ~CMoiety();

    /**
     *  Saves the contents of the object to a CWriteConfig object.
     *  This saves the data in Gepasi 3.21 file format
     *  @param pconfigbuffer reference to a CWriteConfig object.
     *  @return Fail
     */ 
    //    C_INT32 saveOld(CWriteConfig & configBuffer);

    /**
     * Add a metabolite to a moiety
     * @param C_FLOAT64 value
     * @param CMetab * metabolite
     */
    void add(C_FLOAT64 value, CMetab * metabolite);

    /**
     *
     */
    void cleanup();

    /**
     *
     */
    void cleanup(const std::string & name);

    /**
     *
     */
    void cleanup(C_INT32 index);

    /**
     *
     */
    void change(C_INT32 index,
                C_FLOAT64 value);

    /**
     *
     */
    void change(const std::string & name,
                C_FLOAT64 value);

    /**
     *
     */
    bool setName(const std::string name);

    /**
     *
     */
    void setInitialValue();

    /**
     * get the string representation of the moiety
     */
    std::string getDescription() const;

    /**
     * get the string representation of the moiety using the CMetabNameInterface
     */
    std::string getDescription(const CModel* model) const;

    /**
     *
     */
    C_FLOAT64 dependentNumber();

    /**
     *
     */
    C_FLOAT64 dependentRate();

    /**
     *
     */
    C_FLOAT64 getNumber() const;

    /**
     * Returns the address of mNumber
     */
    void * getNumberAddr();

    /**
        *  Returns a string with the name of this compartment.
        *  @return std::string key
        */
    virtual const std::string & getKey() const; //By G
  };

#endif // COPASI_CMoiety
