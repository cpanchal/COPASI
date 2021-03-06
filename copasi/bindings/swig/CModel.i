// Copyright (C) 2010 - 2013 by Pedro Mendes, Virginia Tech Intellectual 
// Properties, Inc., University of Heidelberg, and The University 
// of Manchester. 
// All rights reserved. 

// Copyright (C) 2008 - 2009 by Pedro Mendes, Virginia Tech Intellectual 
// Properties, Inc., EML Research, gGmbH, University of Heidelberg, 
// and The University of Manchester. 
// All rights reserved. 

// Copyright (C) 2006 - 2007 by Pedro Mendes, Virginia Tech Intellectual 
// Properties, Inc. and EML Research, gGmbH. 
// All rights reserved. 















%{

#include "model/CModel.h"

%}

%template(ObjectStdVector) std::vector<CCopasiObject*>;
typedef std::vector<CCopasiObject*> ObjectStdVector;

%ignore CModel::load;
%ignore CModel::compileIfNecessary(CProcessReport* pProcessReport);
%ignore CModel::forceCompile(CProcessReport* pProcessReport);
%ignore CModel::VolumeUnitNames;
%ignore CModel::AreaUnitNames;
%ignore CModel::LengthUnitNames;
%ignore CModel::TimeUnitNames;
%ignore CModel::QuantityUnitOldXMLNames;
%ignore CModel::QuantityUnitNames;
%ignore CModel::ModelTypeNames;
%ignore CModel::appendDependentModelObjects;
%ignore CModel::appendDependentReactions;
%ignore CModel::appendDependentMetabolites;
%ignore CModel::appendDependentCompartments;
%ignore CModel::appendDependentModelValues;
%ignore CModel::appendDependentEvents;
%ignore CModel::removeDependentModelObjects;
%ignore CModel::getUptoDateObjects;

// according to Stefan, the method to calculate the elasticities is no longer used
// and might actually not work at all
%ignore CModel::calculateElasticityMatrix(const C_FLOAT64&,const C_FLOAT64&);
%ignore CModel::getModelParameterSet() const;
%ignore CModel::getMetabolites() const;
%ignore CModel::getMetabolitesX() const;
%ignore CModel::getModelValues() const;
%ignore CModel::getReactions() const;
%ignore CModel::getEvents() const;
%ignore CModel::getCompartments() const;
%ignore CModel::getStateTemplate() const;
%ignore CModel::processQueue;
%ignore CModel::processRoots;
%ignore CModel::calculateDerivatives;
%ignore CModel::calculateDerivativesX;
%ignore CModel::getListOfInitialRefreshes;
%ignore CModel::getListOfSimulatedRefreshes;
%ignore CModel::getListOfNonSimulatedRefreshes;
%ignore CModel::getListOfConstantRefreshes;
%ignore CModel::buildInitialRefreshSequence;
%ignore CModel::getRootFinders;
%ignore CModel::getL;
%ignore CModel::getModelParameterSets() const;

// suppress warnings on nested structures
%warnfilter(325) CLinkMatrixView;

#ifdef SWIGR
// since enums in R are mapped to strings, it seems to confuse swig
// if a string is passed to one of the setXXXUnit methods in CModel
// because it doesn't know whether to pass it on as a string or to translate it
// to the enum
%rename (setTimeUnitFromString) CModel::setTimeUnit(const std::string & name);
%rename (setVolumeUnitFromString) CModel::setVolumeUnit(const std::string & name);
%rename (setAreaUnitFromString) CModel::setAreaUnit(const std::string & name);
%rename (setLengthUnitFromString) CModel::setLengthUnit(const std::string & name);
%rename (setQuantityUnitFromString) CModel::setQuantityUnit(const std::string & name);
#endif // SWIGR

%include "model/CModel.h"

%catches(CCopasiException) CModel::compile();
%catches(CCopasiException) CModel::compileEvents();
%catches(CCopasiException) CModel::compileIfNecessary;
%catches(CCopasiException) CModel::convert2NonReversible;
%catches(CCopasiException) CModel::initializeMetabolites;

%extend CModel
{
    /**
     *  Get the number of compartments 
     *  @return C_INT32 getCompartments().size()
     */
    unsigned C_INT32 getNumCompartments() const
    {
      return self->getCompartments().size();
    }

    /**
     *  Get the number of reactions
     *  @return C_INT32 getReactions().size()
     */
    unsigned C_INT32 getNumReactions() const
    {
      return self->getReactions().size();
    }

    CReaction* getReaction(unsigned C_INT32 index)
    {
	   try
	   {
        return self->getReactions()[index];
	   }
	   catch (...)
	   {
		return NULL;
	   }
    }
	
	 CReaction* getReaction(const std::string& name)
    {
	   try
	   {
        return self->getReactions()[name];
	   }
	   catch (...)
	   {
		return NULL;
	   }
    }

    CCompartment* getCompartment(unsigned C_INT32 index)
    {
	   try
	   {
        return self->getCompartments()[index];
		}
	   catch (...)
	   {
		return NULL;
	   }
    }

	CCompartment* getCompartment(const std::string& name)
    {
	   try
	   {
        return self->getCompartments()[name];
		}
	   catch (...)
	   {
		return NULL;
	   }
    }

	
    CMetab* getMetabolite(unsigned C_INT32 index)
    {	
		try
		{
        return self->getMetabolites()[index];
		}
	   catch (...)
	   {
		return NULL;
	   }
    }
	
	CMetab* getMetabolite(const std::string& name)
    {	
		try
		{
        return self->getMetabolites()[$self->findMetabByName(name)];
		}
	   catch (...)
	   {
		return NULL;
	   }
    }

    CModelValue* getModelValue(unsigned C_INT32 index)
    {
		try
		{
        return self->getModelValues()[index];
		}
	   catch (...)
	   {
		return NULL;
	   }
    }

    CMoiety* getMoiety(unsigned C_INT32 index)
    {
		try
		{
        return self->getMoieties()[index];
		}
	   catch (...)
	   {
		return NULL;
	   }
    }

    bool forceCompile()
    {
        return $self->forceCompile(NULL);
    };

    bool compileIfNecessary()
    {
        return $self->compileIfNecessary(NULL);
    };

    void applyInitialValues()
    {
        $self->compileIfNecessary(NULL);
        $self->applyInitialValues();
        $self->updateNonSimulatedValues();
    }

    void updateInitialValues(const std::vector<CCopasiObject*>& v)
    {
        std::set<const CCopasiObject*> changedObjects;
        changedObjects.insert(v.begin(),v.end());
        std::vector<Refresh*> refreshes=$self->buildInitialRefreshSequence(changedObjects);
        std::vector<Refresh*>::iterator refreshIt = refreshes.begin(), refreshEndit = refreshes.end();
        while (refreshIt != refreshEndit)
            (**refreshIt++)();
    };

    CModelValue* getModelValue(const std::string& name)
    {
		try
		{
        return $self->getModelValues()[name];
		}
		catch(...)
		{
		return NULL;
		}
    }
	
	CEvent* getEvent(const std::string& name)
    {
		try
		{
        return $self->getEvents()[name];
		}
		catch(...)
		{
		return NULL;
		}
    }

    // for backwards compatibility
   unsigned C_INT32 getNumIndependentMetabs() const 
   {
       std::cerr << "Calling getNumIndependentMetabs on CModel instances is obsolete, please use getNumIndependentReactionMetabs instead." << std::endl;
        return $self->getNumIndependentReactionMetabs();
   }

    // for backwards compatibility
   unsigned C_INT32 getNumDependentMetabs() const 
   {
       std::cerr << "Calling getNumDependentMetabs on CModel instances is obsolete, please use getNumDependentReactionMetabs instead." << std::endl;
        return $self->getNumDependentReactionMetabs();
   }

   // for backward compatibility
   void setComments(const std::string& notes)
   {
      std::cerr << "Calling setComments on CModel instances is obsolete, please use setNotes instead." << std::endl;
      self->setNotes(notes);
   }

   // for backward compatibility
   const std::string& getComments() const
   {
     std::cerr << "Calling getComments on CModel instances is obsolete, please use getNotes instead." << std::endl;
     return self->getNotes();
   }

   // more convenience methods
   unsigned C_INT32 getNumEvents() const
   {
        return $self->getEvents().size();
   }

   CEvent* getEvent(unsigned C_INT32 index)
   {
		try
		{
        return $self->getEvents()[index];
		}
		catch(...)
		{
		return NULL;
		}
		
   }
}



