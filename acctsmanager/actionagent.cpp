#include <algorithm>
ACTION acctsmanager::xupdateauth(name account, name permission, name parent, authority auth) {
    require_auth(this->get_self());

    action(permission_level{account, "owner"_n}, "eosio"_n, "updateauth"_n,
        std::make_tuple(account, permission, parent, auth))
        .send();
}
ACTION acctsmanager::xlinkauth(name account, name code, name type, name requirement) {
    require_auth(this->get_self());

    action(permission_level{account, "active"_n}, "eosio"_n, "linkauth"_n,
        std::make_tuple(account, code, type, requirement))
        .send();
}

ACTION acctsmanager::treasuryxfer(name from, name to, extended_asset fo_quantity, std::string memo) {
    //   require_auth(permission_level{get_self(), "manager"_n});
    // require_auth(get_self());

    action(permission_level{from, "xfer"_n}, fo_quantity.contract, "transfer"_n,
        make_tuple(from, to, fo_quantity.quantity, memo))
        .send();
}

ACTION acctsmanager::linkdacauth(name auth_acct, name trea_acct, uint32_t trans_sec) {
    require_auth(this->get_self());
    check(trans_sec < 3600, "trans_sec must less then 1 hour.");

    // create account
    // TODO: cat't new account in the same action,do it in eosjs
    // action(permission_level{get_self(), "newacct"_n}, get_self(), "newacct"_n, std::make_tuple(auth_acct)).send();
    // action(permission_level{get_self(), "newacct"_n}, get_self(), "newacct"_n, std::make_tuple(trea_acct)).send();

    // set auth_acct permissions
    const permission_level_weight code{.permission = permission_level{get_self(), "eosio.code"_n}, .weight = 1};
    const permission_level_weight high{.permission = permission_level{auth_acct, HIGH_PERMISSION}, .weight = 1};
    const permission_level_weight cust_code{
        .permission = permission_level{name(DACCUST_CONTRACT), "eosio.code"_n}, .weight = 1};

    authority auth_active_authority{.threshold = 1, .keys = {}, .accounts = {high}};

    authority owner_authority{.threshold = 1, .keys = {}, .accounts = {cust_code}};

    // TODO: cat't new authority in the same action,do it in eosjs
    /*
        authority manager_authority{.threshold = 1, .keys = {}, .accounts = {code}};

        action(permission_level{auth_acct, "owner"_n}, "eosio"_n, "updateauth"_n,
            std::make_tuple(auth_acct, HIGH_PERMISSION, "active"_n, manager_authority))
            .send();
        action(permission_level{auth_acct, "owner"_n}, "eosio"_n, "updateauth"_n,
            std::make_tuple(auth_acct, MEDIUM_PERMISSION, HIGH_PERMISSION, manager_authority))
            .send();
        action(permission_level{auth_acct, "owner"_n}, "eosio"_n, "updateauth"_n,
            std::make_tuple(auth_acct, LOW_PERMISSION, MEDIUM_PERMISSION, manager_authority))
            .send();
        action(permission_level{auth_acct, "owner"_n}, "eosio"_n, "updateauth"_n,
            std::make_tuple(auth_acct, ONE_PERMISSION, LOW_PERMISSION, manager_authority))
            .send();
        action(permission_level{auth_acct, "owner"_n}, "eosio"_n, "updateauth"_n,
            std::make_tuple(auth_acct, ADMIN_PERMISSION, ONE_PERMISSION, manager_authority))
            .send();
    */
    action(permission_level{auth_acct, "owner"_n}, "eosio"_n, "linkauth"_n,
        std::make_tuple(auth_acct, name(DACCUST_CONTRACT), name(""), HIGH_PERMISSION))
        .send();
    action(permission_level{auth_acct, "owner"_n}, "eosio"_n, "linkauth"_n,
        std::make_tuple(auth_acct, name(DACCUST_CONTRACT), name("firecand"), MEDIUM_PERMISSION))
        .send();
    action(permission_level{auth_acct, "owner"_n}, "eosio"_n, "linkauth"_n,
        std::make_tuple(auth_acct, name(DACCUST_CONTRACT), name("firecust"), HIGH_PERMISSION))
        .send();
    action(permission_level{auth_acct, "owner"_n}, "eosio"_n, "linkauth"_n,
        std::make_tuple(auth_acct, name(DACPROP_CONTRACT), name(""), ONE_PERMISSION))
        .send();
    action(permission_level{auth_acct, "owner"_n}, "eosio"_n, "linkauth"_n,
        std::make_tuple(auth_acct, name(DACDIR_CONTRACT), name("regref"), LOW_PERMISSION))
        .send();
    action(permission_level{auth_acct, "owner"_n}, "eosio"_n, "linkauth"_n,
        std::make_tuple(auth_acct, name(DACMSIG_CONTRACT), name(""), ADMIN_PERMISSION))
        .send();

    action(permission_level{auth_acct, "owner"_n}, "eosio"_n, "updateauth"_n,
        std::make_tuple(auth_acct, "active"_n, "owner"_n, auth_active_authority))
        .send();

    action(permission_level{auth_acct, "owner"_n}, "eosio"_n, "updateauth"_n,
        std::make_tuple(auth_acct, "owner"_n, name(""), owner_authority))
        .send();

    // set trea_acct permissions

    // TODO: cat't new authority in the same action,do it in eosjs
    /*
        const permission_level_weight auth_active{.permission = permission_level{auth_acct, "active"_n}, .weight = 1};
        const permission_level_weight prop_code{
            .permission = permission_level{name(DACPROP_CONTRACT), "eosio.code"_n}, .weight = 1};
        const permission_level_weight auth_med_2{.permission = permission_level{auth_acct, MEDIUM_PERMISSION}, .weight =
       2}; const wait_weight             wait = wait_weight{.wait_sec = trans_sec, .weight = 1};

        std::vector<permission_level_weight> xfer_accounts = {prop_code, auth_med_2, cust_code};

        std::sort(xfer_accounts.begin(), xfer_accounts.end(), [](permission_level_weight a, permission_level_weight b) {
            return (a.permission.actor.to_string() < b.permission.actor.to_string());
        });
        authority prop_code_authority{.threshold = 1, .keys = {}, .accounts = {prop_code}};
        authority xfer_authority{.threshold = 2, .keys = {}, .accounts = xfer_accounts, .waits = {wait}};

        action(permission_level{trea_acct, "owner"_n}, "eosio"_n, "updateauth"_n,
            std::make_tuple(trea_acct, "escrow"_n, "active"_n, prop_code_authority))
            .send();

        action(permission_level{trea_acct, "owner"_n}, "eosio"_n, "updateauth"_n,
            std::make_tuple(trea_acct, "xfer"_n, "escrow"_n, xfer_authority))
            .send();
    */

    // linkauth
    action(permission_level{trea_acct, "owner"_n}, "eosio"_n, "linkauth"_n,
        std::make_tuple(trea_acct, name(DACESCROW_CONTRACT), name("approve"), "escrow"_n))
        .send();
    action(permission_level{trea_acct, "owner"_n}, "eosio"_n, "linkauth"_n,
        std::make_tuple(trea_acct, name(DACESCROW_CONTRACT), name("init"), "escrow"_n))
        .send();
    action(permission_level{trea_acct, "owner"_n}, "eosio"_n, "linkauth"_n,
        std::make_tuple(trea_acct, name(DACTOKEN_CONTRACT), name("transfer"), "xfer"_n))
        .send();
    action(permission_level{trea_acct, "owner"_n}, "eosio"_n, "linkauth"_n,
        std::make_tuple(trea_acct, name(SYSTOKEN_CONTRACT), name("transfer"), "xfer"_n))
        .send();
    action(permission_level{trea_acct, "owner"_n}, "eosio"_n, "linkauth"_n,
        std::make_tuple(trea_acct, this->get_self(), name("treasuryxfer"), "xfer"_n))
        .send();
    action(permission_level{trea_acct, "owner"_n}, "eosio"_n, "updateauth"_n,
        std::make_tuple(trea_acct, "active"_n, "owner"_n, auth_active_authority))
        .send();

    action(permission_level{trea_acct, "owner"_n}, "eosio"_n, "updateauth"_n,
        std::make_tuple(trea_acct, "owner"_n, name(""), owner_authority))
        .send();
}

ACTION acctsmanager::callaction(name code, std::string func_name, std::string paramstr, std::string delimiter) {
    call_action_item_t call_action_items[] = {{"memberreg", memberreg_req}, {"memberunreg", memberunreg_req},
        {"nominatecand", nominatecand_req}, {"withdrawcand", withdrawcand_req}, {"stake", stake_req},
        {"unstake", unstake_req}, {"logrecord", logrecord_req},
        {"pay",pay_req},{"cancelcontr",cancelcontr_req},
        {"acceptcontr",acceptcontr_req},{"approved",approved_req}};

    int item_num = sizeof(call_action_items) / sizeof(struct call_action_item);
    int item_idx = 0;
    for (; item_idx < item_num; item_idx++) {
        if (func_name == call_action_items[item_idx].func_name) {
            call_action_items[item_idx].call_action_func(code, paramstr, delimiter);
            return;
        }
    }

    check(false, "ERR::ACTION_NOT_FOUND::Unknown action");
    return;
}

// eosio.msig
ACTION acctsmanager::propose(
    name proposer, name proposal_name, std::vector<permission_level> requested, transaction trx) {
    require_auth(this->get_self());

    action(permission_level{proposer, "active"_n}, name(MSIG_CONTRACT), "propose"_n,
        std::make_tuple(proposer, proposal_name, requested, trx))
        .send();
}

ACTION acctsmanager::approve(name proposer, name proposal_name, permission_level level,
    const eosio::binary_extension<eosio::checksum256> &proposal_hash) {
    // require_auth(this->get_self());

    if (!proposal_hash) {
        action(level, name(MSIG_CONTRACT), "approve"_n, make_tuple(proposer, proposal_name, level)).send();
    } else {
        action(level, name(MSIG_CONTRACT), "approve"_n, make_tuple(proposer, proposal_name, level, proposal_hash))
            .send();
    }
}

ACTION acctsmanager::unapprove(name proposer, name proposal_name, permission_level level) {
    // require_auth(this->get_self());

    action(level, name(MSIG_CONTRACT), "unapprove"_n, make_tuple(proposer, proposal_name, level)).send();
}

ACTION acctsmanager::cancel(name proposer, name proposal_name, name canceler) {
    // require_auth(this->get_self());
    action(permission_level{canceler, "active"_n}, name(MSIG_CONTRACT), "cancel"_n,
        make_tuple(proposer, proposal_name, canceler))
        .send();
}

ACTION acctsmanager::exec(name proposer, name proposal_name, name executer) {
    // require_auth(this->get_self());
    action(permission_level{executer, "active"_n}, name(MSIG_CONTRACT), "exec"_n,
        make_tuple(proposer, proposal_name, executer))
        .send();
}

// dacmultisigs
ACTION acctsmanager::proposed(name proposer, name proposal_name, string metadata, name dac_id) {
    require_auth(this->get_self());

    dacdir::dac dac           = dacdir::dac_for_id(dac_id);
    name        auth_account  = dac.account_for_type(dacdir::AUTH);
    auto        msig_contract = dac.account_for_type(dacdir::MSIGS);
    auto        auth1         = permission_level{proposer, "active"_n};
    auto        auth2         = permission_level{auth_account, "one"_n};

    std::vector<permission_level> auths = {auth1, auth2};
    action(auths, msig_contract, "proposed"_n, make_tuple(proposer, proposal_name, metadata, dac_id)).send();
}
ACTION acctsmanager::approved(name proposer, name proposal_name, name approver, name dac_id) {
    // require_auth(this->get_self());

    dacdir::dac dac           = dacdir::dac_for_id(dac_id);
    name        auth_account  = dac.account_for_type(dacdir::AUTH);
    auto        msig_contract = dac.account_for_type(dacdir::MSIGS);
    auto        auth1         = permission_level{approver, "active"_n};
    auto        auth2         = permission_level{auth_account, "one"_n};

    std::vector<permission_level> auths = {auth1, auth2};
    action(auths, msig_contract, "approved"_n, make_tuple(proposer, proposal_name, approver, dac_id)).send();
}
ACTION acctsmanager::unapproved(name proposer, name proposal_name, name unapprover, name dac_id) {
    // require_auth(this->get_self());

    dacdir::dac dac           = dacdir::dac_for_id(dac_id);
    name        auth_account  = dac.account_for_type(dacdir::AUTH);
    auto        msig_contract = dac.account_for_type(dacdir::MSIGS);
    auto        auth1         = permission_level{unapprover, "active"_n};
    auto        auth2         = permission_level{auth_account, "one"_n};

    std::vector<permission_level> auths = {auth1, auth2};
    action(auths, msig_contract, "unapproved"_n, make_tuple(proposer, proposal_name, unapprover, dac_id)).send();
}

ACTION acctsmanager::deny(name proposer, name proposal_name, name denyer, name dac_id) {
    // require_auth(this->get_self());

    dacdir::dac dac           = dacdir::dac_for_id(dac_id);
    name        auth_account  = dac.account_for_type(dacdir::AUTH);
    auto        msig_contract = dac.account_for_type(dacdir::MSIGS);
    auto        auth1         = permission_level{denyer, "active"_n};
    auto        auth2         = permission_level{auth_account, "one"_n};

    std::vector<permission_level> auths = {auth1, auth2};
    action(auths, msig_contract, "deny"_n, make_tuple(proposer, proposal_name, denyer, dac_id)).send();
}

ACTION acctsmanager::cancelled(name proposer, name proposal_name, name canceler, name dac_id) {
    // require_auth(this->get_self());
    dacdir::dac dac           = dacdir::dac_for_id(dac_id);
    name        auth_account  = dac.account_for_type(dacdir::AUTH);
    auto        msig_contract = dac.account_for_type(dacdir::MSIGS);
    auto        auth1         = permission_level{canceler, "active"_n};
    auto        auth2         = permission_level{auth_account, "one"_n};

    std::vector<permission_level> auths = {auth1, auth2};
    action(auths, msig_contract, "cancelled"_n, make_tuple(proposer, proposal_name, canceler, dac_id)).send();
}
ACTION acctsmanager::executed(name proposer, name proposal_name, name executer, name dac_id) {
    // require_auth(this->get_self());

    dacdir::dac dac           = dacdir::dac_for_id(dac_id);
    name        auth_account  = dac.account_for_type(dacdir::AUTH);
    auto        msig_contract = dac.account_for_type(dacdir::MSIGS);
    auto        auth1         = permission_level{executer, "active"_n};
    auto        auth2         = permission_level{auth_account, "one"_n};

    std::vector<permission_level> auths = {auth1, auth2};
    action(auths, msig_contract, "executed"_n, make_tuple(proposer, proposal_name, executer, dac_id)).send();
}

// dacdirectory
ACTION acctsmanager::regdac(eosio::name auth_acct, eosio::name trea_acct, name dac_id, extended_symbol dac_symbol,
    string title, map<uint8_t, string> refs, map<uint8_t, eosio::name> accounts) {
    require_auth(this->get_self());

    auto auth1 = permission_level{auth_acct, "active"_n};
    auto auth2 = permission_level{trea_acct, "active"_n};

    std::vector<permission_level> auths = {auth1, auth2};
    action(auths, "dacdirectory"_n, "regdac"_n, make_tuple(auth_acct, dac_id, dac_symbol, title, refs, accounts))
        .send();
}

// dacdactokens
ACTION acctsmanager::tokencreate(name issuer, asset maximum_supply) {
    require_auth(this->get_self());
    action(permission_level{name(DACTOKEN_CONTRACT), "create"_n}, name(DACTOKEN_CONTRACT), "create"_n,
        std::make_tuple(issuer, maximum_supply))
        .send();
}

ACTION acctsmanager::tokenissue(name to, asset quantity, string memo) {
    require_auth(this->get_self());

    action(permission_level{to, "active"_n}, name(DACTOKEN_CONTRACT), "issue"_n, std::make_tuple(to, quantity, memo))
        .send();
}

ACTION acctsmanager::tokenburn(name from, asset quantity) {
    require_auth(this->get_self());
    action(permission_level{from, "active"_n}, name(DACTOKEN_CONTRACT), "burn"_n, std::make_tuple(from, quantity))
        .send();
}

ACTION acctsmanager::newmemterms(string terms, string hash, name dac_id) {
    require_auth(this->get_self());

    dacdir::dac dac          = dacdir::dac_for_id(dac_id);
    eosio::name auth_account = dac.account_for_type(dacdir::AUTH);

    action(permission_level{auth_account, "active"_n}, dac.symbol.get_contract(), "newmemterms"_n,
        make_tuple(terms, hash, dac_id))
        .send();
}
// custodian
ACTION acctsmanager::votecust(name voter, std::vector<name> newvotes, name dac_id) {
    require_auth(this->get_self());

    dacdir::dac dac                = dacdir::dac_for_id(dac_id);
    eosio::name custodian_contract = dac.account_for_type(dacdir::CUSTODIAN);
    action(permission_level{voter, "active"_n}, custodian_contract, "votecust"_n, make_tuple(voter, newvotes, dac_id))
        .send();
}

ACTION acctsmanager::appointcust(vector<name> cust, name dac_id) {
    require_auth(this->get_self());

    dacdir::dac dac                = dacdir::dac_for_id(dac_id);
    eosio::name auth_account       = dac.account_for_type(dacdir::AUTH);
    eosio::name custodian_contract = dac.account_for_type(dacdir::CUSTODIAN);

    action(permission_level{auth_account, "active"_n}, custodian_contract, "appointcust"_n, make_tuple(cust, dac_id))
        .send();
}
ACTION acctsmanager::updateconfig(contr_config new_config, name dac_id) {
    require_auth(this->get_self());

    dacdir::dac dac                = dacdir::dac_for_id(dac_id);
    eosio::name auth_account       = dac.account_for_type(dacdir::AUTH);
    eosio::name custodian_contract = dac.account_for_type(dacdir::CUSTODIAN);

    action(permission_level{auth_account, "active"_n}, custodian_contract, "updateconfig"_n,
        make_tuple(new_config, dac_id))
        .send();
}

// exchange
ACTION acctsmanager::inittoken(name user, symbol new_symbol, extended_asset initial_pool1, extended_asset initial_pool2,
    int initial_fee, name fee_contract) {
    require_auth(this->get_self());
    action(permission_level{user, "active"_n}, name(DEX_CONTRACT), "inittoken"_n,
        make_tuple(user, new_symbol, initial_pool1, initial_pool2, initial_fee, fee_contract))
        .send();
}

ACTION acctsmanager::openext(name user, const name &payer, const extended_symbol &ext_symbol) {
    require_auth(this->get_self());
    action(permission_level{user, "active"_n}, name(DEX_CONTRACT), "openext"_n, make_tuple(user, payer, ext_symbol))
        .send();
}
ACTION acctsmanager::closeext(const name &user, const name &to, const extended_symbol &ext_symbol, string memo) {
    require_auth(this->get_self());
    action(permission_level{user, "active"_n}, name(DEX_CONTRACT), "closeext"_n, make_tuple(user, to, ext_symbol, memo))
        .send();
}

ACTION acctsmanager::withdraw(name user, name to, extended_asset to_withdraw, string memo) {
    require_auth(this->get_self());
    action(
        permission_level{user, "active"_n}, name(DEX_CONTRACT), "withdraw"_n, make_tuple(user, to, to_withdraw, memo))
        .send();
}
ACTION acctsmanager::addliquidity(name user, asset to_buy, asset max_asset1, asset max_asset2) {
    require_auth(this->get_self());
    action(permission_level{user, "active"_n}, name(DEX_CONTRACT), "addliquidity"_n,
        make_tuple(user, to_buy, max_asset1, max_asset2))
        .send();
}
ACTION acctsmanager::remliquidity(name user, asset to_sell, asset min_asset1, asset min_asset2) {
    require_auth(this->get_self());
    action(permission_level{user, "active"_n}, name(DEX_CONTRACT), "remliquidity"_n,
        make_tuple(user, to_sell, min_asset1, min_asset2))
        .send();
}
ACTION acctsmanager::exchange(name user, symbol_code pair_token, extended_asset ext_asset_in, asset min_expected) {
    require_auth(this->get_self());
    action(permission_level{user, "active"_n}, name(DEX_CONTRACT), "exchange"_n,
        make_tuple(user, pair_token, ext_asset_in, min_expected))
        .send();
}
ACTION acctsmanager::exchangev2(
    name user, symbol_code pair_token1, symbol_code pair_token2, extended_asset ext_asset_in, asset min_expected) {
    require_auth(this->get_self());
    action(permission_level{user, "active"_n}, name(DEX_CONTRACT), "exchangev2"_n,
        make_tuple(user, pair_token1, pair_token2, ext_asset_in, min_expected))
        .send();
}
ACTION acctsmanager::close(const name &owner, const symbol &symbol) {
    require_auth(this->get_self());
    action(permission_level{owner, "active"_n}, name(DEX_CONTRACT), "close"_n, make_tuple(owner, symbol)).send();
}

// makecontract
ACTION acctsmanager::createcontr(name id, name party_a, name party_b, string title, string summary, string content_hash,
    extended_asset contract_pay) {
    require_auth(this->get_self());
    action(permission_level{party_a, "active"_n}, name("makecontract"), "createcontr"_n,
        make_tuple(id, party_a, party_b, title, summary, content_hash,contract_pay))
        .send();
}