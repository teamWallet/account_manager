#include "abieos_numeric.hpp"

struct key_weight {
  abieos::public_key key;
  uint16_t weight;
};

struct permission_level_weight {
  eosio::permission_level permission;
  uint16_t weight;
};

struct wait_weight {
  uint32_t wait_sec;
  uint16_t weight;
};

struct authority {
  uint32_t threshold;
  std::vector<key_weight> keys;
  std::vector<permission_level_weight> accounts;
  std::vector<wait_weight> waits;
};