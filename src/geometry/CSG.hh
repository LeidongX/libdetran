//----------------------------------*-C++-*-----------------------------------//
/**
 *  @file  CSG.hh
 *  @brief Classes to represent CSG tree
 *  @note  Copyright (C) 2013 Jeremy Roberts
 */
//----------------------------------------------------------------------------//

#ifndef detran_geometry_CSG_HH_
#define detran_geometry_CSG_HH_

#include "Point.hh"
#include "QuadraticSurface.hh"
#include "utilities/DBC.hh"

namespace detran_geometry
{

enum NODE_OPERATORS
{
  UNION, INTERSECTION, SUBSTRACTION, END_NODE_OPERATORS
};

/**
 *  @class CSG_Node
 *  @brief Base class for nodes in a CSG tree
 */
class CSG_Node
{

public:

  //--------------------------------------------------------------------------//
  // TYPEDEFS
  //--------------------------------------------------------------------------//

  typedef detran_utilities::SP<CSG_Node>    SP_node;
  typedef detran_utilities::size_t          size_t;
  typedef std::vector<Point>                vec_point;
  typedef const Point                       c_Point;
  typedef const double                      c_double;

  //--------------------------------------------------------------------------//
  // PUBLIC FUNCTIONS
  //--------------------------------------------------------------------------//

  /// Virtual destructor
  virtual ~CSG_Node() {/* ... */}
  /// Does the node contain the point?
  virtual bool contains(c_Point &r) const = 0;
  /// Where does the node intersect the ray?
  virtual vec_point intersections(c_Point &r, c_Point &d, c_double t_max) = 0;

};

/// Comparison of two points with respect to the given origin
struct IntersectionPointCompare
{
  IntersectionPointCompare(const Point &o) : d_origin(o) {/* ... */}
  bool operator() (const Point &r0, const Point &r1)
  {
    return distance(r0, d_origin) < distance(r1, d_origin);
  }
  const Point &d_origin;
};

/**
 *  @class CSG_Primitive
 *  @brief A leaf (i.e.) terminal node consisting of a surface
 */
class CSG_Primitive: public CSG_Node
{

public:

  //--------------------------------------------------------------------------//
  // TYPEDEFS
  //--------------------------------------------------------------------------//

  typedef Surface::SP_surface   SP_surface;

  //--------------------------------------------------------------------------//
  // PUBLIC FUNCTIONS
  //--------------------------------------------------------------------------//

  /// Constructor.  The node is "in" (true) or "out" (false) of the surface.
  CSG_Primitive(SP_surface surface, bool sense);
  bool contains(c_Point &r) const;
  vec_point intersections(c_Point &r, c_Point &d, c_double t_max);

private:

  /// The surface defining this leaf
  SP_surface d_surface;
  /// The sense of the surface
  bool d_sense;

};

/**
 *  @class CSG_Operator
 *  @brief An intermediate node representing operation between two nodes
 */
class CSG_Operator: public CSG_Node
{

public:

  CSG_Operator(SP_node L, SP_node R);
  //virtual bool contains(c_Point &r) const = 0;
  vec_point intersections(c_Point &r, c_Point &d, c_double t_max);

protected:

  SP_node d_L;
  SP_node d_R;

};

/// Union of two nodes
class CSG_Union: public CSG_Operator
{
public:
  CSG_Union(SP_node L, SP_node R);
  bool contains(c_Point &r) const;
};

/// Intersection of two nodes
class CSG_Intersection: public CSG_Operator
{
public:
  CSG_Intersection(SP_node L, SP_node R);
  bool contains(c_Point &r) const;
};

/// Difference of two nodes
class CSG_Difference: public CSG_Operator
{
public:
  CSG_Difference(SP_node L, SP_node R);
  bool contains(c_Point &r) const;
};

} // end namespace detran_geometry

#endif /* detran_geometry_CSG_HH_ */

//----------------------------------------------------------------------------//
//              end of file CSG.hh
//----------------------------------------------------------------------------//