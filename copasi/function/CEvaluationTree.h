/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/function/CEvaluationTree.h,v $
   $Revision: 1.20 $
   $Name:  $
   $Author: shoops $ 
   $Date: 2005/11/30 22:01:08 $
   End CVS Header */

#ifndef COPASI_CEvaluationTree
#define COPASI_CEvaluationTree

#include <vector>

#include "CEvaluationNode.h"
#include "report/CCopasiContainer.h"

class ASTNode;
template <class CType> class CCopasiVectorN;

class CEvaluationTree:
      public CCopasiContainer
  {
  public:
    /**
     *  The valid types of a function
     */
    enum Type
    {
      Function = 0,
      MassAction,
      PreDefined,
      UserDefined,
      Expression
    };

    /**
     *  The string representation of valid types of a function
     */
    static const std::string TypeName[];

    /**
     *  The string representation of valid types of a function
     */
    static const char * XMLType[];

    /**
     * The id of the corresponding function in an SBML file.
     * This value is either set upon importing an SBML file,
     * or when the object is first exported to an SBML file.
     */
    std::string mSBMLId;

    // Operations
  public:
    /**
     * Create an EvaluationTree of the specified type.
     * Note: the returned object has to be released after use with delete.
     * @param CEvaluationTree::Type type 
     * @return EvaluationTree * pEvaluationTree
     */
    static CEvaluationTree *
    create(CEvaluationTree::Type type);

    /**
     * Copy an EvaluationTree of the specified type.
     * Note: the returned object has to be released after use with delete.
     * @param const EvaluationTree & src
     * @return EvaluationTree * pEvaluationTree
     */
    static CEvaluationTree *
    copy(const CEvaluationTree & src);

    /**
     * Default constructor
     * @param const std::string & name (default: "NoName")
     * @param const CCopasiContainer * pParent (default: NULL)
     * @param const CEvaluationTree::Type & type (default: Function)
     */
    CEvaluationTree(const std::string & name = "NoName",
                    const CCopasiContainer * pParent = NULL,
                    const Type & type = Function);

    /**
     * Copy constructor
     * @param const CEvaluationTree & src
     * @param const CCopasiContainer * pParent (default: NULL)
     */
    CEvaluationTree(const CEvaluationTree & src,
                    const CCopasiContainer * pParent = NULL);

    /**
     * Destructor
     */
    virtual ~CEvaluationTree();

    /**
     * Retrieves the type of the function
     * @return const CEvaluationTree::Type & type
     */
    const CEvaluationTree::Type & getType() const;

    /**
     * Set the type of the function
     * @param const CEvaluationTree::Type & type
     */
    void setType(const CEvaluationTree::Type & type);

    /**
     * Retrieves the key of the EvaluationTree
     * @return const std::string & key
     */
    const std::string & getKey() const;

    /**
     * Sets the root node of the tree.
     * @param CEvaluationNode* root node of the tree
     * @return bool success
     */
    bool setRoot(CEvaluationNode* pRootNode);

    /**
     * Sets the complete tree from an SBML ASTNode.
     * @param const ASTNode& root node of the tree
     * @return bool success
     */
    bool setTree(const ASTNode& pRootNode);

    /**
     * Converts an SBML ASTNode to the corresponding
     * CEvaluationNode tree. 
     * @param const ASTNode& node
     * @return CEvaluationNode*
     */
    static CEvaluationNode* convertASTNode(const ASTNode& node);

    /**
     * Converts a CEvaluationTree to an ASTNode based tree.
     * @return ASTNode* root node of the tree.
     */
    virtual ASTNode* toAST() const;

    /**
     * Set the infix description of the tree and compile it.
     * @param const std::string & infix
     * @return bool success
     */
    virtual bool setInfix(const std::string & infix);

    /**
     * Retrieve the infix description of the tree
     * @return const std::string & infix
     */
    const std::string & getInfix() const;

    /**
     * Retrieve the position of the error in the string description.
     * std::string::npos indicates no error.
     * @return std::string::size_type errorPosition
     */
    std::string::size_type getErrorPosition() const;

    /**
     * Retrieve the list of nodes
     * @return const std::vector< CEvaluationNode * > & nodeList;
     */
    const std::vector< CEvaluationNode * > & getNodeList() const;

    /**
     * Retrieve the index to the value of the named variable.
     * @param const std::string & name
     * @return unsigned C_INT32
     */
    virtual unsigned C_INT32 getVariableIndex(const std::string & name) const;

    /**
     * Retrieve the value of the indexed variable
     * @param const unsigned C_INT32 & index
     * @return const C_FLOAT64 & variableValue
     */
    virtual const C_FLOAT64 & getVariableValue(const unsigned C_INT32 & index) const;

    /**
     * Retrieve a pointer to the object define dby CN.
     * @param const CCopasiObjectName & CN
     * @return const CCopasiObject * value
     */
    virtual const CCopasiObject * getObject(const CCopasiObjectName & CN) const;

    /**
     * Returns the root node of the tree.
     */
    CEvaluationNode* getRoot();

    /**
     * Returns the root node of the tree.
     */
    const CEvaluationNode* getRoot() const;

    /**
     * Updates the infix and the nodeList
     */
    bool updateTree();

    /**
     * Sets the SBMLId.
     */
    void setSBMLId(const std::string& id);

    /**
     * Compile the individual nodes.
     * @return bool success
     */
    bool compileNodes();

    /**
     * Returns a reference to the SBML Id.
     */
    const std::string& getSBMLId() const;

    /**
     * Complete the list of evaluation tree such that all called trees are includes.
     * @param CCopasiVectorN< CEvaluationTree > & list
     * @param const unsigned C_INT32 & added (Default: list.size())
     * @return bool success
     */
    static bool completeEvaluationTreeList(CCopasiVectorN< CEvaluationTree > & list,
                                           const unsigned C_INT32 & added = 0);

    /**
     * Check whether the the tree has circular dependencies.
     * @return bool hasCircularDependency
     */
    bool hasCircularDependency() const;

    /**
     * Check whether the the calls any tree in the list
     * @param std::set< std::string > & list
     * @return bool calls
     */
    bool calls(std::set< std::string > & list) const;

  protected:
    /**
     * Parse the description
     * @return bool success
     */
    bool parse();

  private:
    /**
     * Initialize the contained CCopasiObjects
     */
    void initObjects();

    // Attributes
  private:
    /**
     * The type of the function
     */
    CEvaluationTree::Type mType;

    /**
     * The key of the function
     */
    std::string mKey;

  protected:
    /**
     * The infix representation of the expression
     */
    std::string mInfix;

  private:
    /**
     * The error position in case the compilation fails.
     * std::string::npos indicates no error.
     */
    std::string::size_type mErrorPosition;

  protected:
    /**
     * A vector of nodes containing the result of parse.
     */
    std::vector< CEvaluationNode * > * mpNodeList;

    /**
     * The root node of the tree.
     */
    CEvaluationNode * mpRoot;

    /**
     * The last computed value of the tree;
     */
    C_FLOAT64 mValue;
  };

#endif // COPASI_CEvaluationTree
