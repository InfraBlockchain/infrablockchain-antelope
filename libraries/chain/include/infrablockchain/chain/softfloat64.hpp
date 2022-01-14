/**
 *  @file infrablockchain/chain/softfloat.hpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */
#pragma once

#include <memory>
#include <softfloat.hpp>

namespace infrablockchain { namespace chain {

   class softfloat64 {
   public:
      static double add( double a, double b ) {
         float64_t ret = f64_add( to_softfloat64(a), to_softfloat64(b) );
         return from_softfloat64(ret);
      }

      static double sub( double a, double b ) {
         float64_t ret = f64_sub( to_softfloat64(a), to_softfloat64(b) );
         return from_softfloat64(ret);
      }

      static double div( double a, double b ) {
         float64_t ret = f64_div( to_softfloat64(a), to_softfloat64(b) );
         return from_softfloat64(ret);
      }

      static double mul( double a, double b ) {
         float64_t ret = f64_mul( to_softfloat64(a), to_softfloat64(b) );
         return from_softfloat64(ret);
      }

      static float64_t fromRaw( const uint64_t a ) { float64_t x; x.v = a; return x; }

      static float64_t zero() { return softfloat64::fromRaw( 0 ); }

      static float64_t inf() { return softfloat64::fromRaw( (uint_fast64_t)(0x7FF) << 52 ); }

      static float64_t one() { return softfloat64::fromRaw( (uint_fast64_t)(1023) << 52 ); }

      static float64_t nan() { return softfloat64::fromRaw( 0x7FFFFFFFFFFFFFFFLL ); }

      static bool isInf( float64_t a ) { return (a.v & 0x7fffffffffffffff) == 0x7ff0000000000000; }

      static bool isNaN( float64_t a ) { return (a.v & 0x7fffffffffffffff) > 0x7ff0000000000000; }

      static float64_t abs( float64_t a ) { float64_t x; x.v = a.v & ((1ULL << 63) - 1); return x; }

      static float64_t negate( float64_t a ) { float64_t x; x.v = a.v ^ (1ULL << 63); return x; }

      static float64_t pow( float64_t x, float64_t y );

      static float64_t powi( float64_t x, int y );

      static float64_t exp( float64_t x );

      static float64_t log( float64_t x );
   };

} } /// infrablockchain::chain