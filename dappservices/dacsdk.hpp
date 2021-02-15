#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/multi_index.hpp>

#include "./daccustodian_shared.hpp"
#include "./dacdirectory_shared.hpp"
#include "./dactokens_shared.hpp"

using namespace eosio;
using namespace dac;
using namespace std;

#define CPL_BIT(x, n) (x ^ (1U << (n - 1)))

#define SET_BIT(x, n) (x | (1U << (n - 1)))

#define CLEAR_BIT(x, n) (x & (~(1U << (n - 1))))

#define SET_BITS(x, n, m) (x | (((~0U) >> (32 - (m - n + 1))) << (n - 1)))

#define CLEAR_BITS(x, n, m) (x & (~(((~0U) >> (32 - (m - n + 1))) << (n - 1))))

#define GET_BIT(x, n) ((x) >> (n)&1U)

#define GET_BITS(x, n, m) (x & ((~((~0U) << (m - n + 1))) << (n - 1))) >> (n - 1)

#define EOSIO_DISPATCH_SVC(contract, methods)                                                                          \
    extern "C" {                                                                                                       \
    void apply(uint64_t receiver, uint64_t code, uint64_t action) {                                                    \
        if (code == receiver) {                                                                                        \
            switch (action) {                                                                                          \
                EOSIO_DISPATCH_HELPER(contract, methods)                                                               \
                EOSIO_DISPATCH_HELPER(contract, DAPPSERVICE_ACTIONS_COMMANDS())                                        \
            }                                                                                                          \
        }                                                                                                              \
        eosio_exit(0);                                                                                                 \
    }                                                                                                                  \
    }

#define CONTRACT_START()                                                                                               \
    CONTRACT CONTRACT_NAME() : public eosio::contract {                                                                \
        using contract::contract;                                                                                      \
                                                                                                                       \
      public:                                                                                                          \
        DAPPSERVICES_ACTIONS()

#define CONTRACT_END(methods)                                                                                          \
    }                                                                                                                  \
    ;
// EOSIO_DISPATCH_SVC(CONTRACT_NAME(), methods)

enum account_type : uint8_t { GUEST = 1, MEMBER = 2, CANDIDATE = 3, INVESTOR = 4, CUSTODIAN = 32, INVALID = 33 };

#define SDK_DAPPSERVICE_BASE()                                                                                         \
    uint32_t getperm(name user, name dac_id) {                                                                         \
        uint32_t permission = 0;                                                                                       \
        if (ismember(user, dac_id)) {                                                                                  \
            SET_BIT(permission, MEMBER);                                                                               \
        } else {                                                                                                       \
            SET_BIT(permission, GUEST);                                                                                \
        }                                                                                                              \
        if (iscandidate(user, dac_id)) {                                                                               \
            SET_BIT(permission, CANDIDATE);                                                                            \
        }                                                                                                              \
        if (iscustodian(user, dac_id)) {                                                                               \
            SET_BIT(permission, CUSTODIAN);                                                                            \
        }                                                                                                              \
        if (isinvestor(user, dac_id)) {                                                                                \
            SET_BIT(permission, INVESTOR);                                                                             \
        }                                                                                                              \
        return permission;                                                                                             \
    }                                                                                                                  \
    bool ismember(name member, name dac_id) {                                                                          \
        auto       member_terms_account = dacdir::dac_for_id(dac_id).symbol.get_contract();                            \
        regmembers members(member_terms_account, dac_id.value);                                                        \
        memterms   memberterms(member_terms_account, dac_id.value);                                                    \
        auto       mem                 = members.find(member.value);                                                   \
        auto       latest_member_terms = (--memberterms.end());                                                        \
        return (mem != members.end() && mem->agreedterms != 0 && latest_member_terms->version == mem->agreedterms)     \
                   ? true                                                                                              \
                   : false;                                                                                            \
    }                                                                                                                  \
    bool iscandidate(name cand, name dac_id) {                                                                         \
        dacdir::dac      dac               = dacdir::dac_for_id(dac_id);                                               \
        auto             custidian_account = dac.account_for_type(dacdir::CUSTODIAN);                                  \
        candidates_table candidates(custidian_account, dac_id.value);                                                  \
        auto             candidate = candidates.find(cand.value);                                                      \
        return (candidate != candidates.end()) ? true : false;                                                         \
    }                                                                                                                  \
    bool iscustodian(name cust, name dac_id) {                                                                         \
        dacdir::dac      dac               = dacdir::dac_for_id(dac_id);                                               \
        auto             custidian_account = dac.account_for_type(dacdir::CUSTODIAN);                                  \
        custodians_table custodians(custidian_account, dac_id.value);                                                  \
        auto             custodian = custodians.find(cust.value);                                                      \
        return (custodian != custodians.end()) ? true : false;                                                         \
    }                                                                                                                  \
    bool isinvestor(name investor, name dac_id) {                                                                      \
        if (!ismember(investor, dac_id)) {                                                                             \
            return false;                                                                                              \
        }                                                                                                              \
        auto         dac               = dacdir::dac_for_id(dac_id);                                                   \
        auto         token_contract    = dac.symbol.get_contract();                                                    \
        name         dividend_contract = dac.account_for_type(dacdir::DIVIDEND);                                       \
        dividendscfg div_cfg           = dividendscfg::get_dividend_config(dividend_contract, dac_id);                 \
        staking      owner_stake(dividend_contract, investor.value);                                                   \
        auto         stk = owner_stake.find(div_cfg.stake.get_symbol().code().raw());                                  \
        return stk != owner_stake.end() ? true : false;                                                                \
    }
