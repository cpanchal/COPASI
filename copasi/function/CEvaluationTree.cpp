// Copyright (C) 2010 - 2013 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// Copyright (C) 2008 - 2009 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

// Copyright (C) 2005 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#include "copasi.h"

#include <sstream>
#include <vector>
#include <algorithm>

#include "CEvaluationNode.h"
#include "CEvaluationTree.h"
#include "CFunction.h"
#include "CExpression.h"
#include "CEvaluationLexer.h"
#include "CFunctionDB.h"

#include "report/CKeyFactory.h"
#include "report/CCopasiObjectReference.h"
#include "sbml/math/ASTNode.h"
#include "utilities/CCopasiTree.h"
#include "utilities/CNodeIterator.h"
#include "CopasiDataModel/CCopasiDataModel.h"
#include "report/CCopasiRootContainer.h"
#include "math/CMathObject.h"

const std::string CEvaluationTree::TypeName[] =
{
  "userdefined",
  "predefined",
  "predefined",
  "userdefined",
  "userdefined",
  "userdefined",
  "internal",
  ""
};

const char* CEvaluationTree::XMLType[] =
{
  "Function",
  "MassAction",
  "PreDefined",
  "UserDefined",
  "Expression",
  "Boolean",
  "MathExpression",
  NULL
};

CEvaluationTree *
CEvaluationTree::create(CEvaluationTree::Type type)
{
  CEvaluationTree * pNew = NULL;

  switch (type)
    {
      case Function:
        pNew = new CFunction();
        break;

      case MassAction:
        pNew = new CMassAction();
        break;

      case PreDefined:
        pNew = new CKinFunction();
        pNew->setType(PreDefined);
        break;

      case UserDefined:
        pNew = new CKinFunction();
        break;

      case Expression:
        pNew = new CExpression();
        break;

      default:
        fatalError();
        break;
    }

  return pNew;
}

CEvaluationTree *
CEvaluationTree::copy(const CEvaluationTree & src)
{
  CEvaluationTree * pNew = NULL;

  switch (src.getType())
    {
      case Function:
        pNew = new CFunction(*static_cast<const CFunction *>(&src));
        break;

      case MassAction:
        pNew = new CMassAction(*static_cast<const CMassAction *>(&src));
        break;

      case PreDefined:
      case UserDefined:
        pNew = new CKinFunction(*static_cast<const CKinFunction *>(&src));
        break;

      case Expression:
        pNew = new CExpression(*static_cast<const CExpression *>(&src));
        break;

      default:
        fatalError();
        break;
    }

  return pNew;
}

CEvaluationTree::CEvaluationTree(const std::string & name,
                                 const CCopasiContainer * pParent,
                                 const CEvaluationTree::Type & type):
  CCopasiContainer(name, pParent, "Function"),
  mType(type),
  mInfix(),
  mUsable(false),
  mErrorPosition(std::string::npos),
  mpNodeList(NULL),
  mpRoot(NULL),
  mValue(std::numeric_limits<C_FLOAT64>::quiet_NaN()),
  mCalculationSequence()
{
  initObjects();
  setInfix("");
}

CEvaluationTree::CEvaluationTree(const CEvaluationTree & src,
                                 const CCopasiContainer * pParent):
  CCopasiContainer(src, pParent),
  mType(src.mType),
  mInfix(),
  mUsable(false),
  mErrorPosition(std::string::npos),
  mpNodeList(NULL),
  mpRoot(NULL),
  mValue(src.mValue),
  mCalculationSequence()
{
  initObjects();
  setInfix(src.mInfix);
}

CEvaluationTree::~CEvaluationTree()
{
  CEvaluationLexer::freeNodeList(mpNodeList);
}

const CEvaluationTree::Type & CEvaluationTree::getType() const
{return mType;}

void CEvaluationTree::setType(const CEvaluationTree::Type & type)
{
  mType = type;
}

bool CEvaluationTree::setInfix(const std::string & infix)
{
  if (infix == mInfix &&
      infix != "") return true;

  // We assume until proven otherwise that the tree is not usable
  mUsable = false;

  mInfix = infix;

  return parse();
}

const std::string & CEvaluationTree::getInfix() const
{return mInfix;}

bool CEvaluationTree::operator == (const CEvaluationTree & rhs) const
{
  return (mInfix == rhs.mInfix && mType == rhs.mType);
}

std::string::size_type CEvaluationTree::getErrorPosition() const
{return mErrorPosition;}

const std::vector< CEvaluationNode * > & CEvaluationTree::getNodeList() const
{
  if (!mpNodeList)
    const_cast<CEvaluationTree *>(this)->mpNodeList = new std::vector< CEvaluationNode * >();

  return *mpNodeList;
}

size_t CEvaluationTree::getVariableIndex(const std::string & /*name*/) const
{return C_INVALID_INDEX;}

const C_FLOAT64 & CEvaluationTree::getVariableValue(const size_t & /*index*/) const
{
  static C_FLOAT64 Value = std::numeric_limits<C_FLOAT64>::quiet_NaN();
  return Value;
}

bool CEvaluationTree::parse()
{
  bool success = true;

  // clean up
  CEvaluationLexer::freeNodeList(mpNodeList);
  mpNodeList = NULL;
  mpRoot = NULL;

  if (mType == MassAction) return true;

  if (mInfix == "")
    {
      mpNodeList = new std::vector< CEvaluationNode * >;
      mpRoot = new CEvaluationNode();
      mpNodeList->push_back(mpRoot);
      return true;
    }

  // parse the description into a linked node tree
  std::istringstream buffer(mInfix);
  CEvaluationLexer Parser(&buffer);

  success = (Parser.yyparse() == 0);

  mpNodeList = Parser.getNodeList();
  mpRoot = Parser.getRootNode();

  // clean up if parsing failed
  if (!success)
    {
      mErrorPosition = Parser.getErrorPosition();

      CEvaluationLexer::freeNodeList(mpNodeList);
      mpNodeList = NULL;
      mpRoot = NULL;
    }

  if (success && hasCircularDependency())
    {
      success = false;
      CCopasiMessage(CCopasiMessage::ERROR, MCFunction + 4, mErrorPosition);
    }

  return success;
}

bool CEvaluationTree::compile()
{return compileNodes();}

bool CEvaluationTree::isUsable() const
{return mUsable;}

bool CEvaluationTree::isBoolean() const
{
  if (mpRoot != NULL)
    return mpRoot->isBoolean();

  return false;
}

void CEvaluationTree::buildCalculationSequence()
{
  CNodeIterator < CEvaluationNode > itNode(mpRoot);

  while (itNode.next() != itNode.end())
    {
      switch (CEvaluationNode::type(itNode->getType()))
        {
          case CEvaluationNode::NUMBER:
          case CEvaluationNode::CONSTANT:
          case CEvaluationNode::OBJECT:
            break;

          default:
            mCalculationSequence.push_back(*itNode);
            break;
        }
    }
}

bool CEvaluationTree::compileNodes()
{
  clearDirectDependencies();
  mCalculationSequence.clear();

  if (mInfix == "")
    return mUsable = true;

  if (mpNodeList == NULL)
    return mUsable = false;

  // The compile order must be child first.
  CNodeIterator< CEvaluationNode > itNode(mpRoot);
  mUsable = true;

  while (mUsable && itNode.next() != itNode.end())
    {
      if (*itNode != NULL)
        {
          mUsable &= itNode->compile(this);
        }
    }

  std::vector< CEvaluationNode * >::iterator it;
  std::vector< CEvaluationNode * >::iterator end = mpNodeList->end();

  if (!mUsable)
    {
      // Find the error node in the node list
      for (it = mpNodeList->begin(); it != end; ++it)
        {
          if (*it == *itNode)
            {
              end = it + 1;
              break;
            }
        }

      mErrorPosition = 0;

      for (it = mpNodeList->begin(); it != end; ++it)
        mErrorPosition += (*it)->getData().length();

      mErrorPosition -= (*--it)->getData().length();
      CCopasiMessage(CCopasiMessage::ERROR, MCFunction + 3, getObjectName().c_str(), mErrorPosition);
    }
  else
    {
      const CObjectInterface * pObject;

      for (it = mpNodeList->begin(); it != end; ++it)
        switch ((*it)->getType() & 0xFF000000)
          {
            case CEvaluationNode::OBJECT:
            {
              if (mType == Expression &&
                  (pObject = static_cast< CEvaluationNodeObject *>(*it)->getObjectInterfacePtr()) != NULL)
                {
                  const CCopasiObject * pDataObject = dynamic_cast< const CCopasiObject * >(pObject);

                  if (pDataObject == NULL)
                    {
                      const CMathObject * pMathObject = dynamic_cast< const CMathObject * >(pObject);

                      if (pMathObject != NULL)
                        {
                          pDataObject = pMathObject->getDataObject();
                        }
                    }

                  addDirectDependency(pDataObject);
                }
            }
            break;

            case CEvaluationNode::CALL:
              addDirectDependency(static_cast< CEvaluationNodeCall *>(*it)->getCalledTree());
              break;

            default:
              break;
          }

      buildCalculationSequence();
    }

  return mUsable;
}

void CEvaluationTree::calculate()
{
  try
    {
      if (mpRoot != NULL)
        {
          std::vector< CEvaluationNode * >::iterator it = mCalculationSequence.begin();
          std::vector< CEvaluationNode * >::iterator end = mCalculationSequence.end();

          for (; it != end; ++it)
            {
              (*it)->calculate();
            }

          mValue = mpRoot->getValue();
        }
      else
        {
          mValue = std::numeric_limits< C_FLOAT64 >::quiet_NaN();
        }
    }

  catch (...)
    {
      mValue = std::numeric_limits<C_FLOAT64>::quiet_NaN();
    }
}

bool CEvaluationTree::setRoot(CEvaluationNode* pRootNode)
{
  assert(pRootNode->getParent() == NULL);

  if (pRootNode == NULL) return false;

  if (mpNodeList != NULL)
    CEvaluationLexer::freeNodeList(mpNodeList);

  mpRoot = pRootNode;

  mpNodeList = new std::vector< CEvaluationNode * >();

  return this->updateTree();
}

bool CEvaluationTree::updateTree()
{
  if (mpRoot == NULL)
    {
      CEvaluationLexer::freeNodeList(mpNodeList);
      mpNodeList = NULL;

      return false;
    }

  if (mpNodeList == NULL)
    {
      mpNodeList = new std::vector< CEvaluationNode * >;
    }

  // Clear the list but preserve the tree
  mpNodeList->clear();

  CCopasiTree<CEvaluationNode>::iterator it = mpRoot;
  CCopasiTree<CEvaluationNode>::iterator end = NULL;

  for (; it != end; ++it)
    mpNodeList->push_back(&*it);

  mInfix = mpRoot->buildInfix();

  return true;
}

bool CEvaluationTree::setTree(const ASTNode& pRootNode)
{
  return this->setRoot(CEvaluationTree::fromAST(&pRootNode));
}

CEvaluationNode * CEvaluationTree::fromAST(const ASTNode * pASTNode)
{
  if (pASTNode == NULL) return NULL;

  CNodeContextIterator< const ASTNode, std::vector< CEvaluationNode * > > itNode(pASTNode);
  CEvaluationNode* pResultNode = NULL;

  while (itNode.next() != itNode.end())
    {
      if (*itNode != NULL)
        {
          CCopasiMessage Message;

          switch (itNode->getType())
            {
              case AST_LAMBDA:
                // this nodetype will never be handled directly
                break;

              case AST_PLUS:
              case AST_MINUS:
              case AST_TIMES:
              case AST_DIVIDE:
              case AST_POWER:
              case AST_FUNCTION_POWER:
                // create a CEvaluationNodeOperator
                pResultNode = CEvaluationNodeOperator::fromAST(*itNode, itNode.context());
                break;

              case AST_INTEGER:
              case AST_REAL:
              case AST_REAL_E:
              case AST_RATIONAL:
                // create a CEvaluationNodeNumber
                pResultNode = CEvaluationNodeNumber::fromAST(*itNode, itNode.context());
                break;

              case AST_NAME:
              case AST_NAME_TIME:
#if LIBSBML_VERSION >= 40100
              case AST_NAME_AVOGADRO:
#endif // LIBSBML_VERSION >= 40100
                // create a CEvaluationNodeObject
                pResultNode = CEvaluationNodeObject::fromAST(*itNode, itNode.context());
                break;

              case AST_CONSTANT_E:
              case AST_CONSTANT_PI:
              case AST_CONSTANT_FALSE:
              case AST_CONSTANT_TRUE:
                // create a CEvaluationNodeConstant
                pResultNode = CEvaluationNodeConstant::fromAST(*itNode, itNode.context());
                break;

              case AST_FUNCTION:
                // create a function call node
                pResultNode = CEvaluationNodeCall::fromAST(*itNode, itNode.context());
                break;

              case AST_FUNCTION_DELAY:
                // create a function call node
                pResultNode = CEvaluationNodeDelay::fromAST(*itNode, itNode.context());
                break;

              case AST_FUNCTION_ABS:
              case AST_FUNCTION_ARCCOS:
              case AST_FUNCTION_ARCCOSH:
              case AST_FUNCTION_ARCCOT:
              case AST_FUNCTION_ARCCOTH:
              case AST_FUNCTION_ARCCSC:
              case AST_FUNCTION_ARCCSCH:
              case AST_FUNCTION_ARCSEC:
              case AST_FUNCTION_ARCSECH:
              case AST_FUNCTION_ARCSIN:
              case AST_FUNCTION_ARCSINH:
              case AST_FUNCTION_ARCTAN:
              case AST_FUNCTION_ARCTANH:
              case AST_FUNCTION_CEILING:
              case AST_FUNCTION_COS:
              case AST_FUNCTION_COSH:
              case AST_FUNCTION_COT:
              case AST_FUNCTION_COTH:
              case AST_FUNCTION_CSC:
              case AST_FUNCTION_CSCH:
              case AST_FUNCTION_EXP:
              case AST_FUNCTION_FACTORIAL:
              case AST_FUNCTION_FLOOR:
              case AST_FUNCTION_LN:
              case AST_FUNCTION_LOG:
              case AST_FUNCTION_ROOT:
              case AST_FUNCTION_SEC:
              case AST_FUNCTION_SECH:
              case AST_FUNCTION_SIN:
              case AST_FUNCTION_SINH:
              case AST_FUNCTION_TAN:
              case AST_FUNCTION_TANH:
              case AST_LOGICAL_NOT:
                pResultNode = CEvaluationNodeFunction::fromAST(*itNode, itNode.context());
                break;

              case AST_LOGICAL_AND:
              case AST_LOGICAL_OR:
              case AST_LOGICAL_XOR:
              case AST_RELATIONAL_EQ:
              case AST_RELATIONAL_GEQ:
              case AST_RELATIONAL_GT:
              case AST_RELATIONAL_LEQ:
              case AST_RELATIONAL_LT:
              case AST_RELATIONAL_NEQ:
                pResultNode = CEvaluationNodeLogical::fromAST(*itNode, itNode.context());
                break;

              case AST_FUNCTION_PIECEWISE:
                pResultNode = CEvaluationNodeChoice::fromAST(*itNode, itNode.context());
                break;

              case AST_UNKNOWN:
                // create an unknown element error
                Message = CCopasiMessage(CCopasiMessage::EXCEPTION, MCMathML + 2);

                break;
            }

          if (itNode.parentContextPtr() != NULL)
            {
              itNode.parentContextPtr()->push_back(pResultNode);
            }
        }
    }

  return pResultNode;
}

const CEvaluationNode* CEvaluationTree::getRoot() const
{
  return this->mpRoot;
}

CEvaluationNode* CEvaluationTree::getRoot()
{
  return this->mpRoot;
}

void CEvaluationTree::initObjects()
{
  std::set< const CCopasiObject * > Self;
  Self.insert(this);

  CCopasiObject * pObject =
    addObjectReference("Value", mValue, CCopasiObject::ValueDbl);

  pObject->setDirectDependencies(Self);
}

CCopasiObject::DataObjectSet CEvaluationTree::getDeletedObjects() const
{
  CCopasiObject::DataObjectSet Deleted;

  Deleted.insert(this);
  Deleted.insert(static_cast< const CCopasiObject * >(getObject(CCopasiObjectName("Reference=Value"))));

  return Deleted;
}

ASTNode* CEvaluationTree::toAST(const CCopasiDataModel* pDataModel) const
{
  return this->mpRoot->toAST(pDataModel);
}

bool CEvaluationTree::hasCircularDependency() const
{
  std::set< std::string > List;

  return calls(List);
}

bool CEvaluationTree::dependsOnTree(const std::string & name) const
{
  if (!mpNodeList) return false;

  std::vector< CEvaluationNode * >::const_iterator it = mpNodeList->begin();
  std::vector< CEvaluationNode * >::const_iterator end = mpNodeList->end();

  for (; it != end; ++it)
    if (((*it)->getType() & 0xFF000000) == CEvaluationNode::CALL &&
        (*it)->getData() == name)
      return true;

  return false;
}

bool CEvaluationTree::calls(std::set< std::string > & list) const
{
  if (!mpNodeList) return false;

  std::pair<std::set< std::string >::iterator, bool> Result = list.insert(getObjectName());

  if (!Result.second) return true;

  bool Calls = false;
  std::vector< CEvaluationNode * >::iterator it;
  std::vector< CEvaluationNode * >::iterator end = mpNodeList->end();

  for (it = mpNodeList->begin(); it != end; ++it)
    if (((*it)->getType() & 0xFF000000) == CEvaluationNode::CALL &&
        dynamic_cast<CEvaluationNodeCall *>(*it)->calls(list))
      {
        Calls = true;
        break;
      }

  list.erase(Result.first);

  return Calls;
}

bool CEvaluationTree::hasDiscontinuity() const
{
  if (mpNodeList == NULL)
    return false;

  std::vector< CEvaluationNode * >::iterator it = mpNodeList->begin();
  std::vector< CEvaluationNode * >::iterator end = mpNodeList->end();

  for (; it != end; ++it)
    {
      switch ((int)(*it)->getType())
        {
          case (CEvaluationNode::CHOICE | CEvaluationNodeChoice::IF):
          case (CEvaluationNode::FUNCTION | CEvaluationNodeFunction::FLOOR):
          case (CEvaluationNode::FUNCTION | CEvaluationNodeFunction::CEIL):
          case (CEvaluationNode::OPERATOR | CEvaluationNodeOperator::MODULUS):
            // We found a discontinuity.
            return true;
            break;

          case (CEvaluationNode::CALL | CEvaluationNodeCall::FUNCTION):
          case (CEvaluationNode::CALL | CEvaluationNodeCall::EXPRESSION):

            // If the called tree has a discontinuity so do we.
            if (static_cast< CEvaluationNodeCall * >(*it)->getCalledTree()->hasDiscontinuity())
              {
                return true;
              }

            break;
        }
    }

  return false;
}
