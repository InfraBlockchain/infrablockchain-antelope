//
// Created by Eugene Chung on 23/10/2018.
//

#ifndef YOSEMITE_NUMBER_HPP
#define YOSEMITE_NUMBER_HPP

#include <eosiolib/types.hpp>

namespace yosemite {

   constexpr inline uint128_t to_uint128(uint64_t high, uint64_t low) {
      // high | low
      uint128_t result{high};
      result <<= 64;
      return result | low;
   }

}

#endif //YOSEMITE_NUMBER_HPP
