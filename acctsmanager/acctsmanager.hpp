#pragma once

#include "../_contract-shared-headers/comm.hpp"
#include "../_contract-shared-headers/comm_struct.hpp"
#include "../dappservices/dacsdk.hpp"

#include <eosio/action.hpp>
#include <eosio/asset.hpp>
#include <eosio/binary_extension.hpp>
#include <eosio/crypto.hpp>
#include <eosio/eosio.hpp>
#include <eosio/ignore.hpp>
#include <eosio/permission.hpp>
#include <eosio/singleton.hpp>
#include <eosio/system.hpp>
#include <eosio/transaction.hpp>

using namespace eosio;
using namespace std;

#define _STRINGIZE(x) #x
#define STRINGIZE(x) _STRINGIZE(x)

#ifdef DACCUSTCONTRACT
#define DACCUST_CONTRACT STRINGIZE(DACCUSTCONTRACT)
#endif
#ifndef DACCUST_CONTRACT
#define DACCUST_CONTRACT "daccustodian"
#endif

#ifdef DACPROPCONTRACT
#define DACPROP_CONTRACT STRINGIZE(DACPROPCONTRACT)
#endif
#ifndef DACPROP_CONTRACT
#define DACPROP_CONTRACT "dacproposals"
#endif

#ifdef DACDIRCONTRACT
#define DACDIR_CONTRACT STRINGIZE(DACDIRCONTRACT)
#endif
#ifndef DACDIR_CONTRACT
#define DACDIR_CONTRACT "dacdirectory"
#endif

#ifdef DACTOKENCONTRACT
#define DACTOKEN_CONTRACT STRINGIZE(DACTOKENCONTRACT)
#endif
#ifndef DACTOKEN_CONTRACT
#define DACTOKEN_CONTRACT "dacdactokens"
#endif

#ifdef DACESCROWCONTRACT
#define DACESCROW_CONTRACT STRINGIZE(DACESCROWCONTRACT)
#endif
#ifndef DACESCROW_CONTRACT
#define DACESCROW_CONTRACT "dacdacescrow"
#endif

#ifdef DACMSIGCONTRACT
#define DACMSIG_CONTRACT STRINGIZE(DACMSIGCONTRACT)
#endif
#ifndef DACMSIG_CONTRACT
#define DACMSIG_CONTRACT "dacmultisigs"
#endif

#ifdef SYSTOKENCONTRACT
#define SYSTOKEN_CONTRACT STRINGIZE(SYSTOKENCONTRACT)
#endif
#ifndef SYSTOKEN_CONTRACT
#define SYSTOKEN_CONTRACT "eosio.token"
#endif

#ifdef MSIGCONTRACT
#define MSIG_CONTRACT STRINGIZE(MSIGCONTRACT)
#endif
#ifndef MSIG_CONTRACT
#define MSIG_CONTRACT "eosio.msig"
#endif

#ifdef DEXCONTRACT
#define DEX_CONTRACT STRINGIZE(DEXCONTRACT)
#endif
#ifndef DEX_CONTRACT
#define DEX_CONTRACT "dex4everyone"
#endif

const eosio::name ADMIN_PERMISSION  = "admin"_n;
const eosio::name ONE_PERMISSION    = "one"_n;
const eosio::name LOW_PERMISSION    = "low"_n;
const eosio::name MEDIUM_PERMISSION = "med"_n;
const eosio::name HIGH_PERMISSION   = "high"_n;

const int64_t MAX = eosio::asset::max_amount;

typedef void (*call_action)(name code, std::string paramstr, std::string delimiter);

/*! \brief interaction 内部消息发送定义 */
typedef struct call_action_item {
    /* 信令枚举 */
    string func_name;

    /* 发送消息处理函数 */
    call_action call_action_func;
} call_action_item_t;

#define CONTRACT_NAME() acctsmanager
#define DAPPSERVICES_ACTIONS() uint32_t test;

CONTRACT_START()
acctsmanager(eosio::name receiver, eosio::name code, datastream<const char *> ds) : contract(receiver, code, ds){};

#if MAINNET
constexpr static auto core_symbol = symbol("EOS", 4);
#else
constexpr static auto core_symbol = symbol("BOS", 4);
#endif
struct newaccount_info {
    name      creator;
    name      name;
    authority owner;
    authority active;
};

// Struct for the stats table
TABLE stat {
    // Number of accounts currently created
    uint64_t num_created = 0;
};

typedef singleton<"stat"_n, stat> stat_table;

TABLE config {
    time_point_sec expiration = time_point_sec(current_time_point()) + 3600 * 24 * 365;
    uint32_t       rambytes   = 409600;
    asset          stake_cpu  = asset{5'0000, core_symbol};
    asset          stake_net  = asset{5'0000, core_symbol};
};

typedef singleton<"config"_n, config> config_table;

//   // Constructor
//   acctsmanager(name self, name code, datastream<const char *> ds)
//       : eosio::contract(self, code, ds) {}

ACTION init();
ACTION newacct(const name &acct);
ACTION regainowner(const name &acct, const authority &auth);
ACTION xtransfer(name from, name to, extended_asset fo_quantity, std::string memo);
ACTION setconfig(const config &cfg);
ACTION deleteconfig();
ACTION extend(uint32_t extend_seconds);

ACTION xupdateauth(name account, name permission, name parent, authority auth);
ACTION xlinkauth(name account, name code, name type, name requirement);

ACTION treasuryxfer(name from, name to, extended_asset fo_quantity, std::string memo);

ACTION linkdacauth(name auth_acct, name trea_acct, uint32_t trans_sec);
ACTION callaction(name code, std::string func_name, std::string paramstr, std::string delimiter);

// eosio.msig
ACTION propose(name proposer, name proposal_name, std::vector<permission_level> requested, transaction trx);
ACTION approve(name proposer, name proposal_name, permission_level level,
    const eosio::binary_extension<eosio::checksum256> &proposal_hash);
ACTION unapprove(name proposer, name proposal_name, permission_level level);
ACTION cancel(name proposer, name proposal_name, name canceler);
ACTION exec(name proposer, name proposal_name, name executer);

// dacmultisigs
ACTION proposed(name proposer, name proposal_name, string metadata, name dac_id);
ACTION approved(name proposer, name proposal_name, name approver, name dac_id);
ACTION unapproved(name proposer, name proposal_name, name unapprover, name dac_id);
ACTION deny(name proposer, name proposal_name, name denyer, name dac_id);
ACTION cancelled(name proposer, name proposal_name, name canceler, name dac_id);
ACTION executed(name proposer, name proposal_name, name executer, name dac_id);

// dacdirectory
ACTION regdac(name auth_acct, name trea_acct, name dac_id, extended_symbol dac_symbol, string title,
    map<uint8_t, string> refs, map<uint8_t, eosio::name> accounts);

// dacdactokens
ACTION tokencreate(name issuer, asset maximum_supply);
ACTION tokenissue(name to, asset quantity, string memo);
ACTION tokenburn(name from, asset quantity);
ACTION newmemterms(string terms, string hash, name dac_id);

// custodian
ACTION votecust(name voter, std::vector<name> newvotes, name dac_id);
ACTION appointcust(vector<name> cust, name dac_id);
ACTION updateconfig(contr_config new_config, name dac_id);

// exchange
ACTION inittoken(name user, symbol new_symbol, extended_asset initial_pool1, extended_asset initial_pool2,
    int initial_fee, name fee_contract);

ACTION openext(name user, const name &payer, const extended_symbol &ext_symbol);
ACTION closeext(const name &user, const name &to, const extended_symbol &ext_symbol, string memo);
ACTION withdraw(name user, name to, extended_asset to_withdraw, string memo);
ACTION addliquidity(name user, asset to_buy, asset max_asset1, asset max_asset2);
ACTION remliquidity(name user, asset to_sell, asset min_asset1, asset min_asset2);
ACTION exchange(name user, symbol_code pair_token, extended_asset ext_asset_in, asset min_expected);
ACTION exchangev2(
    name user, symbol_code pair_token1, symbol_code pair_token2, extended_asset ext_asset_in, asset min_expected);
// token close
ACTION close(const name &owner, const symbol &symbol);

// makecontract
ACTION createcontr(name id, name party_a, name party_b, string title, string summary, string content_hash,
    extended_asset contract_pay);

private:
// Update the auth for acct_name
void account_auth(name acct_name, name changeto, name perm_child, name perm_parent, string pubkey_str) {
    // Setup authority for contract. Choose either a new key, or account, or
    // both.
    authority contract_authority;
    contract_authority.threshold = 1;
    contract_authority.waits     = {};
    if (pubkey_str != "None") {
        // Convert the public key string to the requried type
        const abieos::public_key pubkey = abieos::string_to_public_key(pubkey_str);

        key_weight kweight{
            .key    = pubkey,
            .weight = 1,
        };

        // Key is supplied
        contract_authority.keys     = {kweight};
        contract_authority.accounts = {};
    } else {
        // Account to take over permission changeto@perm_child
        permission_level_weight accounts{.permission = permission_level{changeto, perm_child}, .weight = (uint16_t)1};

        // Key is not supplied
        contract_authority.keys     = {};
        contract_authority.accounts = {accounts};
    }

    // Remove contract permissions and replace with changeto account.
    action(permission_level{acct_name, name("owner")}, name("eosio"), name("updateauth"),
        std::make_tuple(acct_name, perm_child, perm_parent, contract_authority))
        .send();
}
void create_account(const name &account, const authority &owner, const authority &active, const uint32_t &ram,
    const asset &net, const asset &cpu) {
    newaccount_info new_account =
        newaccount_info{.creator = get_self(), .name = account, .owner = owner, .active = active};

    action(permission_level{get_self(), "newacct"_n}, name("eosio"), name("newaccount"),
        std::make_tuple(get_self(), account, owner, active))
        .send();

    action(permission_level{get_self(), "newacct"_n}, name("eosio"), name("buyrambytes"),
        make_tuple(get_self(), account, ram))
        .send();

    action(permission_level{get_self(), "newacct"_n}, name("eosio"), name("delegatebw"),
        make_tuple(get_self(), account, net, cpu, true))
        .send();
}

// function for action proxy
static asset asset_from_string(const string &from) {
    string s = trim(from);

    // Find space in order to split amount and symbol
    auto space_pos = s.find(' ');
    check((space_pos != string::npos), "Asset's amount and symbol should be separated with space");
    auto symbol_str = trim(s.substr(space_pos + 1));
    auto amount_str = s.substr(0, space_pos);

    // Ensure that if decimal point is used (.), decimal fraction is specified
    auto dot_pos = amount_str.find('.');
    if (dot_pos != string::npos) {
        check((dot_pos != amount_str.size() - 1), "Missing decimal fraction after decimal point");
    }

    // Parse symbol
    uint8_t precision_digit;
    if (dot_pos != string::npos) {
        precision_digit = amount_str.size() - dot_pos - 1;
    } else {
        precision_digit = 0;
    }

    symbol sym = symbol(symbol_str, precision_digit);

    // Parse amount
    int64_t int_part, fract_part;
    if (dot_pos != string::npos) {
        int_part   = stoull(amount_str.substr(0, dot_pos));
        fract_part = stoull(amount_str.substr(dot_pos + 1));
        if (amount_str[0] == '-')
            fract_part *= -1;
    } else {
        int_part = stoull(amount_str);
    }

    int64_t amount = int_part;

    uint64_t p10 = 1;
    uint64_t p   = sym.precision();
    while (p > 0) {
        p10 *= 10;
        --p;
    }

    int128_t tmp = (int128_t)amount * (int128_t)p10;
    eosio::check(tmp <= MAX, "ERR::OVERFLOW::multiplication overflow");
    eosio::check(tmp >= -MAX, "ERR::OVERFLOW::multiplication underflow");

    amount = (int64_t)tmp;

    amount += fract_part;
    eosio::check(-MAX <= amount, "ERR::OVERFLOW::addition underflow");
    eosio::check(amount <= MAX, "ERR::OVERFLOW::addition overflow");

    return asset(amount, sym);
}
static void memberreg_req(name code, std::string paramstr, std::string delimiter) {
    vector<std::string> v;

    split(trim(paramstr), delimiter, v);
    auto sender      = name{v[0]};
    auto agreedterms = v[1];
    auto dac_id      = name{v[2]};

    eosio::action(
        eosio::permission_level{sender, "active"_n}, code, "memberreg"_n, make_tuple(sender, agreedterms, dac_id))
        .send();
}
static void memberunreg_req(name code, std::string paramstr, std::string delimiter) {
    vector<std::string> v;

    split(trim(paramstr), delimiter, v);
    auto sender = name{v[0]};
    auto dac_id = name{v[1]};
    eosio::action(eosio::permission_level{sender, "active"_n}, code, "memberunreg"_n, make_tuple(sender, dac_id))
        .send();
}
static void nominatecand_req(name code, std::string paramstr, std::string delimiter) {
    vector<std::string> v;

    split(trim(paramstr), delimiter, v);

    auto cand         = name{v[0]};
    auto requestedpay = asset(asset_from_string(v[1]));
    auto dac_id       = name{v[2]};

    eosio::action(
        eosio::permission_level{cand, "active"_n}, code, "nominatecand"_n, make_tuple(cand, requestedpay, dac_id))
        .send();
}
static void withdrawcand_req(name code, std::string paramstr, std::string delimiter) {
    vector<std::string> v;

    split(trim(paramstr), delimiter, v);

    auto cand   = name{v[0]};
    auto dac_id = name{v[1]};

    eosio::action(eosio::permission_level{cand, "active"_n}, code, "withdrawcand"_n, make_tuple(cand, dac_id)).send();
}

static void stake_req(name code, std::string paramstr, std::string delimiter) {
    vector<std::string> v;

    split(trim(paramstr), delimiter, v);

    auto owner    = name{v[0]};
    auto quantity = extended_asset(asset_from_string(v[1]), name{v[2]});

    eosio::action(eosio::permission_level{owner, "active"_n}, code, "stake"_n, make_tuple(owner, quantity)).send();
}

static void unstake_req(name code, std::string paramstr, std::string delimiter) {
    vector<std::string> v;

    split(trim(paramstr), delimiter, v);

    auto owner    = name{v[0]};
    auto quantity = extended_asset(asset_from_string(v[1]), name{v[2]});

    eosio::action(eosio::permission_level{owner, "active"_n}, code, "unstake"_n, make_tuple(owner, quantity)).send();
}

static void logrecord_req(name code, std::string paramstr, std::string delimiter) {
    vector<std::string> v;

    split(trim(paramstr), delimiter, v);

    auto account = name{v[0]};
    auto content = v[1];

    eosio::action(eosio::permission_level{account, "active"_n}, code, "logrecord"_n, make_tuple(account, content))
        .send();
}

static void cancelcontr_req(name code, std::string paramstr, std::string delimiter) {
    vector<std::string> v;

    split(trim(paramstr), delimiter, v);

    auto id       = name{v[0]};
    auto canceler = name{v[1]};

    eosio::action(eosio::permission_level{canceler, "active"_n}, code, "cancelcontr"_n, make_tuple(id, canceler))
        .send();
}
static void acceptcontr_req(name code, std::string paramstr, std::string delimiter) {
    vector<std::string> v;

    split(trim(paramstr), delimiter, v);

    auto id       = name{v[0]};
    auto accepter = name{v[1]};

    eosio::action(eosio::permission_level{accepter, "active"_n}, code, "acceptcontr"_n, make_tuple(id, accepter))
        .send();
}
static void approved_req(name code, std::string paramstr, std::string delimiter) {
    vector<std::string> v;

    split(trim(paramstr), delimiter, v);

    auto id       = name{v[0]};
    auto approver = name{v[1]};

    eosio::action(eosio::permission_level{approver, "active"_n}, code, "approved"_n, make_tuple(id, approver)).send();
}
static void pay_req(name code, std::string paramstr, std::string delimiter) {
    vector<std::string> v;

    split(trim(paramstr), delimiter, v);

    auto id      = name{v[0]};
    auto payer   = name{v[1]};
    auto payment = extended_asset(asset_from_string(v[2]), name{v[3]});
    eosio::action(eosio::permission_level{payer, "active"_n}, code, "pay"_n, make_tuple(id, payer, payment)).send();
}
SDK_DAPPSERVICE_BASE()

// protected:
CONTRACT_END()
