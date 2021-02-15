#ifndef DACCUSTODIAN_SHARED_H
#define DACCUSTODIAN_SHARED_H

namespace dac {

#include <eosio/eosio.hpp>
#include <eosio/multi_index.hpp>

    struct [[eosio::table("custodians"), eosio::contract("daccustodian")]] custodian {
        eosio::name  cust_name;
        eosio::asset requestedpay;
        uint64_t     total_votes;

        uint64_t primary_key() const { return cust_name.value; }

        uint64_t by_votes_rank() const { return static_cast<uint64_t>(UINT64_MAX - total_votes); }

        uint64_t by_requested_pay() const { return static_cast<uint64_t>(requestedpay.amount); }
    };

    typedef eosio::multi_index<"custodians"_n, custodian,
        eosio::indexed_by<"byvotesrank"_n, eosio::const_mem_fun<custodian, uint64_t, &custodian::by_votes_rank>>,
        eosio::indexed_by<"byreqpay"_n, eosio::const_mem_fun<custodian, uint64_t, &custodian::by_requested_pay>>>
        custodians_table;

    struct [[eosio::table("candidates"), eosio::contract("daccustodian")]] candidate {
        eosio::name           candidate_name;
        eosio::asset          requestedpay;
        eosio::asset          locked_tokens;
        uint64_t              total_votes;
        uint8_t               is_active;
        eosio::time_point_sec custodian_end_time_stamp;

        uint64_t primary_key() const { return candidate_name.value; }
        uint64_t by_number_votes() const { return static_cast<uint64_t>(total_votes); }
        uint64_t by_votes_rank() const { return static_cast<uint64_t>(UINT64_MAX - total_votes); }
        uint64_t by_requested_pay() const { return static_cast<uint64_t>(requestedpay.amount); }
    };

    typedef eosio::multi_index<"candidates"_n, candidate,
        eosio::indexed_by<"bycandidate"_n, eosio::const_mem_fun<candidate, uint64_t, &candidate::primary_key>>,
        eosio::indexed_by<"byvotes"_n, eosio::const_mem_fun<candidate, uint64_t, &candidate::by_number_votes>>,
        eosio::indexed_by<"byvotesrank"_n, eosio::const_mem_fun<candidate, uint64_t, &candidate::by_votes_rank>>,
        eosio::indexed_by<"byreqpay"_n, eosio::const_mem_fun<candidate, uint64_t, &candidate::by_requested_pay>>>
        candidates_table;

    struct [[eosio::table]] vote_weight {
        eosio::name voter;
        uint64_t    weight;

        uint64_t primary_key() const { return voter.value; }
    };
    typedef eosio::multi_index<"weights"_n, vote_weight> weights;

    struct contr_config {
        //    The amount of assets that are locked up by each candidate applying for election.
        eosio::extended_asset lockupasset;
        //    The maximum number of votes that each member can make for a candidate.
        uint8_t maxvotes = 5;
        //    Number of custodians to be elected for each election count.
        uint8_t numelected = 3;
        //    Length of a period in seconds.
        //     - used for pay calculations if an eary election is called and to trigger deferred `newperiod` calls.
        uint32_t periodlength = 7 * 24 * 60 * 60;

        // The contract will direct all payments via the service provider.
        bool should_pay_via_service_provider;

        // Amount of token value in votes required to trigger the initial set of custodians
        uint32_t initial_vote_quorum_percent;

        // Amount of token value in votes required to trigger the allow a new set of custodians to be set after the
        // initial threshold has been achieved.
        uint32_t vote_quorum_percent;

        // required number of custodians required to approve different levels of authenticated actions.
        uint8_t auth_threshold_high;
        uint8_t auth_threshold_mid;
        uint8_t auth_threshold_low;

        // The time before locked up stake can be released back to the candidate using the unstake action
        uint32_t lockup_release_time_delay;

        eosio::extended_asset requested_pay_max;
    };

} // namespace dac

#endif