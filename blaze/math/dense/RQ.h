//=================================================================================================
/*!
//  \file blaze/math/dense/RQ.h
//  \brief Header file for the dense matrix in-place RQ decomposition
//
//  Copyright (C) 2013 Klaus Iglberger - All Rights Reserved
//
//  This file is part of the Blaze library. You can redistribute it and/or modify it under
//  the terms of the New (Revised) BSD License. Redistribution and use in source and binary
//  forms, with or without modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list
//     of conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//  3. Neither the names of the Blaze development group nor the names of its contributors
//     may be used to endorse or promote products derived from this software without specific
//     prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
//  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
//  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
//  SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
//  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
//  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
*/
//=================================================================================================

#ifndef _BLAZE_MATH_DENSE_RQ_H_
#define _BLAZE_MATH_DENSE_RQ_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <memory>
#include <blaze/math/Aliases.h>
#include <blaze/math/constraints/Adaptor.h>
#include <blaze/math/constraints/BLASCompatible.h>
#include <blaze/math/constraints/Hermitian.h>
#include <blaze/math/constraints/Lower.h>
#include <blaze/math/constraints/StrictlyTriangular.h>
#include <blaze/math/constraints/Symmetric.h>
#include <blaze/math/constraints/UniTriangular.h>
#include <blaze/math/Exception.h>
#include <blaze/math/expressions/DenseMatrix.h>
#include <blaze/math/lapack/gerqf.h>
#include <blaze/math/lapack/orgrq.h>
#include <blaze/math/lapack/ungrq.h>
#include <blaze/math/traits/DerestrictTrait.h>
#include <blaze/math/typetraits/IsResizable.h>
#include <blaze/math/typetraits/IsSquare.h>
#include <blaze/math/views/Submatrix.h>
#include <blaze/util/algorithms/Min.h>
#include <blaze/util/EnableIf.h>


namespace blaze {

//=================================================================================================
//
//  RQ DECOMPOSITION FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\name RQ decomposition functions */
//@{
template< typename MT1, bool SO1, typename MT2, bool SO2, typename MT3, bool SO3 >
void rq( const DenseMatrix<MT1,SO1>& A, DenseMatrix<MT2,SO2>& R, DenseMatrix<MT3,SO3>& Q );
//@}
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Auxiliary function for the RQ decomposition.
// \ingroup dense_matrix
//
// \param A The RQ decomposed column-major matrix.
// \param tau Array for the scalar factors of the elementary reflectors.
// \return void
//
// This function is an auxiliary helper for the dense matrix RQ decomposition. It performs the
// reconstruction of the \c Q matrix from the RQ decomposition.
*/
template< typename MT1 >  // Type of matrix A
inline EnableIf_<IsBuiltin< ElementType_<MT1> > >
   rq_backend( MT1& A, const ElementType_<MT1>* tau )
{
   orgrq( A, tau );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Auxiliary function for the RQ decomposition.
// \ingroup dense_matrix
//
// \param A The RQ decomposed column-major matrix.
// \param tau Array for the scalar factors of the elementary reflectors.
// \return void
//
// This function is an auxiliary helper for the dense matrix RQ decomposition. It performs the
// reconstruction of the \c Q matrix from the RQ decomposition.
*/
template< typename MT1 >  // Type of matrix A
inline EnableIf_<IsComplex< ElementType_<MT1> > >
   rq_backend( MT1& A, const ElementType_<MT1>* tau )
{
   ungrq( A, tau );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*!\brief RQ decomposition of the given dense matrix.
// \ingroup dense_matrix
//
// \param A The matrix to be decomposed.
// \param R The resulting \c R matrix.
// \param Q The resulting \c Q matrix.
// \return void
// \exception std::invalid_argument Dimensions of fixed size matrix do not match.
// \exception std::invalid_argument Square matrix cannot be resized to \a m-by-\a n.
//
// This function performs the dense matrix RQ decomposition of a general \a m-by-\a n matrix.
// The resulting decomposition has the form

                              \f[ A = R \cdot Q, \f]

// where \c R is an upper trapezoidal \a m-by-min(\a m,\a n) matrix and \c Q is a general
// min(\a m,\a n)-by-\a n matrix. The decomposition is written to the two distinct matrices
// \c R and \c Q, which are resized to the correct dimensions (if possible and necessary).
//
// The function fails if ...
//
//  - ... either \a R or \a Q are fixed size matrices and the dimensions don't match;
//  - ... \a R is a compile time square matrix, but is required to be non-square.
//
// In all failure cases a \a std::invalid_argument exception is thrown.
//
// Example:

   \code
   blaze::DynamicMatrix<double,blaze::columnMajor> A( 32, 16 );
   // ... Initialization of A

   blaze::DynamicMatrix<double,blaze::columnMajor> R( 32, 16 );
   blaze::DynamicMatrix<double,blaze::columnMajor> Q( 16, 16 );


   rq( A, R, Q );

   assert( A == R * Q );
   \endcode

// \note This function only works for matrices with \c float, \c double, \c complex<float>, or
// \c complex<double> element type. The attempt to call the function with matrices of any other
// element type results in a compile time error!
//
// \note This function can only be used if a fitting LAPACK library is available and linked to
// the executable. Otherwise a call to this function will result in a linker error.
*/
template< typename MT1  // Type of matrix A
        , bool SO1      // Storage order of matrix A
        , typename MT2  // Type of matrix R
        , bool SO2      // Storage order of matrix R
        , typename MT3  // Type of matrix Q
        , bool SO3 >    // Storage order of matrix Q
void rq( const DenseMatrix<MT1,SO1>& A, DenseMatrix<MT2,SO2>& R, DenseMatrix<MT3,SO3>& Q )
{
   BLAZE_CONSTRAINT_MUST_NOT_BE_STRICTLY_TRIANGULAR_MATRIX_TYPE( MT1 );
   BLAZE_CONSTRAINT_MUST_BE_BLAS_COMPATIBLE_TYPE( ElementType_<MT1> );

   BLAZE_CONSTRAINT_MUST_NOT_BE_SYMMETRIC_MATRIX_TYPE( MT2 );
   BLAZE_CONSTRAINT_MUST_NOT_BE_HERMITIAN_MATRIX_TYPE( MT2 );
   BLAZE_CONSTRAINT_MUST_NOT_BE_UNITRIANGULAR_MATRIX_TYPE( MT2 );
   BLAZE_CONSTRAINT_MUST_NOT_BE_STRICTLY_TRIANGULAR_MATRIX_TYPE( MT2 );
   BLAZE_CONSTRAINT_MUST_NOT_BE_LOWER_MATRIX_TYPE( MT2 );
   BLAZE_CONSTRAINT_MUST_BE_BLAS_COMPATIBLE_TYPE( ElementType_<MT2> );

   BLAZE_CONSTRAINT_MUST_NOT_BE_ADAPTOR_TYPE( MT3 );
   BLAZE_CONSTRAINT_MUST_BE_BLAS_COMPATIBLE_TYPE( ElementType_<MT3> );

   using ET1 = ElementType_<MT1>;

   const size_t m( (~A).rows() );
   const size_t n( (~A).columns() );
   const size_t mindim( min( m, n ) );

   if( ( !IsResizable<MT2>::value && ( (~R).rows() != m || (~R).columns() != mindim ) ) ||
       ( !IsResizable<MT3>::value && ( (~Q).rows() != mindim || (~Q).columns() != n ) ) ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Dimensions of fixed size matrix do not match" );
   }

   if( IsSquare<MT2>::value && m != mindim ) {
      BLAZE_THROW_INVALID_ARGUMENT( "Square matrix cannot be resized to m-by-min(m,n)" );
   }

   const std::unique_ptr<ET1[]> tau( new ET1[mindim] );
   DerestrictTrait_<MT2> r( derestrict( ~R ) );

   if( m < n )
   {
      (~Q) = A;
      gerqf( ~Q, tau.get() );

      resize( ~R, m, m, false );
      reset( r );

      for( size_t i=0UL; i<m; ++i ) {
         for( size_t j=i; j<m; ++j ) {
            r(i,j) = (~Q)(i,j+n-m);
         }
      }

      rq_backend( ~Q, tau.get() );
   }
   else
   {
      r = A;
      gerqf( r, tau.get() );
      (~Q) = submatrix( r, m-n, 0UL, n, n );
      rq_backend( ~Q, tau.get() );

      for( size_t i=m-n; i<m; ++i ) {
         for( size_t j=0UL; j<i+n-m; ++j ) {
            reset( r(i,j) );
         }
      }
   }
}
//*************************************************************************************************

} // namespace blaze

#endif
