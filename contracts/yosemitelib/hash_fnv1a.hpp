//
// Created by eugene on 10/17/18.
//

#ifndef YOSEMITE_HASH_FNV1A_HPP
#define YOSEMITE_HASH_FNV1A_HPP

#include <stdint.h>

namespace yosemite {

   //fnv1a 32 and 64 bit hash functions
   // key is the data to hash, len is the size of the data (or how much of it to hash against)
   // code license: public domain or equivalent
   // post: https://notes.underscorediscovery.com/constexpr-fnv1a/
   inline const uint32_t hash_32_fnv1a(const void* key, const uint32_t len) {

      const char* data = (char*)key;
      uint32_t hash = 0x811c9dc5;
      uint32_t prime = 0x1000193;

      for(int i = 0; i < len; ++i) {
         uint8_t value = data[i];
         hash = hash ^ value;
         hash *= prime;
      }

      return hash;

   } //hash_32_fnv1a

   inline const uint64_t hash_64_fnv1a(const void* key, const uint64_t len) {

      const char* data = (char*)key;
      uint64_t hash = 0xcbf29ce484222325;
      uint64_t prime = 0x100000001b3;

      for(int i = 0; i < len; ++i) {
         uint8_t value = data[i];
         hash = hash ^ value;
         hash *= prime;
      }

      return hash;
   }

   // FNV1a c++11 constexpr compile time hash functions, 32 and 64 bit
   // str should be a null terminated string literal, value should be left out
   // e.g hash_32_fnv1a_const("example")
   // code license: public domain or equivalent
   // post: https://notes.underscorediscovery.com/constexpr-fnv1a/

   constexpr uint32_t val_32_const = 0x811c9dc5;
   constexpr uint32_t prime_32_const = 0x1000193;
   constexpr uint64_t val_64_const = 0xcbf29ce484222325;
   constexpr uint64_t prime_64_const = 0x100000001b3;

   inline constexpr uint32_t hash_32_fnv1a_const(const char* const str, const uint32_t value = val_32_const) noexcept {
      return (str[0] == '\0') ? value : hash_32_fnv1a_const(&str[1], (value ^ uint32_t(str[0])) * prime_32_const);
   }

   inline constexpr uint64_t hash_64_fnv1a_const(const char* const str, const uint64_t value = val_64_const) noexcept {
      return (str[0] == '\0') ? value : hash_64_fnv1a_const(&str[1], (value ^ uint64_t(str[0])) * prime_64_const);
   }

}

#endif //YOSEMITE_HASH_FNV1A_HPP
