/**
 * CFunctionParameter
 * 
 * Created for Copasi by Stefan Hoops
 * (C) Stefan Hoops 2001
 */

#define COPASI_TRACE_CONSTRUCTION
#include "copasi.h"
#include "CFunctionParameter.h"
#include "report/CKeyFactory.h"

const std::string CFunctionParameter::DataTypeName[] =
  {"Integer", "Double", "Vector of Integer", "Vector of Double", ""};

CFunctionParameter::CFunctionParameter(const std::string & name,
                                       const CCopasiContainer * pParent):
    CCopasiContainer(name, pParent, "Variable"),
    mKey(CKeyFactory::add("FunctionParameter", this)),
    mName(mObjectName),
    mType((CFunctionParameter::DataType) - 1),
    mUsage("unknown")
{CONSTRUCTOR_TRACE;}

CFunctionParameter::CFunctionParameter(const CFunctionParameter & src,
                                       const CCopasiContainer * pParent):
    CCopasiContainer(src, pParent),
    mKey(CKeyFactory::add("FunctionParameter", this)),
    mName(mObjectName),
    mType(src.mType),
    mUsage(src.mUsage)
{CONSTRUCTOR_TRACE;}

CFunctionParameter::CFunctionParameter(const std::string &name,
                                       const enum CFunctionParameter::DataType &type,
                                       const std::string &usage,
                                       const CCopasiContainer * pParent):
    CCopasiContainer(name, pParent, "Variable"),
    mKey(CKeyFactory::add("FunctionParameter", this)),
    mName(mObjectName),
    mType(type),
    mUsage(usage)
{CONSTRUCTOR_TRACE;}

CFunctionParameter::~CFunctionParameter()
{
  CKeyFactory::remove(mKey);
  DESTRUCTOR_TRACE;
}

void CFunctionParameter::cleanup() {}

void CFunctionParameter::load(CReadConfig & configbuffer,
                              CReadConfig::Mode mode)
{
  configbuffer.getVariable("FunctionParameter", "string", &mName, mode);
  configbuffer.getVariable("DataType", "C_INT32", &mType);
  configbuffer.getVariable("Usage", "string", &mUsage);
}

void CFunctionParameter::save(CWriteConfig & configbuffer)
{
  configbuffer.setVariable("FunctionParameter", "string", &mName);
  configbuffer.setVariable("DataType", "C_INT32", &mType);
  configbuffer.setVariable("Usage", "string", &mUsage);
}

void CFunctionParameter::setName(const std::string & name) {mName = name;}

std::string CFunctionParameter::getKey() const {return mKey;}

const std::string & CFunctionParameter::getName() const {return mName;}

void CFunctionParameter::setUsage(const std::string & usage) {mUsage = usage;}

const std::string & CFunctionParameter::getUsage() const {return mUsage;}

void CFunctionParameter::setType(const CFunctionParameter::DataType & type)
{mType = type;}

const CFunctionParameter::DataType &

CFunctionParameter::getType() const
  {
    return mType;
  }
