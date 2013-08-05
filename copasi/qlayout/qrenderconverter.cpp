
#include <qgraphicsitem.h>
#include <qpen.h>
#include <qfont.h>
#include <QFontMetrics>
#include <QPixmap>
#include <QFile>
#include <QGraphicsScene>

#include <qlayout/qrenderconverter.h>
#include <qlayout/qroundedrect.h>
#include <layout/CLStyle.h>
#include <layout/CLEllipse.h>
#include <layout/CLRectangle.h>
#include <layout/CLImage.h>
#include <layout/CLPolygon.h>
#include <layout/CLText.h>
#include <layout/CLCurve.h>
#include <layout/CLRenderCurve.h>
#include <layout/CLRenderCubicBezier.h>
#include <layout/CLRenderPoint.h>
#include <layout/CLLinearGradient.h>
#include <layout/CLRadialGradient.h>
#include <layout/CLRenderResolver.h>
#include <layout/CLGraphicalPrimitive1D.h>
#include <report/CCopasiRootContainer.h>
#include <CopasiDataModel/CCopasiDataModel.h>

#include <math.h>

void transform(QGraphicsItem *item, const CLTransformation2D* trans, const CLGroup* group)
{
  if (trans != NULL && trans->isSetMatrix())
  {
    QRectF pathRect = item->boundingRect();
    QTransform transform(
      trans->getMatrix2D()[0], 
      trans->getMatrix2D()[1], 
      trans->getMatrix2D()[2], 
      trans->getMatrix2D()[3], 
      trans->getMatrix2D()[4], 
      trans->getMatrix2D()[5]
    );
    
    QTransform matrix;
    matrix = matrix.translate(pathRect.x(), pathRect.y());
    matrix = matrix.inverted();
    matrix = matrix * transform;

    QTransform translate; 
    translate = translate.translate(pathRect.x(), pathRect.y());
    matrix = matrix * translate;
    item->setTransform(matrix, true);

  }
  else if (group != NULL && group->isSetMatrix())
  {
    QRectF pathRect = item->boundingRect();
    QTransform transform(
      group->getMatrix2D()[0], 
      group->getMatrix2D()[1], 
      group->getMatrix2D()[2], 
      group->getMatrix2D()[3], 
      group->getMatrix2D()[4], 
      group->getMatrix2D()[5]
    );
    
    QTransform matrix;
    matrix = matrix.translate(pathRect.x(), pathRect.y());
    matrix = matrix.inverted();
    matrix = matrix * transform;

    QTransform translate; 
    translate = translate.translate(pathRect.x(), pathRect.y());
    matrix = matrix * translate;
    item->setTransform(matrix, true);

  }
}

QColor getColor(const CLColorDefinition* cd)
{
  if (cd == NULL) return QColor();
  return QColor::fromRgb(cd->getRed(), cd->getGreen(), cd->getBlue(), cd->getAlpha());
}

QColor getColor(const std::string& color, const CLRenderResolver* resolver)
{
  if (color == "none")
    return QColor(Qt::transparent);
  if (resolver->getColorDefinition(color)==NULL && resolver->getGradientBase(color) == NULL)
  {
    CLColorDefinition cd;cd.setColorValue(color);
    return getColor(&cd);
  }
  return getColor(resolver->getColorDefinition(color));
}

QLinearGradient* getLinearGradient(const CLLinearGradient* linear, const CLBoundingBox* bounds, const CLRenderResolver* resolver)
{
  double x1 = bounds->getPosition().getX() + linear->getXPoint1().getAbsoluteValue()  + (linear->getXPoint1().getRelativeValue() / 100.0 * bounds->getDimensions().getWidth());
  double y1 = bounds->getPosition().getY() +linear->getYPoint1().getAbsoluteValue()  + (linear->getYPoint1().getRelativeValue() / 100.0 * bounds->getDimensions().getHeight());
  double x2 = bounds->getPosition().getX() + linear->getXPoint2().getAbsoluteValue()  + (linear->getXPoint2().getRelativeValue() / 100.0 * bounds->getDimensions().getWidth());
  double y2 = bounds->getPosition().getY() + linear->getYPoint2().getAbsoluteValue()  + (linear->getYPoint2().getRelativeValue() / 100.0 * bounds->getDimensions().getHeight());

  QLinearGradient* result = new QLinearGradient(x1, y1, x2, y2);
  switch (linear->getSpreadMethod())
  {
  case CLGradientBase::REFLECT:
    result->setSpread(QGradient::ReflectSpread);
    break;
  case CLGradientBase::REPEAT:
    result->setSpread(QGradient::RepeatSpread);
    break;
  case CLGradientBase::PAD:
    result->setSpread(QGradient::PadSpread);
    break;
  default:
    break;
  }

  for (size_t i = 0; i < linear->getNumGradientStops(); ++i)
  {
    const CLGradientStop* stop = linear->getGradientStop(i);
    result->setColorAt(stop->getOffset().getAbsoluteValue()+ stop->getOffset().getRelativeValue() / 100.0, getColor(stop->getStopColor(), resolver));
  }
  return result;

}

QRadialGradient* getRadialGradient(const CLRadialGradient* radial, const CLBoundingBox* bounds, const CLRenderResolver* resolver)
{
  double cx = bounds->getPosition().getX() + radial->getCenterX().getAbsoluteValue()  + radial->getCenterX().getRelativeValue() / 100.0 * bounds->getDimensions().getWidth();
  double cy = bounds->getPosition().getY() + radial->getCenterY().getAbsoluteValue()  + radial->getCenterY().getRelativeValue() / 100.0 * bounds->getDimensions().getHeight();
  double fx = bounds->getPosition().getX() + radial->getFocalPointX().getAbsoluteValue()  + radial->getFocalPointX().getRelativeValue() / 100.0 * bounds->getDimensions().getWidth();
  double fy = bounds->getPosition().getY() + radial->getFocalPointY().getAbsoluteValue()  + radial->getFocalPointY().getRelativeValue() / 100.0 * bounds->getDimensions().getHeight();
  double r = radial->getRadius().getAbsoluteValue()  + radial->getRadius().getRelativeValue() / 100.0 * bounds->getDimensions().getWidth();

  QRadialGradient* result = new QRadialGradient(cx, cy, r, fx, fy);
  switch (radial->getSpreadMethod())
  {
  case CLGradientBase::REFLECT:
    result->setSpread(QGradient::ReflectSpread);
    break;
  case CLGradientBase::REPEAT:
    result->setSpread(QGradient::RepeatSpread);
    break;
  case CLGradientBase::PAD:
    result->setSpread(QGradient::PadSpread);
    break;
  default:
    break;
  }

  for (size_t i = 0; i < radial->getNumGradientStops(); ++i)
  {
    const CLGradientStop* stop = radial->getGradientStop(i);
    result->setColorAt(stop->getOffset().getAbsoluteValue() + stop->getOffset().getRelativeValue() / 100.0, getColor(stop->getStopColor(), resolver));
  }
  return result;
}


QGradient* getGradient(const CLGradientBase* base, const CLBoundingBox* bounds, const CLRenderResolver* resolver)
{
  const CLLinearGradient* linear = dynamic_cast<const CLLinearGradient*>(base);
  if (linear != NULL)
    return getLinearGradient(linear, bounds, resolver);
  const CLRadialGradient* radial = dynamic_cast<const CLRadialGradient*>(base);
  if (radial != NULL)
    return getRadialGradient(radial, bounds, resolver);
  return NULL;

}

QFont* getFont(const CLText *item,const CLGroup *group, const CLRenderResolver* resolver, const CLBoundingBox *pBB)
{
  QString font("Verdana"); double fontSize=10, weight=50; bool italic=false;
  if (item != NULL && item->isSetFontFamily())
    font = item->getFontFamily().c_str();
  else if (group != NULL && group->isSetFontFamily())
    font = group->getFontFamily().c_str();  

  if (item != NULL && item->isSetFontSize())
    fontSize = item->getFontSize().getAbsoluteValue() + item->getFontSize().getRelativeValue()/100.0 * pBB->getDimensions().getHeight();
  else if (group != NULL && group->isSetFontSize())
    fontSize = group->getFontSize().getAbsoluteValue() + group->getFontSize().getRelativeValue()/100.0 * pBB->getDimensions().getHeight();

  if (item != NULL && item->isSetFontWeight())
    switch(item->getFontWeight())
  {
    case CLText::WEIGHT_BOLD:
      weight = QFont::Bold;
      break;
    case CLText::WEIGHT_NORMAL:
    default:
      weight = QFont::Normal;
      break;
  }
  else if (group != NULL && group->isSetFontWeight())
    switch(group->getFontWeight())
  {
    case CLText::WEIGHT_BOLD:
      weight = QFont::Bold;
      break;
    case CLText::WEIGHT_NORMAL:
    default:
      weight = QFont::Normal;
      break;
  }

  if (item != NULL && item->isSetFontStyle())
    switch (item->getFontStyle())
  {
    case CLText::STYLE_ITALIC:
      italic = true;
      break;

  }
  else if (group != NULL && group->isSetFontStyle())
    switch (group->getFontStyle())
  {
    case CLText::STYLE_ITALIC:
      italic = true;
      break;

  }

  QFont *result = new QFont(font, -1, weight, italic);
  result->setPixelSizeFloat(fontSize);

  if (font == "serif")
  {
    result->setStyleHint(QFont::Serif);
  }
  else if (font == "sans-serif" || font == "sans")
  {
    result->setStyleHint(QFont::SansSerif);
  }
  else if (font == "monospace")
  {
    result->setStyleHint(QFont::Monospace);
  }

  return result;
}

QBrush* getBrush(const CLGraphicalPrimitive2D *item,const CLGroup *group, const CLRenderResolver* resolver, const CLBoundingBox *pBB)
{
  QColor color;
  if (item != NULL && item->isSetFill())
  {
    const CLGradientBase* base = resolver->getGradientBase(item->getFillColor());
    if (base != NULL)
    {
      return new QBrush(*getGradient(base, pBB, resolver));
    }
    return new QBrush(getColor(item->getFillColor(), resolver));
  } 
  else if (group != NULL && group->isSetFill())
  {
    const CLGradientBase* base = resolver->getGradientBase(group->getFillColor());
    if (base != NULL)
    {
      return new QBrush(*getGradient(base, pBB, resolver));
    }
    return new QBrush(getColor(group->getFillColor(), resolver));
  }
  return new QBrush();
}

QPen* getPen(const CLGraphicalPrimitive1D *item,const CLGroup *group, const CLRenderResolver* resolver, const CLBoundingBox *pBB)
{
  QColor color; double width;
  if (item != NULL && item->isSetStroke())
  {
    color = getColor(item->getStroke(), resolver);
  } 
  else if (group != NULL && group->isSetStroke())
  {
    color = getColor(group->getStroke(), resolver);
  }
  else return new QPen(Qt::transparent);

  if (item != NULL && item->isSetStrokeWidth())
  {
    width = item->getStrokeWidth();
  }
  else if (group != NULL && group->isSetStrokeWidth())
  {
    width = group->getStrokeWidth();
  }
  else return new QPen(Qt::transparent);


  QPen *result = new QPen(color, width);
  result->setCapStyle(Qt::RoundCap);
  result->setJoinStyle(Qt::RoundJoin);

  if (item != NULL && item->isSetDashArray())
  {
    const std::vector<unsigned int>& raw = item->getDashArray();
    auto start = raw.begin();    
    QVector<qreal> pattern; 
    while (start != raw.end())
    {
      pattern << *start;
      ++start;
    }
    result->setDashPattern(pattern);
  }
  else if (group != NULL && group->isSetDashArray())
  {
    const std::vector<unsigned int>& raw = group->getDashArray();
    auto start = raw.begin();    
    QVector<qreal> pattern; 
    while (start != raw.end())
    {
      pattern << *start;
      ++start;
    }
    result->setDashPattern(pattern);
  }

  return result;


}

void fillItemFromEllipse(QGraphicsItemGroup *item, const CLBoundingBox *pBB,const CLEllipse *pEllipse, const CLGroup *group, const CLRenderResolver* resolver)
{
  double x = pBB->getPosition().getX() + pEllipse->getCX().getAbsoluteValue() + pEllipse->getCX().getRelativeValue() / 100.0 * pBB->getDimensions().getWidth();
  double y = pBB->getPosition().getY() + pEllipse->getCY().getAbsoluteValue() + pEllipse->getCY().getRelativeValue() / 100.0 * pBB->getDimensions().getHeight();
  double z = pBB->getPosition().getZ() + pEllipse->getCZ().getAbsoluteValue() + pEllipse->getCZ().getRelativeValue() / 100.0 * pBB->getDimensions().getDepth();
  double rx = pEllipse->getRX().getAbsoluteValue() + pEllipse->getRX().getRelativeValue() / 100.0 * pBB->getDimensions().getWidth();
  double ry = pEllipse->getRY().getAbsoluteValue() + pEllipse->getRY().getRelativeValue() / 100.0 * pBB->getDimensions().getHeight();

  QGraphicsEllipseItem* ellipseItem = new QGraphicsEllipseItem(
    x-rx, y-ry, rx*2, ry*2 );
  QPen *pen = getPen(pEllipse, group, resolver, pBB);
  ellipseItem->setPen(*pen);
  delete pen;
  QBrush *brush = getBrush(pEllipse, group, resolver, pBB);
  ellipseItem->setBrush(*brush);
  delete brush;
  transform(ellipseItem, pEllipse, group);
  item->addToGroup(ellipseItem);
}

void fillItemFromCurve(QGraphicsItemGroup *item, const CLBoundingBox *pBB,const CLCurve* pCurve, const CLGroup *group, const CLRenderResolver* resolver)
{
  QPainterPath path; 

  if (!pCurve->isContinuous())
  {
    for (size_t i = 0; i < pCurve->getNumCurveSegments(); ++i)
    {
      const CLLineSegment* current = pCurve->getSegmentAt(i);
      path.moveTo(current->getStart().getX(), current->getStart().getY());
      if (current->isBezier())
      {
        path.cubicTo(
          current->getBase1().getX(), current->getBase1().getY(), 
          current->getBase2().getX(), current->getBase2().getY(), 
          current->getEnd().getX(), current->getEnd().getY());
      }
      else
      {
        path.lineTo(          
          current->getEnd().getX(), current->getEnd().getY());
      }

    }
  }
  else if (pCurve->getListOfPoints().size() > 0)
  {
    for (size_t i = 0; i < pCurve->getNumCurveSegments(); ++i)
    {
      const CLLineSegment* current = pCurve->getSegmentAt(i);
      if (i==0)
        path.moveTo(current->getStart().getX(), current->getStart().getY());
      if (current->isBezier())
      {
        path.cubicTo(
          current->getBase1().getX(), current->getBase1().getY(), 
          current->getBase2().getX(), current->getBase2().getY(), 
          current->getEnd().getX(), current->getEnd().getY());
      }
      else
      {
        path.lineTo(          
          current->getEnd().getX(), current->getEnd().getY());
      }

    }
  }
  else return;


  QGraphicsPathItem *pathItem = new QGraphicsPathItem(path);
  QPen *pen = getPen(NULL, group, resolver, pBB);
  pathItem->setPen(*pen);
  delete pen;
  //QBrush *brush = getBrush(NULL, group, resolver, pBB);
  //pathItem->setBrush(*brush);
  //delete brush;
  item->addToGroup(pathItem);
}

void addToPath(QPainterPath &path, const CLRenderCubicBezier* cubic, const CLBoundingBox *pBB)
{
  path.cubicTo(
    pBB->getPosition().getX() + cubic->basePoint1_X().getAbsoluteValue() + cubic->basePoint1_X().getRelativeValue() / 100.0 * pBB->getDimensions().getWidth(),
    pBB->getPosition().getY() + cubic->basePoint1_Y().getAbsoluteValue() + cubic->basePoint1_Y().getRelativeValue() / 100.0 * pBB->getDimensions().getHeight(),
    pBB->getPosition().getX() + cubic->basePoint2_X().getAbsoluteValue() + cubic->basePoint2_X().getRelativeValue() / 100.0 * pBB->getDimensions().getWidth(),
    pBB->getPosition().getY() + cubic->basePoint2_Y().getAbsoluteValue() + cubic->basePoint2_Y().getRelativeValue() / 100.0 * pBB->getDimensions().getHeight(),
    pBB->getPosition().getX() + cubic->getXOffset().getAbsoluteValue() + cubic->getXOffset().getRelativeValue() / 100.0 * pBB->getDimensions().getWidth(),
    pBB->getPosition().getY() + cubic->getYOffset().getAbsoluteValue() + cubic->getYOffset().getRelativeValue() / 100.0 * pBB->getDimensions().getHeight()
    );
}

void addToPath(QPainterPath &path, const CLRenderPoint* current, const CLBoundingBox *pBB)
{
  path.lineTo(          
    pBB->getPosition().getX() + current->getXOffset().getAbsoluteValue() + current->getXOffset().getRelativeValue() / 100.0 * pBB->getDimensions().getWidth(),
    pBB->getPosition().getY() + current->getYOffset().getAbsoluteValue() + current->getYOffset().getRelativeValue() / 100.0 * pBB->getDimensions().getHeight()
    );
}
void moveToPoint(QPainterPath &path, const CLRenderPoint* current, const CLBoundingBox *pBB)
{
  path.moveTo(          
    pBB->getPosition().getX() + current->getXOffset().getAbsoluteValue() + current->getXOffset().getRelativeValue() / 100.0 * pBB->getDimensions().getWidth(),
    pBB->getPosition().getY() + current->getYOffset().getAbsoluteValue() + current->getYOffset().getRelativeValue() / 100.0 * pBB->getDimensions().getHeight()
    );
}


QPainterPath *getPath(const CLPolygon* pCurve, const CLBoundingBox *pBB)
{
  QPainterPath *path = new QPainterPath();

  const std::vector<CLRenderPoint*>& elements = *pCurve->getListOfElements();
  std::vector<CLRenderPoint*>::const_iterator it = elements.begin();
  bool first = true;
  for (; it != elements.end(); ++it)
  {
    const CLRenderPoint* current = *it;
    const CLRenderCubicBezier* cubic = dynamic_cast<const CLRenderCubicBezier*>(current);
    if (first)
    {
      moveToPoint(*path, current, pBB);
      first = false;
      continue;
    }

    if (cubic != NULL)
    {
      addToPath(*path, cubic, pBB);
    }
    else
    {
      addToPath(*path, current, pBB);
    }
  }
  
  path->closeSubpath();


  return path;
}


QPainterPath *getPath(const CLRectangle* pRect, const CLBoundingBox *pBB)
{

  double x = pBB->getPosition().getX() + pRect->getX().getAbsoluteValue() + pRect->getX().getRelativeValue() / 100.0 * pBB->getDimensions().getWidth();
  double y = pBB->getPosition().getY() + pRect->getY().getAbsoluteValue() + pRect->getY().getRelativeValue() / 100.0 * pBB->getDimensions().getHeight();
  double w = pRect->getWidth().getAbsoluteValue() + pRect->getWidth().getRelativeValue() / 100.0 * pBB->getDimensions().getWidth();
  double h = pRect->getHeight().getAbsoluteValue() + pRect->getHeight().getRelativeValue() / 100.0 * pBB->getDimensions().getHeight();
  double rx = pRect->getRadiusX().getAbsoluteValue() + pRect->getRadiusX().getRelativeValue() / 100.0 * pBB->getDimensions().getWidth();
  double ry = pRect->getRadiusY().getAbsoluteValue() + pRect->getRadiusY().getRelativeValue() / 100.0 * pBB->getDimensions().getHeight();

  QPainterPath *path = new QPainterPath();

  path->addRoundedRect(x, y, w, h, rx, ry);

  return path;
}

QPainterPath *getPath(const CLEllipse* pEllipse, const CLBoundingBox *pBB)
{

  double x = pBB->getPosition().getX() + pEllipse->getCX().getAbsoluteValue() + pEllipse->getCX().getRelativeValue() / 100.0 * pBB->getDimensions().getWidth();
  double y = pBB->getPosition().getY() + pEllipse->getCY().getAbsoluteValue() + pEllipse->getCY().getRelativeValue() / 100.0 * pBB->getDimensions().getHeight();
  double z = pBB->getPosition().getZ() + pEllipse->getCZ().getAbsoluteValue() + pEllipse->getCZ().getRelativeValue() / 100.0 * pBB->getDimensions().getDepth();
  double rx = pEllipse->getRX().getAbsoluteValue() + pEllipse->getRX().getRelativeValue() / 100.0 * pBB->getDimensions().getWidth();
  double ry = pEllipse->getRY().getAbsoluteValue() + pEllipse->getRY().getRelativeValue() / 100.0 * pBB->getDimensions().getHeight();

  QPainterPath *path = new QPainterPath();

  path->addEllipse(x-rx, y-ry, 2*rx, 2*ry);

  return path;
}

QPainterPath *getPath(const CLRenderCurve* pCurve, const CLBoundingBox *pBB)
{
  QPainterPath *path = new QPainterPath();

  const std::vector<CLRenderPoint*>& elements = *pCurve->getListOfCurveElements();
  std::vector<CLRenderPoint*>::const_iterator it = elements.begin();
  bool first = true;
  for (; it != elements.end(); ++it)
  {
    const CLRenderPoint* current = *it;
    const CLRenderCubicBezier* cubic = dynamic_cast<const CLRenderCubicBezier*>(current);
    if (first)
    {
      moveToPoint(*path, current, pBB);
      first = false;
      continue;
    }

    if (cubic != NULL)
    {
      addToPath(*path, cubic, pBB);
    }
    else
    {
      addToPath(*path, current, pBB);
    }
  }

  return path;
}

QPointF normalizePoint(const QPointF& vector)
{
  qreal length = (qreal) sqrt(vector.x()*vector.x() + vector.y()*vector.y());
    if (length == 0) length = 1;
    return QPointF(vector.x()/length, vector.y()/length);
}

void applyRotationalMapping(QPainterPath& linePath, const CLLineEnding* ending, QPointF point, QPointF second)
{
  if (!ending->getIsEnabledRotationalMapping())
    return;
  
  QPointF directionVector (point.x() - second.x(), point.y() - second.y());
  directionVector = normalizePoint(directionVector);
  
  if (directionVector.x() == 0 && directionVector.y() == 0)
    return;
  
  QPointF orthogonlVector;
              
  if (directionVector.x() == 0)
      orthogonlVector = QPointF(directionVector.y(), 0);
  else
      orthogonlVector = QPointF(-directionVector.y()*directionVector.x(),
                                    1 - directionVector.y()*directionVector.y());
  orthogonlVector = normalizePoint(orthogonlVector);
  
  QTransform rotateMatrix (directionVector.x(), directionVector.y(), orthogonlVector.x(),
                                    orthogonlVector.y(), 0, 0);
  
  linePath = rotateMatrix.map(linePath);
  
  
}

void addLineEndingToItem(QGraphicsPathItem* item, const CLLineEnding* ending, const CLGroup* group, const CLRenderResolver* resolver, QPointF point, QPointF second, QGraphicsItemGroup* itemGroup)
{
  const CLGroup* lineGroup = ending->getGroup();
  for (size_t i = 0; i < lineGroup->getNumElements(); ++i)
  {
    const CLPolygon* poly = dynamic_cast<const CLPolygon*>(lineGroup->getElement(i));
    const CLRenderCurve* rcurve = dynamic_cast<const CLRenderCurve*>(lineGroup->getElement(i));
    const CLEllipse* ellipse = dynamic_cast<const CLEllipse*>(lineGroup->getElement(i));
    const CLRectangle* rect = dynamic_cast<const CLRectangle*>(lineGroup->getElement(i));
    if (rcurve != NULL)
    {
      QPainterPath path = item->path();
      QPainterPath& linePath = *getPath(rcurve, ending->getBoundingBox());            
      applyRotationalMapping(linePath, ending, point, second);      
      linePath.translate(point);
      path.addPath(linePath);
      item->setPath(path);
    } 
    else if (poly != NULL)
    {
      QPainterPath path = item->path();
      QPainterPath& linePath = *getPath(poly, ending->getBoundingBox());            
      applyRotationalMapping(linePath, ending, point, second);      
      linePath.translate(point);
      path.addPath(linePath);
      item->setPath(path);

      if (poly->isSetFill() || group->isSetFill())
      {
        QBrush* brush = getBrush(poly, ending->getGroup(), resolver, ending->getBoundingBox());
        QPen* pen = getPen(poly, ending->getGroup(), resolver, ending->getBoundingBox());
        QGraphicsPathItem* outline = new QGraphicsPathItem(linePath);
        outline->setPen(*pen);
        outline->setBrush(*brush);
        itemGroup->addToGroup(outline);
      }

    }
    else if (ellipse != NULL)
    {
      QPainterPath path = item->path();
      QPainterPath& linePath = *getPath(ellipse, ending->getBoundingBox());            
      applyRotationalMapping(linePath, ending, point, second);      
      linePath.translate(point);
      path.addPath(linePath);
      item->setPath(path);

      if (ellipse->isSetFill() || group->isSetFill())
      {
        QBrush* brush = getBrush(ellipse, ending->getGroup(), resolver, ending->getBoundingBox());
        QPen* pen = getPen(ellipse, ending->getGroup(), resolver, ending->getBoundingBox());
        QGraphicsPathItem* outline = new QGraphicsPathItem(linePath);
        outline->setPen(*pen);
        outline->setBrush(*brush);
        itemGroup->addToGroup(outline);
      }
    }
    else if (rect != NULL)
    {
      QPainterPath path = item->path();
      QPainterPath& linePath = *getPath(rect, ending->getBoundingBox());            
      applyRotationalMapping(linePath, ending, point, second);      
      linePath.translate(point);
      path.addPath(linePath);
      item->setPath(path);

      if (rect->isSetFill() || group->isSetFill())
      {
        QBrush* brush = getBrush(rect, ending->getGroup(), resolver, ending->getBoundingBox());
        QPen* pen = getPen(rect, ending->getGroup(), resolver, ending->getBoundingBox());
        QGraphicsPathItem* outline = new QGraphicsPathItem(linePath);
        outline->setPen(*pen);
        outline->setBrush(*brush);
        itemGroup->addToGroup(outline);
      }

    }
  }
}

void fillItemFromRenderCurve(QGraphicsItemGroup *item, const CLBoundingBox *pBB,const CLRenderCurve* pCurve, const CLGroup *group, const CLRenderResolver* resolver)
{
  QPainterPath& path = *getPath(pCurve, pBB); 

  QGraphicsPathItem *pathItem = new QGraphicsPathItem(path);
  QPen *pen = getPen(pCurve, group, resolver, pBB);
  pathItem->setPen(*pen);
  delete pen;
  item->addToGroup(pathItem);
  //QBrush *brush = getBrush(NULL, group, resolver, pBB);
  //pathItem->setBrush(*brush);
  //delete brush;

  if (group -> isSetStartHead())
  {
    const CLLineEnding *line = resolver->getLineEnding(group->getStartHead());
    addLineEndingToItem(pathItem, line, group, resolver, path.elementAt(0), path.elementAt(1),item);
    
  }
   
  if (group->isSetEndHead())
  {
    const CLLineEnding *line = resolver->getLineEnding(group->getEndHead());
    addLineEndingToItem(pathItem, line, group, resolver, path.elementAt(path.elementCount()-1),path.elementAt(path.elementCount()-2),item);
  }

}


void fillItemFromPolygon(QGraphicsItemGroup *item, const CLBoundingBox *pBB,const CLPolygon* pPoly, const CLGroup *group, const CLRenderResolver* resolver)
{
  QPainterPath& path = *getPath(pPoly, pBB); 
  path.setFillRule(Qt::WindingFill);

  if (pPoly->isSetFillRule())
  {
    switch(pPoly->getFillRule())
    {
    case CLGraphicalPrimitive2D::EVENODD:
      path.setFillRule(Qt::OddEvenFill);
      break;
    case CLGraphicalPrimitive2D::NONZERO:
      path.setFillRule(Qt::WindingFill);
      break;
    }

  }

  if (group->isSetFillRule())
  {
    switch(group->getFillRule())
    {
    case CLGraphicalPrimitive2D::EVENODD:
      path.setFillRule(Qt::OddEvenFill);
      break;
    case CLGraphicalPrimitive2D::NONZERO:
      path.setFillRule(Qt::WindingFill);
      break;
    }

  } 

  QGraphicsPathItem *pathItem = new QGraphicsPathItem(path);
  QPen *pen = getPen(pPoly, group, resolver, pBB);
  pathItem->setPen(*pen);
  delete pen;
  QBrush *brush = getBrush(pPoly, group, resolver, pBB);
  pathItem->setBrush(*brush);
  delete brush;
  transform(pathItem, pPoly, group);
  

  item->addToGroup(pathItem);
}

void fillItemFromText(QGraphicsItemGroup *item, const CLBoundingBox *pBB,const CLText *pText, const CLGroup *group, const CLRenderResolver* resolver)
{
  double x = pBB->getPosition().getX() + pText->getX().getAbsoluteValue() + pText->getX().getRelativeValue() / 100.0 * pBB->getDimensions().getWidth();
  double y = pBB->getPosition().getY() + pText->getY().getAbsoluteValue() + pText->getY().getRelativeValue() / 100.0 * pBB->getDimensions().getHeight();


  QGraphicsTextItem* result = new QGraphicsTextItem(pText->getText().c_str());
  result ->setPos(x, y);

  if (pText->isSetStroke())
  {
    result->setDefaultTextColor(getColor(pText->getStroke(), resolver));
  }
  else if (group->isSetStroke())
  {
    result->setDefaultTextColor(getColor(group->getStroke(), resolver));
  }
  QFont *font = getFont(pText, group, resolver, pBB);

  if (font != NULL)
  {
    result->setFont(*font);
    if (group->isSetTextAnchor() || group->isSetVTextAnchor())
    {
      QFontMetricsF fm(*font);
      qreal width = result->boundingRect().width();
      qreal height = result->boundingRect().height();
      QPointF pos = result->pos();

      if (group->isSetTextAnchor() && width  > 0)
      {
        switch(group->getTextAnchor())
        {
        case CLText::ANCHOR_MIDDLE:
          pos.setX(pBB->getPosition().getX() + (pBB->getDimensions().getWidth() - width )/2.0);
          break;
        case CLText::ANCHOR_END:
          pos.setX(pBB->getPosition().getX() + pBB->getDimensions().getWidth() - width );
          break;
        case CLText::ANCHOR_START:
        default:
          break;
        }        
      }

      if (group->isSetVTextAnchor() && height > 0)
      {
        switch (group->getVTextAnchor())
        {
        case CLText::ANCHOR_MIDDLE:
          pos.setY(pBB->getPosition().getY()+(pBB->getDimensions().getHeight()-height)/2.0);
          break;
        case CLText::ANCHOR_BOTTOM:
          pos.setY(pBB->getPosition().getY()+(pBB->getDimensions().getHeight()-height));
          break;
        case CLText::ANCHOR_TOP:          
        default:
          break;
        }
      }

      result->setPos(pos);        
    }

    if (pText->isSetTextAnchor() || pText->isSetVTextAnchor())
    {
      QFontMetricsF fm(*font);
      qreal width = result->boundingRect().width();
      qreal height = result->boundingRect().height();
      QPointF pos = result->pos();

      if (pText->isSetTextAnchor() && width  > 0)
      {
        switch(pText->getTextAnchor())
        {
        case CLText::ANCHOR_MIDDLE:
          pos.setX(pBB->getPosition().getX() + (pBB->getDimensions().getWidth() - width )/2.0);
          break;
        case CLText::ANCHOR_END:
          pos.setX(pBB->getPosition().getX() + pBB->getDimensions().getWidth() - width );
          break;
        case CLText::ANCHOR_START:
        default:
          break;
        }        
      }

      if (pText->isSetVTextAnchor() && height > 0)
      {
        switch (pText->getVTextAnchor())
        {
        case CLText::ANCHOR_MIDDLE:
          pos.setY(pBB->getPosition().getY()+(pBB->getDimensions().getHeight()-height)/2.0);
          break;
        case CLText::ANCHOR_BOTTOM:
          pos.setY(pBB->getPosition().getY()+(pBB->getDimensions().getHeight()-height));
          break;
        case CLText::ANCHOR_TOP:          
        default:
          break;
        }
      }

      result->setPos(pos);        
    }

    delete font;
  }

  item->addToGroup(result);
}

void fillItemFromImage(QGraphicsItemGroup *item, const CLBoundingBox *pBB,const CLImage *pImage, const CLGroup *group, const CLRenderResolver* resolver)
{
  double x = pBB->getPosition().getX() + pImage->getX().getAbsoluteValue() + pImage->getX().getRelativeValue() / 100.0 * pBB->getDimensions().getWidth();
  double y = pBB->getPosition().getY() + pImage->getY().getAbsoluteValue() + pImage->getY().getRelativeValue() / 100.0 * pBB->getDimensions().getHeight();
  double w = pImage->getWidth().getAbsoluteValue() + pImage->getWidth().getRelativeValue() / 100.0 * pBB->getDimensions().getWidth();
  double h = pImage->getHeight().getAbsoluteValue() + pImage->getHeight().getRelativeValue() / 100.0 * pBB->getDimensions().getHeight();

  QFile *fileName = new QFile(pImage->getImageReference().c_str());
  if (!fileName->exists())
  {
    delete fileName;    
    std::string file = ((*CCopasiRootContainer::getDatamodelList())[0])->getReferenceDirectory() + "/" + pImage->getImageReference();
    fileName = new QFile(file.c_str());
  }
  if (!fileName->exists())
  {
    delete fileName;
    return;
  }
  QPixmap pixmap (fileName->fileName());
  pixmap = pixmap.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  delete fileName;
  QGraphicsPixmapItem* result = new QGraphicsPixmapItem(
    pixmap);
  result->setPos(x,y);
  transform(result, pImage, group);

  item->addToGroup(result);
}

void fillItemFromRectangle(QGraphicsItemGroup *item, const CLBoundingBox *pBB,const CLRectangle *pRect, const CLGroup *group, const CLRenderResolver* resolver)
{
  double x = pBB->getPosition().getX() + pRect->getX().getAbsoluteValue() + pRect->getX().getRelativeValue() / 100.0 * pBB->getDimensions().getWidth();
  double y = pBB->getPosition().getY() + pRect->getY().getAbsoluteValue() + pRect->getY().getRelativeValue() / 100.0 * pBB->getDimensions().getHeight();
  double w = pRect->getWidth().getAbsoluteValue() + pRect->getWidth().getRelativeValue() / 100.0 * pBB->getDimensions().getWidth();
  double h = pRect->getHeight().getAbsoluteValue() + pRect->getHeight().getRelativeValue() / 100.0 * pBB->getDimensions().getHeight();
  double rx = pRect->getRadiusX().getAbsoluteValue() + pRect->getRadiusX().getRelativeValue() / 100.0 * pBB->getDimensions().getWidth();
  double ry = pRect->getRadiusY().getAbsoluteValue() + pRect->getRadiusY().getRelativeValue() / 100.0 * pBB->getDimensions().getHeight();

  QGraphicsRectItem* result = new QRoundedRect(
    x,y,w,h,rx,ry);
  QPen *pen = getPen(pRect, group, resolver, pBB);  
  result->setPen(*pen);
  delete pen;
  QBrush *brush = getBrush(pRect, group, resolver, pBB);
  result->setBrush(*brush);
  delete brush;
  transform(result, pRect, group);
  item->addToGroup(result);
}

void fillItemFromGroup(QGraphicsItemGroup *item, const CLBoundingBox *bounds,const CLGroup *group, const CLRenderResolver* resolver)
{
  if (group == NULL) 
    return;
  size_t numElements = group->getNumElements();

  if (numElements == 0)
  {
    return;
  }

  for (size_t i = 0; i < numElements; ++i)
  {
    const CCopasiObject* object = group->getElement(i);
    const CLEllipse* ellipse = dynamic_cast<const CLEllipse*>(group->getElement(i));
    const CLRectangle* rect = dynamic_cast<const CLRectangle*>(group->getElement(i));
    const CLPolygon* poly = dynamic_cast<const CLPolygon*>(group->getElement(i));
    const CLCurve* curve = dynamic_cast<const CLCurve*>(group->getElement(i));
    const CLRenderCurve* rcurve = dynamic_cast<const CLRenderCurve*>(group->getElement(i));
    const CLText* text = dynamic_cast<const CLText*>(group->getElement(i));
    const CLImage* image = dynamic_cast<const CLImage*>(group->getElement(i));
    const CLGroup* childGroup = dynamic_cast<const CLGroup*>(group->getElement(i));

    if (ellipse != NULL)
    {
      fillItemFromEllipse(item,bounds, ellipse, group, resolver);
    }
    else if (rect != NULL)
    {
      fillItemFromRectangle(item,bounds, rect, group, resolver);
    }
    else if (text != NULL)
    {
      fillItemFromText(item,bounds, text, group, resolver);
    }
    else if (curve != NULL)
    {
      fillItemFromCurve(item,bounds, curve, group, resolver);
    }
    else if (rcurve != NULL)
    {
      fillItemFromRenderCurve(item,bounds, rcurve, group, resolver);
    }
    else if (poly != NULL)
    {
      fillItemFromPolygon(item,bounds, poly, group, resolver);
    }
    else if (image != NULL)
    {
      fillItemFromImage(item,bounds, image, group, resolver);
    }
    else if(childGroup != NULL)
    {
      fillItemFromGroup(item, bounds, childGroup, resolver);
    }
  }
  transform(item, group, NULL);
}

void QRenderConverter::applyStyle(QGraphicsPathItem* item, const CLBoundingBox* bounds, const CLGroup *group, const CLRenderResolver* resolver, QGraphicsItemGroup* itemGroup)
{
  if (resolver == NULL || group == NULL || bounds == NULL || item == NULL) 
    return;

  QPen *pen = getPen(NULL, group, resolver, bounds);  
  item->setPen(*pen);   
  delete pen;  
   
  QPointF start = item->path().elementAt(0);
  QPointF second = item->path().elementAt(1);
  QPointF end = item->path().elementAt(item->path().elementCount()-1);
  QPointF secondLast = item->path().elementAt(item->path().elementCount()-2);

   
  if (group -> isSetStartHead())
  {
    const CLLineEnding *line = resolver->getLineEnding(group->getStartHead());
    addLineEndingToItem(item, line, group, resolver, start, second,itemGroup);
    
  }
   
  if (group->isSetEndHead())
  {
    const CLLineEnding *line = resolver->getLineEnding(group->getEndHead());
    addLineEndingToItem(item, line, group, resolver, end, secondLast,itemGroup);
  }

}

void QRenderConverter::applyStyle(QGraphicsItemGroup *group, const CLBoundingBox* bounds, const CLGroup *style, const CLRenderResolver* resolver)
{
  if (resolver == NULL || style == NULL || bounds == NULL || group== NULL) 
    return;
  for (int i = 0; i < group->childItems().size(); ++i)
  {
  }
}

void QRenderConverter::applyStyle(QGraphicsTextItem *item, const CLBoundingBox* bounds, const CLGroup *style, const CLRenderResolver* resolver)
{
  if (resolver == NULL || style == NULL || bounds == NULL || item == NULL) 
    return;

  if (style->isSetStroke())
    item->setDefaultTextColor(getColor(style->getStroke(), resolver));

  QFont *font = getFont(NULL, style, resolver, bounds);

  if (font != NULL)
  {
    item->setFont(*font);
    if (style->isSetTextAnchor() || style->isSetVTextAnchor())
    {
      QFontMetricsF fm(*font);
      //qreal width=fm.width(item->toPlainText()) + 1;  
      qreal width = item->boundingRect().width();
      qreal height = item->boundingRect().height();
      QPointF pos = item->pos();

      if (style->isSetTextAnchor() && width  > 0)
      {
        switch(style->getTextAnchor())
        {
        case CLText::ANCHOR_MIDDLE:
          pos.setX(bounds->getPosition().getX() + (bounds->getDimensions().getWidth() - width )/2.0);
          break;
        case CLText::ANCHOR_END:
          pos.setX(bounds->getPosition().getX() + bounds->getDimensions().getWidth() - width );
          break;
        case CLText::ANCHOR_START:
        default:
          break;
        }        
      }

      if (style->isSetVTextAnchor() && height > 0)
      {
        switch (style->getVTextAnchor())
        {
        case CLText::ANCHOR_MIDDLE:
          pos.setY(bounds->getPosition().getY()+(bounds->getDimensions().getHeight()-height)/2.0);
          break;
        case CLText::ANCHOR_BOTTOM:
          pos.setY(bounds->getPosition().getY()+(bounds->getDimensions().getHeight()-height));
          break;
        case CLText::ANCHOR_TOP:          
        default:
          break;
        }
      }

      item->setPos(pos);        
    }
    delete font;
  }

}

void QRenderConverter::fillGroupFromStyle(QGraphicsItemGroup *group, const CLBoundingBox *bounds, const CLStyle *style, const CLRenderResolver* resolver)
{
  if (resolver == NULL || style == NULL|| bounds == NULL || group == NULL) 
    return;
  fillItemFromGroup(group, bounds, style->getGroup(), resolver);
}

void QRenderConverter::setBackground(QGraphicsScene *scene, const std::string& fill, const CLRenderResolver* resolver)
{
  if (resolver == NULL || scene == NULL) return;
  QBrush brush(getColor(fill, resolver));
  scene->setBackgroundBrush(brush);
}
