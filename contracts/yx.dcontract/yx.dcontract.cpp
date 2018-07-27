/**
 *  @copyright defined in LICENSE.txt
 */

#include "yx.dcontract.hpp"
#include <yx.system/yx.system.hpp>
#include <yx.ntoken/yx.ntoken.hpp>
#include <eosiolib/yx_sequence_generator.hpp>

namespace yosemite {

    void digital_contract::check_signers_param(const vector <account_name> &signers,
                                               flat_set <account_name> &duplicates) {
        eosio_assert(static_cast<uint32_t>(!signers.empty()), "signers cannot be empty");
        eosio_assert(static_cast<uint32_t>(signers.size() <= 128), "too many signers");

        for (auto signer : signers) {
            eosio_assert(static_cast<uint32_t>(is_account(signer)), "signer account does not exist");
            auto result = duplicates.insert(signer);
            eosio_assert(static_cast<uint32_t>(result.second), "duplicated signer account exists");
        }
    }

    void digital_contract::create(account_name creator, const string &dochash, const string &adddochash,
                                  const vector<account_name> &signers, const time_point_sec &expiration, uint8_t options) {
        eosio_assert(static_cast<uint32_t>(!dochash.empty()), "dochash cannot be empty");
        eosio_assert(static_cast<uint32_t>(dochash.size() <= 256), "dochash is too long");
        eosio_assert(static_cast<uint32_t>(adddochash.size() <= 256), "additional dochash is too long");
        eosio_assert(static_cast<uint32_t>(expiration > time_point_sec(now())), "digital contract is expired");
        flat_set<account_name> empty_duplicates;
        check_signers_param(signers, empty_duplicates);

        require_auth(creator);

        charge_fee(creator, N(create));

        dcontract_index dcontract{get_self(), creator};
        uint64_t dcontract_seq = sequence_generator::new_sequence(get_self(), creator);

        dcontract.emplace(get_self(), [&](auto &i) {
            i.sequence = dcontract_seq;
            std::copy(dochash.begin(), dochash.end(), std::back_inserter(i.dochash));
            std::copy(adddochash.begin(), adddochash.end(), std::back_inserter(i.adddochash));
            std::copy(signers.begin(), signers.end(), std::back_inserter(i.signers));
            i.expiration = expiration;
            i.options = options;
        });
    }

    void digital_contract::addsigners(dcid dc_id, const vector <account_name> &signers) {
        require_auth(dc_id.creator);

        dcontract_index dcontract_idx{get_self(), dc_id.creator};
        auto info = dcontract_idx.find(dc_id.sequence);
        eosio_assert(static_cast<uint32_t>(info != dcontract_idx.end()), "digital contract does not exist");
        eosio_assert(static_cast<uint32_t>(!info->is_signed()), "digital contract is already signed by at least one signer");
        eosio_assert(static_cast<uint32_t>(info->expiration > time_point_sec(now())), "digital contract is expired");
        // check duplicated signer
        flat_set<account_name> duplicates{info->signers.begin(), info->signers.end()};
        check_signers_param(signers, duplicates);

        charge_fee(dc_id.creator, N(addsigners));

        dcontract_idx.modify(info, 0, [&](auto &i) {
            std::copy(signers.begin(), signers.end(), std::back_inserter(i.signers));
        });
    }

    void digital_contract::sign(dcid dc_id, account_name signer, const string &signerinfo) {
        eosio_assert(static_cast<uint32_t>(signerinfo.size() <= 256), "signerinfo is too long");
        eosio_assert(static_cast<uint32_t>(is_account(dc_id.creator)), "creator account does not exist");

        require_auth(signer);

        dcontract_index dcontract_idx{get_self(), dc_id.creator};
        auto info = dcontract_idx.find(dc_id.sequence);
        eosio_assert(static_cast<uint32_t>(info != dcontract_idx.end()), "digital contract does not exist");
        eosio_assert(static_cast<uint32_t>(info->expiration > time_point_sec(now())), "digital contract is expired");
        const auto &itr = std::find(info->signers.begin(), info->signers.end(), signer);
        eosio_assert(static_cast<uint32_t>(itr != info->signers.end()),
                     "not designated signer");
        auto index = std::distance(info->signers.begin(), itr);
        eosio_assert(static_cast<uint32_t>(std::find(info->done_signers.begin(), info->done_signers.end(), index) == info->done_signers.end()),
                     "digital contract is already signed by the signer");

        charge_fee(dc_id.creator, N(sign));

        dcontract_idx.modify(info, 0, [&](auto &i) {
            i.done_signers.push_back(static_cast<uint8_t>(index));
        });

        dcontract_signer_index dc_signer_index{get_self(), signer};
        dc_signer_index.emplace(get_self(), [&](auto &i) {
            i.id = dc_signer_index.available_primary_key();
            i.dc_id_s = dc_id.to_uint128();
            std::copy(signerinfo.begin(), signerinfo.end(), std::back_inserter(i.signerinfo));
        });
    }

    void digital_contract::upadddochash(dcid dc_id, const string &adddochash) {
        eosio_assert(static_cast<uint32_t>(adddochash.size() <= 256), "adddochash is too long");

        require_auth(dc_id.creator);

        dcontract_index dcontract_idx{get_self(), dc_id.creator};
        auto info = dcontract_idx.find(dc_id.sequence);
        eosio_assert(static_cast<uint32_t>(info != dcontract_idx.end()), "digital contract does not exist");

        charge_fee(dc_id.creator, N(upadddochash));

        dcontract_idx.modify(info, 0, [&](auto &i) {
            i.adddochash.clear();
            std::copy(adddochash.begin(), adddochash.end(), std::back_inserter(i.adddochash));
        });
    }

    void digital_contract::remove(dcid dc_id) {
        require_auth(dc_id.creator);

        dcontract_index dcontract_idx{get_self(), dc_id.creator};
        auto info = dcontract_idx.find(dc_id.sequence);
        eosio_assert(static_cast<uint32_t>(info != dcontract_idx.end()), "digital contract does not exist");

        charge_fee(dc_id.creator, N(remove));

        dcontract_idx.erase(info);
    }

    bool digital_contract::check_fee_operation(const uint64_t &operation_name) {
        return operation_name == N(create) ||
               operation_name == N(addsigners) ||
               operation_name == N(sign) ||
               operation_name == N(upadddochash) ||
               operation_name == N(remove);
    }

    void digital_contract::charge_fee(const account_name &payer, uint64_t operation) {
        fees fee_table(get_self(), get_self());
        const auto &fee_holder = fee_table.get(operation, "fee is not set or unknown operation");

        if (fee_holder.fee.amount > 0) {
            INLINE_ACTION_SENDER(yosemite::ntoken, transfer)
                    (N(yx.ntoken), {{payer, N(active)}, {yosemitesys::YOSEMITE_SYSTEM_ACCOUNT_NAME, N(active)}},
                     { payer, FEEDIST_ACCOUNT_NAME, {fee_holder.fee, 0}, payer, "" });
        }
    }
}

EOSIO_ABI(yosemite::digital_contract, (create)(addsigners)(sign)(upadddochash)(remove)(setfee)
)
