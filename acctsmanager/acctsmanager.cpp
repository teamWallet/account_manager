#include "acctsmanager.hpp"
#include "actionagent.cpp"
ACTION acctsmanager::init() {
    require_auth(get_self());

    config_table cfg(get_self(), get_self().value);
    if (!cfg.exists()) {
        cfg.set(config{}, get_self());
    }

    stat_table st(get_self(), get_self().value);
    if (!st.exists()) {
        st.set(stat{}, get_self());
    }
}

ACTION acctsmanager::newacct(const name &acct) {
    require_auth(permission_level{get_self(), "newacct"_n});

    config_table cfg(get_self(), get_self().value);

    auto config = cfg.get();

    stat_table stat(get_self(), get_self().value);
    auto       st = stat.get();

    const permission_level_weight accounts{.permission = permission_level{get_self(), "eosio.code"_n}, .weight = 1};

    const authority auth{.threshold = 1, .keys = {}, .accounts = {accounts}, .waits = {}};

    create_account(acct, auth, auth, config.rambytes, config.stake_cpu, config.stake_net);

    st.num_created++;
    stat.set(st, get_self());
}

ACTION acctsmanager::regainowner(const name &acct, const authority &auth) {
    require_auth(get_self());

    config_table cfg(get_self(), get_self().value);
    auto         config = cfg.get();

    check(config.expiration.sec_since_epoch() <= time_point_sec(current_time_point()).sec_since_epoch(),
        "The current time   must be after the expiration");

    // Changes contract owner permission
    action(permission_level{acct, "owner"_n}, "eosio"_n, "updateauth"_n, make_tuple(acct, "owner"_n, ""_n, auth))
        .send();

    const permission_level_weight code{.permission = permission_level{get_self(), "eosio.code"_n}, .weight = 1};

    auto it = std::find_if(
        auth.accounts.begin(), auth.accounts.end(), [&cp = code](const permission_level_weight &p) -> bool {
            return cp.weight == p.weight && cp.permission == p.permission;
        });

    if (it == auth.accounts.end()) {
        stat_table stat(get_self(), get_self().value);
        auto       st = stat.get();
        st.num_created--;
        stat.set(st, get_self());
    }
}

ACTION acctsmanager::xtransfer(name from, name to, extended_asset fo_quantity, std::string memo) {
    //   require_auth(permission_level{get_self(), "manager"_n});
    require_auth(get_self());

    action(permission_level{from, "active"_n}, fo_quantity.contract, "transfer"_n,
        make_tuple(from, to, fo_quantity.quantity, memo))
        .send();
}

ACTION acctsmanager::setconfig(const config &cfg) {
    require_auth(get_self());

    config_table config(get_self(), get_self().value);
    config.set(cfg, get_self());
}

ACTION acctsmanager::deleteconfig() {
    require_auth(get_self());

    config_table configs(get_self(), get_self().value);
    configs.remove();
}

ACTION acctsmanager::extend(uint32_t extend_seconds) {
    require_auth(get_self());
    config_table cfg(get_self(), get_self().value);
    auto         config = cfg.get();

    config.expiration += time_point_sec(extend_seconds);
    check(config.expiration.sec_since_epoch() > time_point_sec(current_time_point()).sec_since_epoch(),
        "The new expiration must be after the current time");

    cfg.set(config, get_self());
}

// https://github.com/NESTchain/eos_afl_fuzzy/blob/686f0deb5dac097cc292f735ccb47c238e763de0/unittests/test-contracts/proxy/proxy.cpp
// void acctsmanager::on_error(uint128_t sender_id,
//                           eosio::ignore<std::vector<char>>) {
//   print("on_error called on ", get_self(),
//         " contract with sender_id = ", sender_id, "\n");
//   check(_config.exists(), "Attempting use of unconfigured proxy");

//   auto cfg = _config.get();

//   auto id = cfg.next_id;
//   ++cfg.next_id;
//   _config.set(cfg, same_payer);

//   print("Resending Transaction: ", sender_id, " as ", id, "\n");

//   unsigned_int packed_trx_size;
//   get_datastream() >> packed_trx_size;
//   transaction trx;
//   get_datastream() >> trx;

//   trx.delay_sec = cfg.delay;
//   trx.send(id, get_self());
// }