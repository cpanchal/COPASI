/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/elementaryFluxModes/CFluxScore.h,v $
   $Revision: 1.1.1.1 $
   $Name:  $
   $Author: anuragr $ 
   $Date: 2004/10/26 15:17:54 $
   End CVS Header */

/**
 *  CFluxScore class.
 *  Used to compare flux modes
 *
 *  Created for Copasi by Stefan Hoops 2002-05-08
 * (C) Stefan Hoops 2002
 */

#ifndef COPASI_CFluxScore
#define COPASI_CFluxScore

#include <vector>

class CFluxScore
  {
    // Attributes

  private:
    /**
     *  The score of a flux mode
     */
    std::vector <int> mScore;

    // Operations

  public:
    /**
     *  Default constructor
     */
    CFluxScore();

    /**
     *  Specific constructor
     *  @param "const vector < C_FLOAT64 > &" fluxModes
     */
    CFluxScore(const std::vector < C_FLOAT64 > & fluxModes);

    /**
     *  Destructor();
     */
    ~CFluxScore();

    /**
     *  Comparison operator deciding whether lhs has a smaller flux score than
     *  rhs
     *  @param "const CFluxScore &" lhs
     *  @param "const CFluxScore &" rhs
     *  @return "bool" 
     */
    bool operator < (const CFluxScore & rhs) const;
  };

#endif // COPASI_CFluxScore
