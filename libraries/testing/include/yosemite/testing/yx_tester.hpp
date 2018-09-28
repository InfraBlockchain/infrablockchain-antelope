#ifndef YOSEMITE_YX_TESTER_HPP
#define YOSEMITE_YX_TESTER_HPP

#include <boost/test/unit_test.hpp>
#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include <yosemite/chain/system_accounts.hpp>
#include <yosemite/chain/yx_asset.hpp>
#include <yosemite/chain/native_token_symbol.hpp>

#include <yx.system/yx.system.wast.hpp>
#include <yx.system/yx.system.abi.hpp>
#include <yx.txfee/yx.txfee.wast.hpp>
#include <yx.txfee/yx.txfee.abi.hpp>
#include <yx.identity/yx.identity.wast.hpp>
#include <yx.identity/yx.identity.abi.hpp>
#include <yx.ntoken/yx.ntoken.wast.hpp>
#include <yx.ntoken/yx.ntoken.abi.hpp>
#include <yx.token/yx.token.wast.hpp>
#include <yx.token/yx.token.abi.hpp>
#include <yx.dcontract/yx.dcontract.wast.hpp>
#include <yx.dcontract/yx.dcontract.abi.hpp>

#include <Runtime/Runtime.h>

#include <fc/variant_object.hpp>

using namespace eosio::testing;
using namespace eosio;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;
using namespace std;
using namespace yosemite::chain;

using mvo = fc::mutable_variant_object;

namespace yosemite { namespace testing {

class yx_tester : public tester {
public:
   abi_serializer abi_ser_system;
   abi_serializer abi_ser_txfee;
   abi_serializer abi_ser_identity;
   abi_serializer abi_ser_ntoken;
   abi_serializer abi_ser_token;

   void init_yosemite_contracts() {
      produce_blocks(2);

      create_accounts({N(d1), N(user1), N(user2), YOSEMITE_NATIVE_TOKEN_ACCOUNT, YOSEMITE_TX_FEE_ACCOUNT,
                       YOSEMITE_IDENTITY_ACCOUNT, YOSEMITE_USER_TOKEN_ACCOUNT});
      produce_blocks(2);

      set_code(config::system_account_name, yx_system_wast);
      set_abi(config::system_account_name, yx_system_abi);

      produce_blocks();

      auto &accnt2 = control->db().get<account_object, by_name>(config::system_account_name);
      abi_def abi;
      BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt2.abi, abi), true);
      abi_ser_system.set_abi(abi, abi_serializer_max_time);

      set_code(YOSEMITE_TX_FEE_ACCOUNT, yx_txfee_wast);
      set_abi(YOSEMITE_TX_FEE_ACCOUNT, yx_txfee_abi);

      produce_blocks();

      auto &accnt3 = control->db().get<account_object, by_name>(YOSEMITE_TX_FEE_ACCOUNT);
      BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt3.abi, abi), true);
      abi_ser_txfee.set_abi(abi, abi_serializer_max_time);

      set_tx_fee("tf.newacc", "0.0000 DKRW");
      set_tx_fee("tf.regprod", "0.0000 DKRW");
      set_tx_fee("tf.regsysdep", "0.0000 DKRW");
      set_tx_fee("tf.regidauth", "0.0000 DKRW");

      set_tx_fee("tf.nissue", "0.0000 DKRW");
      set_tx_fee("tf.nredeem", "1000.0000 DKRW");
      set_tx_fee("tf.ntransfer", "100.0000 DKRW");
      set_tx_fee("tf.transfer", "100.0000 DKRW");

      set_tx_fee("tf.tcreate", "0.0000 DKRW");
      set_tx_fee("tf.tissue", "0.0000 DKRW");
      set_tx_fee("tf.tredeem", "0.0000 DKRW");
      set_tx_fee("tf.ttransfer", "0.0000 DKRW");
      set_tx_fee("tf.tsetkyc", "0.0000 DKRW");
      set_tx_fee("tf.tsetopts", "0.0000 DKRW");
      set_tx_fee("tf.tfreezeac", "0.0000 DKRW");

      produce_blocks();

      set_code(YOSEMITE_NATIVE_TOKEN_ACCOUNT, yx_ntoken_wast);
      set_abi(YOSEMITE_NATIVE_TOKEN_ACCOUNT, yx_ntoken_abi);

      produce_blocks();

      auto &accnt = control->db().get<account_object, by_name>(YOSEMITE_NATIVE_TOKEN_ACCOUNT);
      BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);
      abi_ser_ntoken.set_abi(abi, abi_serializer_max_time);

      produce_blocks();

      set_code(YOSEMITE_IDENTITY_ACCOUNT, yx_identity_wast);
      set_abi(YOSEMITE_IDENTITY_ACCOUNT, yx_identity_abi);

      auto &accnt4 = control->db().get<account_object, by_name>(YOSEMITE_IDENTITY_ACCOUNT);
      BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt4.abi, abi), true);
      abi_ser_identity.set_abi(abi, abi_serializer_max_time);

      produce_blocks();

      set_code(YOSEMITE_USER_TOKEN_ACCOUNT, yx_token_wast);
      set_abi(YOSEMITE_USER_TOKEN_ACCOUNT, yx_token_abi);

      produce_blocks();

      auto &accnt5 = control->db().get<account_object, by_name>(YOSEMITE_USER_TOKEN_ACCOUNT);
      BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt5.abi, abi), true);
      abi_ser_token.set_abi(abi, abi_serializer_max_time);

      prepare_system_depository(N(d1));
      prepare_identity_authority(N(d1));

      produce_blocks();
   }

   action_result push_action(const action_name &name, const variant_object &data, const account_name &signer,
                             const abi_serializer &abi_ser,
                             const account_name &code = YOSEMITE_NATIVE_TOKEN_ACCOUNT) {
      string action_type_name = abi_ser.get_action_type(name);

      action act;
      act.account = code;
      act.name = name;
      act.data = abi_ser.variant_to_binary(action_type_name, data, abi_serializer_max_time);

      return base_tester::push_action(std::move(act), uint64_t(signer));
   }

   action_result register_sysdepo(account_name depository, const string &url, uint16_t location) {
      try {
         base_tester::push_action(config::system_account_name, N(regsysdepo),
                                  vector<account_name>{depository, config::system_account_name}, mvo()
                                        ("depository", depository)
                                        ("url", url)
                                        ("location", location));
         return success();
      } catch (const fc::exception &ex) {
         return error(ex.top_message());
      }
   }

   action_result authorize_sysdepo(account_name depository) {
      return push_action(N(authsysdepo), mvo()
            ("depository", depository), config::system_account_name, abi_ser_system, config::system_account_name);
   }

   action_result register_idauth(account_name idauth, const string &url, uint16_t location) {
      try {
         base_tester::push_action(config::system_account_name, N(regidauth),
                                  vector<account_name>{idauth, config::system_account_name}, mvo()
                                        ("identity_authority", idauth)
                                        ("url", url)
                                        ("location", location));
         return success();
      } catch (const fc::exception &ex) {
         return error(ex.top_message());
      }
   }

   action_result authorize_idauth(account_name idauth) {
      return push_action(N(authidauth), mvo()
                               ("identity_authority", idauth), config::system_account_name, abi_ser_system,
                         config::system_account_name);
   }

   action_result set_tx_fee(const string &operation, const string &fee) {
      return push_action(N(settxfee), mvo()
            ("operation", operation)
            ("fee", fee), config::system_account_name, abi_ser_txfee, YOSEMITE_TX_FEE_ACCOUNT);
   }

   void prepare_system_depository(account_name depo) {
      auto result = register_sysdepo(depo, "test.org", 1);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = authorize_sysdepo(depo);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);
   }

   void prepare_identity_authority(account_name idauth) {
      auto result = register_idauth(idauth, "idauth.org", 1);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = authorize_idauth(idauth);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = set_id_info(N(d1), N(d1), 0, 15, 0, "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();
   }

   action_result
   set_id_info(account_name account, account_name idauth, uint16_t type, uint16_t kyc, uint32_t state,
               const std::string &data) {
      try {
         base_tester::push_action(YOSEMITE_IDENTITY_ACCOUNT, N(setidinfo), idauth, mvo()
               ("account", account)
               ("identity_authority", idauth)
               ("type", type)
               ("kyc", kyc)
               ("state", state)
               ("data", data)
         );
         return success();
      } catch (const fc::exception &ex) {
         return error(ex.top_message());
      }
   }

   action_result setkyc(account_name account, uint16_t kyc, account_name idauth) {
      try {
         base_tester::push_action(YOSEMITE_IDENTITY_ACCOUNT, N(setkyc), idauth, mvo()
               ("account", account)
               ("kyc", kyc));
         return success();
      } catch (const fc::exception &ex) {
         return error(ex.top_message());
      }
   }
};

}} // namespace yosemite::testing

#endif //YOSEMITE_YX_TESTER_HPP
