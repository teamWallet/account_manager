#include "dacdirectory_shared.hpp"
#include <eosio/eosio.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/singleton.hpp>

using namespace std;
using namespace eosio;
namespace dac {

    struct account_stake_delta {
        name     account;
        asset    stake_delta;
        uint32_t unstake_delay;
    };

    struct account_balance_delta {
        eosio::name  account;
        eosio::asset balance_delta;
    };

    struct account_weight_delta {
        eosio::name account;
        int64_t     weight_delta;
    };

    struct dividendscfg;
    typedef singleton<name("dividendcfg"), dividendscfg> dividend_cfg;
    struct [[eosio::table("dividendcfg"), eosio::contract("dacdividend")]] dividendscfg {
        name            dividend_pool;
        name            div_claim_pool;
        extended_symbol dividend;
        extended_symbol stake;
        uint64_t        sec_for_ref    = 3 * 24 * 60 * 60; // 3 days
        uint64_t        sec_for_div    = 24 * 60 * 60;     //  day
        time_point_sec  div_start_time = time_point_sec(current_time_point().sec_since_epoch() / (24 * 3600));

        static dividendscfg get_dividend_config(eosio::name account, eosio::name scope) {
            return dividend_cfg(account, scope.value).get_or_default(dividendscfg{});
        }

        void save(eosio::name account, eosio::name scope, eosio::name payer = same_payer) {
            dividend_cfg(account, scope.value).set(*this, payer);
        }
    };

    struct [[eosio::table("divsetting"), eosio::contract("dacdividend")]] div_setting {
        name     destination;
        uint64_t share; // the percentage this account receives from the dividend

        uint64_t primary_key() const { return destination.value; }
    };

    typedef multi_index<"divsetting"_n, div_setting> div_settings;

    struct [[eosio::table("stake"), eosio::contract("dacdividend")]] stake_details {
        asset          quantity;
        time_point_sec updated_on;
        time_point_sec last_claim;

        uint64_t primary_key() const { return quantity.symbol.code().raw(); }
    };

    struct [[eosio::table("refund"), eosio::contract("dacdividend")]] refund_details {
        asset          quantity;
        time_point_sec updated_on;

        uint64_t primary_key() const { return quantity.symbol.code().raw(); }
    };

    typedef eosio::multi_index<"stake"_n, stake_details>   staking;
    typedef eosio::multi_index<"refund"_n, refund_details> refunding;

    struct [[eosio::table("history"), eosio::contract("dacdividend")]] stake_history {
        asset          quantity;
        time_point_sec updated_on;

        uint64_t primary_key() const { return updated_on.sec_since_epoch(); }
    };

    typedef eosio::multi_index<"history"_n, stake_history> history;

    // dividend singleton
    // use a flatmap for cheaper history
    struct [[eosio::table("divdata"), eosio::contract("dacdividend")]] div_data {
        asset dividend;
        asset total_stake;
    };

    struct dividendinfo;

    typedef eosio::singleton<"divsinfo"_n, dividendinfo> dividend_info;

    struct [[eosio::table("divsinfo"), eosio::contract("dacdividend")]] dividendinfo {
        asset                         total_stake = asset(0, symbol("DAC", 4));
        asset                         reserve     = asset(0, symbol("EOS", 4));
        bool                          frozen      = false;
        map<time_point_sec, div_data> dividends   = {}; // time,div_data

        static dividendinfo get_dividend_info(eosio::name account, eosio::name scope) {
            return dividend_info(account, scope.value).get_or_default(dividendinfo{});
        }

        void save(eosio::name account, eosio::name scope, eosio::name payer = same_payer) {
            dividend_info(account, scope.value).set(*this, payer);
        }
    };

    asset get_staked(name owner, name code, symbol_code sym) {
        // Hardcoding a precision of 4, it doesnt matter because the index ignores precision

        dacdir::dac dac               = dacdir::dac_for_symbol(extended_symbol{symbol{sym, 4}, code});
        name        dividend_contract = dac.account_for_type(dacdir::DIVIDEND);
        staking     owner_stake(dividend_contract, owner.value);

        asset staked = asset{0, symbol{sym, 4}};

        dividend_cfg div_cfg(dividend_contract, dac.dac_id.value);
        if (div_cfg.exists()) {
            dividendscfg dc  = dividendscfg::get_dividend_config(dividend_contract, dac.dac_id);
            auto         stk = owner_stake.find(dc.stake.get_symbol().code().raw());
            if (stk != owner_stake.end()) {
                staked += stk->quantity;
            }
        }

        return staked;
    }
} // namespace dac