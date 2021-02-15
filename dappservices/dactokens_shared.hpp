#ifndef DACTOKENS_SHARED_H
#define DACTOKENS_SHARED_H

#include "common_utilities.hpp"
#include "dacdirectory_shared.hpp"
#include "dacdividend_shared.hpp"
#include <eosio/asset.hpp>

namespace dac {

    // This is a reference to the member struct as used in the dactoken contract.
    struct member {
        eosio::name sender;
        /// Hash of agreed terms
        uint64_t agreedterms;

        uint64_t primary_key() const { return sender.value; }
    };

    // This is a reference to the termsinfo struct as used in the dactoken contract.
    struct termsinfo {
        std::string terms;
        std::string hash;
        uint64_t    version;

        uint64_t primary_key() const { return version; }
    };

    typedef eosio::multi_index<"memberterms"_n, termsinfo> memterms;

    struct account {
        eosio::asset balance;

        uint64_t primary_key() const { return balance.symbol.code().raw(); }
    };

    typedef eosio::multi_index<"members"_n, member>   regmembers;
    typedef eosio::multi_index<"accounts"_n, account> accounts;

    asset get_balance(name owner, name code, symbol_code sym) {
        accounts    accountstable(code, owner.value);
        const auto &ac = accountstable.get(sym.raw());
        return ac.balance;
    }

    asset get_liquid(name owner, name code, symbol_code sym) {
        // Hardcoding a precision of 4, it doesnt matter because the index ignores precision
        dacdir::dac dac               = dacdir::dac_for_symbol(extended_symbol{symbol{sym, 4}, code});
        name        dividend_contract = dac.account_for_type(dacdir::DIVIDEND);
        staking     owner_stake(dividend_contract, owner.value);
        refunding   owner_refund(dividend_contract, owner.value);

        dividend_cfg div_cfg(dividend_contract, dac.dac_id.value);

        asset liquid = get_balance(owner, code, sym);

        if (div_cfg.exists()) {
            dividendscfg dc  = dividendscfg::get_dividend_config(dividend_contract, dac.dac_id);
            auto         stk = owner_stake.find(dc.stake.get_symbol().code().raw());
            if (stk != owner_stake.end()) {
                liquid -= stk->quantity;
            }

            refunding owner_refund(dividend_contract, owner.value);
            auto      ref = owner_refund.find(dc.stake.get_symbol().code().raw());
            if (ref != owner_refund.end()) {
                liquid -= ref->quantity;
            }
        }
        return liquid;
    }

    static void assertValidMember(eosio::name member, eosio::name dac_id) {
        eosio::name member_terms_account;

        member_terms_account =
            dacdir::dac_for_id(dac_id).symbol.get_contract(); // Need this line without the temp block
        regmembers reg_members(member_terms_account, dac_id.value);
        memterms   memberterms(member_terms_account, dac_id.value);

        const auto &regmem =
            reg_members.get(member.value, "ERR::GENERAL_REG_MEMBER_NOT_FOUND::Account is not registered with members.");
        eosio::check((regmem.agreedterms != 0),
            "ERR::GENERAL_MEMBER_HAS_NOT_AGREED_TO_ANY_TERMS::Account has not agreed to any terms");
        auto latest_member_terms = (--memberterms.end());
        eosio::check(latest_member_terms->version == regmem.agreedterms,
            "ERR::GENERAL_MEMBER_HAS_NOT_AGREED_TO_LATEST_TERMS::Agreed terms isn't the latest.");
    }
} // namespace dac

#endif
