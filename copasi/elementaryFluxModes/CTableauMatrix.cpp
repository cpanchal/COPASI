/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/elementaryFluxModes/CTableauMatrix.cpp,v $
   $Revision: 1.1.1.1 $
   $Name:  $
   $Author: anuragr $ 
   $Date: 2004/10/26 15:17:54 $
   End CVS Header */

/**
 *  CTableauMatrix class.
 *  Used to calculate elementary flux modes
 *
 *  Created for Copasi by Stefan Hoops 2002-05-08
 * (C) Stefan Hoops 2002
 */

#include <iostream>

// #define COPASI_TRACE_CONSTRUCTION
#include "copasi.h"
#include "CTableauMatrix.h"

CTableauMatrix::CTableauMatrix()
{
  CONSTRUCTOR_TRACE;
  mFirstIrreversible = mLine.end();
}

CTableauMatrix::CTableauMatrix(const std::vector< std::vector< C_FLOAT64 > > & stoi,
                               C_INT32 reversibleNumber)
{
  CONSTRUCTOR_TRACE;
  unsigned C_INT32 ReactionCounter = 0;
  unsigned C_INT32 ReactionNumber = stoi.size();

  for (std::vector< std::vector< C_FLOAT64 > >::const_iterator Reaction = stoi.begin();
       Reaction < stoi.end();
       Reaction++, reversibleNumber--, ReactionCounter++)
    {
      mLine.push_back(new CTableauLine(*Reaction,
                                       reversibleNumber > 0 ? true : false,
                                       ReactionCounter,
                                       ReactionNumber));
      if (reversibleNumber == 0)
        {
          mFirstIrreversible = mLine.end();
          mFirstIrreversible--;
        }
    }
}

CTableauMatrix::~CTableauMatrix()
{
  DESTRUCTOR_TRACE;
  for (std::list< const CTableauLine * >::iterator i = mLine.begin();
       i != mLine.end(); i++)
    pdelete(*i);
}

std::list< const CTableauLine * >::iterator
CTableauMatrix::getFirst()
{
  return mLine.begin();
}

std::list< const CTableauLine * >::iterator
CTableauMatrix::getEnd()
{
  return mLine.end();
}

void CTableauMatrix::addLine(const CTableauLine * src)
{
  /* The first element in reaction is always 0 so we can remove it */
  /* This really breaks the "const" but it is the only place tableau */
  /* lines are changed */
  const_cast <CTableauLine *>(src)->truncate();

  /* First we check whether we have a valid new flux mode */
  if (isValid(src))
    {
      if (src->isReversible())
        {
          mFirstIrreversible = mLine.insert(mFirstIrreversible, src);
          mFirstIrreversible++;
        }
      else if (mFirstIrreversible == mLine.end())
        {
          mFirstIrreversible = mLine.insert(mFirstIrreversible, src);
        }
      else
        {
          mLine.push_back(src);
        }
    }
  else
    pdelete(src);
}

void CTableauMatrix::removeLine(const std::list< const CTableauLine * >::iterator line)
{
  if (line == mFirstIrreversible && mFirstIrreversible == mLine.begin())
    {
      mLine.pop_front();
      mFirstIrreversible = mLine.begin();
    }
  else if (line == mFirstIrreversible)
    {
      mFirstIrreversible--;
      mLine.erase(line);
      mFirstIrreversible++;
    }
  else
    {
      mLine.erase(line);
    }
}

bool CTableauMatrix::isValid(const CTableauLine * src)
{
  std::list< const CTableauLine * >::iterator i;
  std::list< const CTableauLine * >::iterator tmp;

  /* Check whether we have already better lines */
  for (i = mLine.begin(); i != mLine.end(); i++)
    if ((*i)->getScore() < src->getScore()) return false;

  i = mLine.begin();
  /* Check whether the new line scores better than existing lines */
  /* If so the existing lines are removed */
  for (i = mLine.begin(); i != mLine.end();)
    if (src->getScore() < (*i)->getScore())
      {
        if (i == mLine.begin())
          {
            removeLine(i);
            i = mLine.begin();
          }
        else
          {
            tmp = i;
            tmp--;
            removeLine(i);
            i = tmp;
            i++;
          }
      }
    else
      i++;

  return true;
}

#ifdef XXXX
void CTableauMatrix::print(void)
{
  cout << "Tableau Matrix: Number of Lines = " << mLine.size() << endl;
  std::list< const CTableauLine * >::iterator i;

  for (i = mLine.begin(); i != mLine.end(); i++)
    (*i)->print();
}
#endif // XXXX
