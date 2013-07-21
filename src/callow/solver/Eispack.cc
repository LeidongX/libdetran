//----------------------------------*-C++-*-----------------------------------//
/**
 *  @file  Eispack.cc
 *  @brief Eispack member definitions
 *  @note  Copyright (C) 2013 Jeremy Roberts
 */
//----------------------------------------------------------------------------//

#include "Eispack.hh"
#include "callow/matrix/MatrixDense.hh"
#include <complex>

namespace callow
{

//----------------------------------------------------------------------------//
Eispack::Eispack(const double tol,
                 const int    maxit)
  : EigenSolver(tol, maxit, "eispack")
  , d_which_value(0)
{
  /* ... */
}

//----------------------------------------------------------------------------//
void Eispack::set_operators(SP_matrix  A,
                            SP_matrix  B,
                            SP_db      db)
{
  Require(A);
  Insist(dynamic_cast<MatrixDense*>(A.bp()),
    "Need a dense matrix for use with Eispack");
  d_A = A;
  if (B)
  {
    Insist(dynamic_cast<MatrixDense*>(B.bp()),
      "Need a dense matrix for use with Eispack");
    d_B = B;
  }
}

//----------------------------------------------------------------------------//
void Eispack::solve_complete(MatrixDense &V_R,
                             MatrixDense &V_I,
                             Vector      &E_R,
                             Vector      &E_I)
{
  Require(d_A);
  MatrixDense &A = *dynamic_cast<MatrixDense*>(d_A.bp());
  // Use copies so the originals are not overwritten
  MatrixDense tmp_A(A);
  //MatrixDense &tmp_A = A;
  int m    = A.number_rows();
  int matz = 1; // positive to get the eigenvectors
  int ierr = 0;

  if (d_B)
  {
    MatrixDense &B = *dynamic_cast<MatrixDense*>(d_B.bp());
    // Use copies so the originals are not overwritten
    MatrixDense tmp_B(B);
    //MatrixDense &tmp_B = B;
    Vector E_D(m, 0.0);
    matz=1;
    tmp_A.print_matlab("A.out");
    tmp_B.print_matlab("B.out");
    rgg_(&m, &m, &tmp_A[0], &tmp_B[0],
         &E_R[0], &E_I[0], &E_D[0], &matz, &V_R[0], &ierr);
    Assert(!ierr);
    // scale the eigenvalues
//    E_R.display("E_R");
//    E_I.display("E_I");
//    E_D.display("E_D");
    E_R.divide(E_D);
    E_I.divide(E_D);
    E_R.display("E_R scaled");
    E_I.display("E_I scaled");
    V_R.display("lalala");
    tmp_A.print_matlab("A2.out");
    tmp_B.print_matlab("B2.out");
  }
  else
  {
    //nm, n, a, wr, wi, matz, z, iv1, fv1, ierr)
    int* iv1(new int[m]);
    double *fv1(new double[m]);
    rg_(&m, &m, &tmp_A[0],
        &E_R[0], &E_I[0], &matz, &V_R[0], iv1, fv1, &ierr);
    E_R.display("E_R");
    delete [] iv1;
    delete [] fv1;
  }

}

//----------------------------------------------------------------------------//
void Eispack::solve_impl(Vector &x, Vector &x0)
{
  int m = d_A->number_columns();
  MatrixDense V_R(m, m, 0.0);
  MatrixDense V_I(m, m, 0.0);
  Vector E_R(m, 0.0);
  Vector E_I(m, 0.0);

  solve_complete(V_R, V_I, E_R, E_I); E_R.display(); E_I.display();

  // Extract the eigenvector corresponding to the maximum real value
  double max_E = E_R[0];
  int    max_i = 0;
  for (int i = 1; i < m; ++i)
  {
    if (E_R[i] > max_E)
    {
      max_E = E_R[i];
      max_i = i;
    }
  }
  for (int i = 0; i < m; ++i)
  {
    x[i] = V_R(i, max_i);
  }
  x.scale(1.0 / x.norm(L2));

  /// Store the eigenvalue
  d_lambda = max_E;
}


} // end namespace callow

//----------------------------------------------------------------------------//
//              end of file Eispack.cc
//----------------------------------------------------------------------------//


