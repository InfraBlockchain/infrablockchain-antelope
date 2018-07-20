/**
 *  @copyright defined in LICENSE.txt
 */

#pragma once

#include <eosiolib/time.hpp>
#include <eosiolib/yx_kyc.hpp>
#include <eosiolib/yx_fee.hpp>
#include <string>

namespace yosemite {

    using namespace eosio;
    using std::string;
    using boost::container::flat_map;
    using boost::container::flat_set;

    /**
     * Hereby 'contract' of digital contract means the 'documented' and 'sign-required' contract on real-world business or peer-to-peer field.
     * But the parent class 'contract' means the smart contract of blockchain.
     */
    class digital_contract : public contract {
    public:
        explicit digital_contract(account_name self) : contract(self) {
        }

        void create(account_name creator, const string &dochash, const string &adddochash,
                    const vector<account_name> &signers, const time_point_sec &expiration, uint8_t options);
        void addsigners(account_name creator, uint64_t id, const vector<account_name> &signers);
        void sign(account_name creator, uint64_t id, account_name signer, const string &signerinfo);
        void upadddochash(account_name creator, uint64_t id, const string &adddochash);
        void remove(account_name creator, uint64_t id);
        void dump(account_name creator, uint64_t id);

    private:
        void check_signers_param(const vector<account_name> &signers) const;
    };

    /* scope = creator */
    struct dcontract_info {
        uint64_t id{}; /* generated sequence for each creator */
        vector<char> dochash;
        flat_set<account_name> signers;
        time_point_sec expiration{};
        vector<char> adddochash;
        uint8_t options{};
        flat_map<account_name, vector<char>> signatures{}; // key(account_name) is a signer

        uint64_t primary_key() const { return id; }
        bool is_signed() const { return !signatures.empty(); }
    };

    typedef eosio::multi_index<N(dcontract), dcontract_info> dcontract_index;

}
