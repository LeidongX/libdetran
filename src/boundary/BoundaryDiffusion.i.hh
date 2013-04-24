//----------------------------------*-C++-*----------------------------------//
/**
 *  @file   BoundaryDiffusion.i.hh
 *  @author robertsj
 *  @date   Sep 11, 2012
 *  @brief  BoundaryDiffusion inline member definitions
 */
//---------------------------------------------------------------------------//

#ifndef detran_BOUNDARYDIFFUSION_I_HH_
#define detran_BOUNDARYDIFFUSION_I_HH_

#include <iostream>

namespace detran
{

//---------------------------------------------------------------------------//
template <class D>
inline const typename BoundaryDiffusion<D>::bf_type&
BoundaryDiffusion<D>::operator()(const size_t side,
                                 const size_t g,
                                 const size_t inout) const
{
  Require(side < D::dimension * 2);
  Require(g < d_number_groups);
  Require(inout < 2);
  return d_boundary_flux[inout][side][g];
}

//---------------------------------------------------------------------------//
template <class D>
inline typename BoundaryDiffusion<D>::bf_type&
BoundaryDiffusion<D>::operator()(const size_t side,
                                 const size_t g,
                                 const size_t inout)
{
  return const_cast<typename BoundaryDiffusion<D>::bf_type&>
  (
    static_cast<const BoundaryDiffusion<D>&>(*this)(side, g, inout)
  );
}

//---------------------------------------------------------------------------//
template <class D>
inline void BoundaryDiffusion<D>::clear(const size_t g)
{
  THROW("NOT IMPLEMENTED");
}

//---------------------------------------------------------------------------//
template <>
inline void BoundaryDiffusion<_3D>::clear(const size_t g)
{
  for (size_t inout = 0; inout < 2; ++inout)
    for (size_t side = 0; side < 6; ++side)
      for (size_t j = 0; j < d_boundary_flux[inout][side][g].size(); ++j)
        for (size_t i = 0; i < d_boundary_flux[inout][side][g][0].size(); ++i)
          d_boundary_flux[inout][side][g][j][i] = 0.0;

}

//---------------------------------------------------------------------------//
template <>
inline void BoundaryDiffusion<_2D>::clear(const size_t g)
{
  for (size_t inout = 0; inout < 2; ++inout)
    for (size_t side = 0; side < 4; ++side)
      for (size_t i = 0; i < d_boundary_flux[inout][side][g].size(); ++i)
        d_boundary_flux[inout][side][g][i] = 0.0;
}

//---------------------------------------------------------------------------//
template <>
inline void BoundaryDiffusion<_1D>::clear(const size_t g)
{
  for (size_t inout = 0; inout < 2; ++inout)
    for (size_t side = 0; side < 2; ++side)
      d_boundary_flux[inout][side][g] = 0.0;
}

} // end namespace detran

#endif /* detran_BOUNDARYDIFFUSION_I_HH_ */
