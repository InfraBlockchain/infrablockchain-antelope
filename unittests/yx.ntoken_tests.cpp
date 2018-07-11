#include <boost/test/unit_test.hpp>
#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>

#include <yx.ntoken/yx.ntoken.wast.hpp>
#include <yx.ntoken/yx.ntoken.abi.hpp>

#include <Runtime/Runtime.h>

#include <fc/variant_object.hpp>

using namespace eosio::testing;
using namespace eosio;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;
using namespace std;

using mvo = fc::mutable_variant_object;

class yx_ntoken_tester : public tester {
public:

    yx_ntoken_tester() {
        produce_blocks(2);

        create_accounts({N(d1), N(user1), N(user2), N(yx.ntoken)});
        produce_blocks(2);

        set_code(N(yx.ntoken), yx_ntoken_wast);
        set_abi(N(yx.ntoken), yx_ntoken_abi);

        produce_blocks();

        const auto &accnt = control->db().get<account_object, by_name>(N(yx.ntoken));
        abi_def abi;
        BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);
        abi_ser.set_abi(abi);
    }

    action_result push_action(const account_name &signer, const action_name &name, const variant_object &data) {
        string action_type_name = abi_ser.get_action_type(name);

        action act;
        act.account = N(yx.ntoken);
        act.name = name;
        act.data = abi_ser.variant_to_binary(action_type_name, data);

        return base_tester::push_action(std::move(act), uint64_t(signer));
    }

    fc::variant get_stats(const string &symbolname) {
        auto symb = eosio::chain::symbol::from_string(symbolname);
        auto symbol_code = symb.to_symbol_code().value;
        vector<char> data = get_row_by_account(N(yx.ntoken), symbol_code, N(statsnative), symbol_code);
        return data.empty() ? fc::variant() : abi_ser.binary_to_variant("stats_native", data);
    }

    fc::variant get_account(account_name acc, const string &symbolname) {
        auto symb = eosio::chain::symbol::from_string(symbolname);
        auto symbol_code = symb.to_symbol_code().value;
        vector<char> data = get_row_by_account(N(yx.ntoken), acc, N(accounts), symbol_code);
        return data.empty() ? fc::variant() : abi_ser.binary_to_variant("account", data);
    }

    action_result regdepon(account_name issuer) {
        return push_action(N(yx.ntoken), N(regdepon), mvo()
                ("issuer", issuer)
        );
    }

    action_result issuen(account_name to, extended_asset quantity, string memo) {
        return push_action(quantity.contract, N(issuen), mvo()
                ("to", to)
                ("quantity", quantity)
                ("memo", memo)
        );
    }

    action_result redeemn(extended_asset quantity, string memo) {
        return push_action(quantity.contract, N(redeemn), mvo()
                ("quantity", quantity)
                ("memo", memo)
        );
    }

    action_result transfer(account_name from,
                           account_name to,
                           extended_asset quantity,
                           account_name payer,
                           string memo) {
        return push_action(from, N(transfer), mvo()
                ("from", from)
                ("to", to)
                ("quantity", quantity)
                ("payer", payer)
                ("memo", memo)
        );
    }

    action_result transfern(account_name from,
                            account_name to,
                            extended_asset quantity,
                            account_name payer,
                            string memo) {
        return push_action(from, N(transfern), mvo()
                ("from", from)
                ("to", to)
                ("quantity", quantity)
                ("payer", payer)
                ("memo", memo)
        );
    }

    abi_serializer abi_ser;
};

BOOST_AUTO_TEST_SUITE(yx_ntoken_tests)

    BOOST_FIXTURE_TEST_CASE(regdepon_tests, yx_ntoken_tester) try {

        auto token = regdepon(N(d1));
        auto stats = get_stats("4,DKRW");
        REQUIRE_MATCHING_OBJECT(stats, mvo()
                ("supply", "0")
                ("is_frozen", "false")
                ("depository", "d1")
        );
        produce_blocks(1);

    } FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
