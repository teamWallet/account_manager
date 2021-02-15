#pragma once

#include <eosio/eosio.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/symbol.hpp>
#include <eosio/transaction.hpp>

namespace dac {
    using namespace eosio;
    using namespace std;

    struct div_cfg {
        name           dividend_pool;
        name           div_claim_pool;
        name           dividend_mint;
        symbol         dividend_symbol;
        name           stake_mint;
        symbol         stake_symbol;
        uint64_t       sec_for_ref    = 3 * 24 * 60 * 60; // 3 days
        uint64_t       sec_for_div    = 24 * 60 * 60;     //  day
        time_point_sec div_start_time = time_point_sec(current_time_point().sec_since_epoch() / (24 * 3600));
    };
    struct [[eosio::table("dividendcfg"), eosio::contract("config")]] dividends_cfg {
        name     table_name;
        div_cfg  dc;
        uint64_t primary_key() const { return table_name.value; }
    };

    struct [[eosio::table("divsettings"), eosio::contract("config")]] div_setting {
        name     destination;
        uint64_t share; // the percentage this account receives from the dividend

        uint64_t primary_key() const { return destination.value; }
    };
    typedef multi_index<name("dividendcfg"), dividends_cfg> dividend_cfgs;
    typedef multi_index<"divsettings"_n, div_setting>       div_settings;

    const dividends_cfg get_div_cfg(const name &scope, const name &table_name) {
        dividend_cfgs div_cfgs = dividend_cfgs(scope, table_name.value);
        div_cfg       cfg      = div_cfgs.get(table_name.value, "ERR::CFG_NOT_FOUND::DAC dividend config");
        return cfg.dc;
    }
} // namespace dac