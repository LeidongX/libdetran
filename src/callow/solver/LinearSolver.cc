//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   LinearSolver.cc
 * \brief  LinearSolver member definitions
 * \author Jeremy Roberts
 * \date   Sep 26, 2012
 */
//---------------------------------------------------------------------------//

#include "LinearSolver.hh"
// preconditioners
#include "callow/preconditioner/PCILU0.hh"
#include "callow/preconditioner/PCJacobi.hh"

namespace callow
{

//-------------------------------------------------------------------------//
// CONSTRUCTOR & DESTRUCTOR
//-------------------------------------------------------------------------//

LinearSolver::LinearSolver(const double atol,
                           const double rtol,
                           const int    maxit,
                           std::string  name)
  : d_absolute_tolerance(atol)
  , d_relative_tolerance(rtol)
  , d_maximum_iterations(maxit)
  , d_residual(maxit + 1, 0)
  , d_number_iterations(0)
  , d_monitor_level(2)
  , d_monitor_diverge(true)
  , d_norm_type(L2)
  , d_name(name)
  , d_pc_side(NONE)
{
  Require(d_absolute_tolerance >= 0.0);
  Require(d_relative_tolerance >= 0.0);
  Require(d_maximum_iterations >  0);
}

//-------------------------------------------------------------------------//
// PUBLIC FUNCTIONS
//-------------------------------------------------------------------------//

void LinearSolver::
set_operators(SP_matrix A,
              SP_db db)
{
  // Preconditions
  Require(A);
  d_A = A;
  Ensure(d_A->number_rows() == d_A->number_columns());

  std::string pc_type = "";
  int pc_side = LEFT;

  if (db)
  {
    if(db->check("pc_type"))
      pc_type = db->get<std::string>("pc_type");
    if (pc_type == "ilu0")
    {
      d_P = new PCILU0(d_A);
    }
    else if (pc_type == "jacobi")
    {
      d_P = new PCJacobi(d_A);
    }
    if(db->check("pc_side"))
      pc_side = db->get<int>("pc_side");
  }

}

void LinearSolver::
set_tolerances(const double atol, const double rtol, const int maxit)
{
  d_absolute_tolerance = atol;
  d_relative_tolerance = rtol;
  d_maximum_iterations = maxit;
  Require(d_absolute_tolerance > 0.0);
  Require(d_relative_tolerance > 0.0);
  Require(d_maximum_iterations >= 0);
}

// print out iteration and residual for initial
bool LinearSolver::monitor_init(double r)
{
  d_residual[0] = r;
  if (d_monitor_level > 1)
    printf("iteration: %5i    residual: %12.8e \n", 0, r);
  if (r < d_absolute_tolerance)
  {
    if (d_monitor_level > 0)
    {
      printf("*** %s converged in %5i iterations with a residual of %12.8e \n",
             d_name.c_str(), 0, r );
    }
    d_status = SUCCESS;
    return true;
  }
  return false;
}

// print out iteration and residual
bool LinearSolver::monitor(int it, double r)
{
  d_number_iterations = it;
  d_residual[it] = r;
  if (d_monitor_level > 1)
    printf("iteration: %5i    residual: %12.8e \n", it, r);
 // Assert(it > 0);
  if (r < std::max(d_relative_tolerance * d_residual[0],
                   d_absolute_tolerance))
  {
    if (d_monitor_level)
    {
      printf("*** %s converged in %5i iterations with a residual of %12.8e \n",
             d_name.c_str(), it, r );
    }
    d_status = SUCCESS;
    return true;
  }
  else if (d_monitor_diverge and it >  1 and r - d_residual[it - 1] > 0.0)
  {
    if (d_monitor_level) printf("*** %s diverged \n", d_name.c_str());
    d_status = DIVERGE;
    return true;
  }
  return false;
}


} // end namespace callow

//---------------------------------------------------------------------------//
//              end of file LinearSolver.cc
//---------------------------------------------------------------------------//
