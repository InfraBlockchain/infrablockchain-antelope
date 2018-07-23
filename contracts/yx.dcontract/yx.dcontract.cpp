/**
 *  @copyright defined in LICENSE.txt
 */

#include "yx.dcontract.hpp"
#include <yx.system/yx.system.hpp>
#include <yx.ntoken/yx.ntoken.hpp>
#include <eosiolib/yx_sequence_generator.hpp>

namespace yosemite {

    void digital_contract::check_signers_param(const vector<account_name> &signers) const {
        eosio_assert(static_cast<uint32_t>(!signers.empty()), "signers cannot be empty");
        eosio_assert(static_cast<uint32_t>(signers.size() <= 16), "too many signers");

        for (auto signer : signers) {
            eosio_assert(static_cast<uint32_t>(is_account(signer)), "signer account does not exist");
        }
    }

    void digital_contract::create(account_name creator, const string &dochash, const string &adddochash,
                                  const vector<account_name> &signers, const time_point_sec &expiration, uint8_t options) {
        eosio_assert(static_cast<uint32_t>(!dochash.empty()), "dochash cannot be empty");
        eosio_assert(static_cast<uint32_t>(dochash.size() <= 256), "dochash is too long");
        eosio_assert(static_cast<uint32_t>(adddochash.size() <= 256), "additional dochash is too long");
        check_signers_param(signers);
        eosio_assert(static_cast<uint32_t>(expiration > time_point_sec(now())), "digital contract is expired");

        require_auth(creator);

        charge_fee(creator, N(create));

        dcontract_index dcontract{get_self(), creator};
        uint64_t dcontract_id = sequence_generator::new_sequence(get_self(), creator);

        dcontract.emplace(get_self(), [&](auto &i) {
            for (auto signer : signers) {
                // check duplicates
                auto result = i.signers.insert(signer);
                eosio_assert(static_cast<uint32_t>(result.second), "duplicated signer account exists");
            }

            i.id = dcontract_id;
            std::copy(dochash.begin(), dochash.end(), std::back_inserter(i.dochash));
            std::copy(adddochash.begin(), adddochash.end(), std::back_inserter(i.adddochash));
            i.expiration = expiration;
            i.options = options;
        });
    }

    void digital_contract::addsigners(account_name creator, uint64_t id, const vector<account_name> &signers) {
        check_signers_param(signers);

        require_auth(creator);

        dcontract_index dcontract_idx{get_self(), creator};
        auto info = dcontract_idx.find(id);
        eosio_assert(static_cast<uint32_t>(info != dcontract_idx.end()), "digital contract does not exist");
        eosio_assert(static_cast<uint32_t>(!info->is_signed()), "digital contract is already signed by at least one signer");
        eosio_assert(static_cast<uint32_t>(info->expiration > time_point_sec(now())), "digital contract is expired");

        charge_fee(creator, N(addsigners));

        dcontract_idx.modify(info, 0, [&](auto &i) {
            for (auto signer : signers) {
                // check duplicates
                auto result = i.signers.insert(signer);
                eosio_assert(static_cast<uint32_t>(result.second), "duplicated signer account exists");
            }
        });
    }

    void digital_contract::sign(account_name creator, uint64_t id, account_name signer, const string &signerinfo) {
        eosio_assert(static_cast<uint32_t>(signerinfo.size() <= 256), "signerinfo is too long");
        eosio_assert(static_cast<uint32_t>(is_account(creator)), "creator account does not exist");

        dcontract_index dcontract_idx{get_self(), creator};
        auto info = dcontract_idx.find(id);
        eosio_assert(static_cast<uint32_t>(info != dcontract_idx.end()), "digital contract does not exist");
        eosio_assert(static_cast<uint32_t>(info->expiration > time_point_sec(now())), "digital contract is expired");
        eosio_assert(static_cast<uint32_t>(info->signers.find(signer) != info->signers.end()), "not designated signer");
        eosio_assert(static_cast<uint32_t>(info->signatures.find(signer) == info->signatures.end()),
                     "digital contract is already signed by the signer");

        charge_fee(creator, N(sign));

        require_auth(signer);
        dcontract_idx.modify(info, 0, [&](auto &i) {
            vector<char> vec;
            std::copy(signerinfo.begin(), signerinfo.end(), std::back_inserter(vec));
            i.signatures[signer] = vec;
            for (auto c : vec) {
                print(c);
            }
            print("\n");
        });
    }

    void digital_contract::upadddochash(account_name creator, uint64_t id, const string &adddochash) {
        eosio_assert(static_cast<uint32_t>(adddochash.size() <= 256), "adddochash is too long");

        require_auth(creator);

        dcontract_index dcontract_idx{get_self(), creator};
        auto info = dcontract_idx.find(id);
        eosio_assert(static_cast<uint32_t>(info != dcontract_idx.end()), "digital contract does not exist");

        charge_fee(creator, N(upadddochash));

        dcontract_idx.modify(info, 0, [&](auto &i) {
            i.adddochash.clear();
            std::copy(adddochash.begin(), adddochash.end(), std::back_inserter(i.adddochash));
        });
    }

    void digital_contract::remove(account_name creator, uint64_t id) {
        require_auth(creator);

        dcontract_index dcontract_idx{get_self(), creator};
        auto info = dcontract_idx.find(id);
        eosio_assert(static_cast<uint32_t>(info != dcontract_idx.end()), "digital contract does not exist");

        charge_fee(creator, N(remove));

        dcontract_idx.erase(info);
    }

    void digital_contract::dump(account_name creator, uint64_t id) {
        require_auth(get_self());
        eosio_assert(static_cast<uint32_t>(id > 0), "wrong digital contract id");

        dcontract_index dcontract_idx{get_self(), creator};
        auto info = dcontract_idx.find(id);
        eosio_assert(static_cast<uint32_t>(info != dcontract_idx.end()), "digital contract does not exist");

        for (auto c : info->dochash) {
            print(c);
        }
        print("\n");
        for (auto c : info->adddochash) {
            print(c);
        }
        print("\n");
        print("[signers]\n");
        for (auto signer : info->signers) {
            printn(signer);
            print(", ");
            auto itr = info->signatures.find(signer);
            if (itr == info->signatures.end()) {
                print("not signed");
            } else {
                for (auto c : itr->second) {
                    print(c);
                }
            }
            print("\n");
        }
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
                    (N(yx.ntoken), {payer, N(active)},
                     { payer, FEEDIST_ACCOUNT_NAME, yx_asset{fee_holder.fee, 0}, payer, "" });
        }
    }
}

EOSIO_ABI(yosemite::digital_contract, (create)(addsigners)(sign)(upadddochash)(remove)(dump))
