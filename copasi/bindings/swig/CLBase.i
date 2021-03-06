// Copyright (C) 2010 - 2013 by Pedro Mendes, Virginia Tech Intellectual 
// Properties, Inc., University of Heidelberg, and The University 
// of Manchester. 
// All rights reserved. 


%{

#include "layout/CLBase.h"

%}

%ignore CLBase::CLBase(const SBase&);


%rename(add)             *::operator+;
%rename(sub)             *::operator-;
%rename(mul)             *::operator*;


%ignore CLPoint::getX;
%ignore CLPoint::getY;
%ignore CLPoint::getZ;
%ignore CLPoint::CLPoint(const Point&);
%ignore CLPoint::getSBMLPoint() const;
%ignore CLPoint::operator<(const CLPoint&) const;

%ignore CLDimensions::getHeight;
%ignore CLDimensions::getWidth;
%ignore CLDimensions::getDepth;
%ignore CLDimensions::CLDimensions(const Dimensions&);
%ignore CLDimensions::getSBMLDimensions() const;

%ignore CLBoundingBox::getPosition() const;
%ignore CLBoundingBox::getDimensions() const;
%ignore CLBoundingBox::CLBoundingBox(const BoundingBox&);
%ignore CLBoundingBox::getSBMLBoundingBox() const;


%include "layout/CLBase.h"

// unignore getX and getY for CLPoint
%rename(getX) CLPoint::getX;
%rename(getY) CLPoint::getY;

// unignore getHeight and getWidth for CLDimensions
%rename(getHeight) CLDimensions::getHeight;
%rename(getWidth) CLDimensions::getWidth;



%extend CLPoint
{
    double getX() const
    {
        double v=$self->getX();
        return v;
    }

    double getY() const
    {
        double v=$self->getY();
        return v;
    }
}

%extend CLDimensions
{
    double getHeight() const
    {
        double v=$self->getHeight();
        return v;
    }

    double getWidth() const
    {
        double v=$self->getWidth();
        return v;
    }
}
