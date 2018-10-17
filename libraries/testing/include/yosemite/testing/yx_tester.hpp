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
#include <yx.nft/yx.nft.wast.hpp>
#include <yx.nft/yx.nft.abi.hpp>
#include <yx.nftex/yx.nftex.wast.hpp>
#include <yx.nftex/yx.nftex.abi.hpp>

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
   using id_type = uint64_t;

   const string zero_ntoken = asset{0, symbol{YOSEMITE_NATIVE_TOKEN_SYMBOL}}.to_string();

   abi_serializer abi_ser_system;
   abi_serializer abi_ser_txfee;
   abi_serializer abi_ser_identity;
   abi_serializer abi_ser_ntoken;
   abi_serializer abi_ser_token;
   abi_serializer abi_ser_nft;
   abi_serializer abi_ser_nftex;

   void init_yosemite_contracts() {
      produce_blocks(2);

      create_accounts({N(d1), N(user1), N(user2), YOSEMITE_NATIVE_TOKEN_ACCOUNT, YOSEMITE_TX_FEE_ACCOUNT,
                       YOSEMITE_IDENTITY_ACCOUNT, YOSEMITE_USER_TOKEN_ACCOUNT, YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT,
                       YOSEMITE_NON_FUNGIBLE_TOKEN_EXCHANGE_ACCOUNT});
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

      set_tx_fee("tf.newacc", zero_ntoken);
      set_tx_fee("tf.regprod", zero_ntoken);
      set_tx_fee("tf.regsysdep", zero_ntoken);
      set_tx_fee("tf.regidauth", zero_ntoken);

      set_tx_fee("tf.nissue", zero_ntoken);
      set_tx_fee("tf.nredeem", asset{100000, symbol{YOSEMITE_NATIVE_TOKEN_SYMBOL}}.to_string());
      set_tx_fee("tf.ntransfer", asset{10000, symbol{YOSEMITE_NATIVE_TOKEN_SYMBOL}}.to_string());
      set_tx_fee("tf.transfer", asset{10000, symbol{YOSEMITE_NATIVE_TOKEN_SYMBOL}}.to_string());

      set_tx_fee("tf.tcreate", zero_ntoken);
      set_tx_fee("tf.tissue", zero_ntoken);
      set_tx_fee("tf.tredeem", zero_ntoken);
      set_tx_fee("tf.ttransfer", zero_ntoken);
      set_tx_fee("tf.tsetkyc", zero_ntoken);
      set_tx_fee("tf.tsetopts", zero_ntoken);
      set_tx_fee("tf.tfreezeac", zero_ntoken);

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

      auto &accnt_token = control->db().get<account_object, by_name>(YOSEMITE_USER_TOKEN_ACCOUNT);
      BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt_token.abi, abi), true);
      abi_ser_token.set_abi(abi, abi_serializer_max_time);

      tester::push_action(config::system_account_name, N(setpriv), config::system_account_name, mutable_variant_object()
            ("account", "yx.token")
            ("is_priv", 1));

      produce_blocks();


      set_code(YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT, yx_nft_wast);
      set_abi(YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT, yx_nft_abi);

      produce_blocks();

      auto &accnt_nft = control->db().get<account_object, by_name>(YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT);
      BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt_nft.abi, abi), true);
      abi_ser_nft.set_abi(abi, abi_serializer_max_time);

      tester::push_action(config::system_account_name, N(setpriv), config::system_account_name, mutable_variant_object()
            ("account", "yx.nft")
            ("is_priv", 1));


      set_code(YOSEMITE_NON_FUNGIBLE_TOKEN_EXCHANGE_ACCOUNT, yx_nftex_wast);
      set_abi(YOSEMITE_NON_FUNGIBLE_TOKEN_EXCHANGE_ACCOUNT, yx_nftex_abi);

      produce_blocks();

      auto &accnt_nftex = control->db().get<account_object, by_name>(YOSEMITE_NON_FUNGIBLE_TOKEN_EXCHANGE_ACCOUNT);
      BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt_nftex.abi, abi), true);
      abi_ser_nftex.set_abi(abi, abi_serializer_max_time);

      tester::push_action(config::system_account_name, N(setpriv), config::system_account_name, mutable_variant_object()
            ("account", "yx.nftex")
            ("is_priv", 1));


      prepare_system_depository(N(d1));
      prepare_identity_authority(N(d1));

      produce_blocks();
   }

   uint64_t current_time() {
      return static_cast<uint64_t>(control->pending_block_time().time_since_epoch().count());
   }

   uint32_t now() {
      return (uint32_t) (current_time() / 1000000);
   }

   string to_asset_string(share_type a, uint64_t symbol_value = YOSEMITE_NATIVE_TOKEN_SYMBOL) {
      return asset{a, symbol{symbol_value}}.to_string();
   }

   string to_yx_asset_string(share_type a, const string &issuer, uint64_t symbol_value = YOSEMITE_NATIVE_TOKEN_SYMBOL) {
      return yx_asset{asset{a, symbol{symbol_value}}, issuer}.to_string();
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

      result = set_id_info(idauth, idauth, 0, 15, 0, "");
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
         log_to_console(ex.to_detail_string());
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

   /* Native Token methods */

   fc::variant ntoken_get_stats(const account_name &depo) {
      vector<char> data = get_row_by_account(YOSEMITE_NATIVE_TOKEN_ACCOUNT, depo, N(ntstats), N(basicstats));
      return data.empty() ? fc::variant() : abi_ser_ntoken.binary_to_variant("stats_type", data, abi_serializer_max_time);
   }

   fc::variant ntoken_get_accounts(const account_name &acc, const account_name &depo) {
      vector<char> data = get_row_by_account(YOSEMITE_NATIVE_TOKEN_ACCOUNT, acc, N(ntaccounts), depo);
      return data.empty() ? fc::variant() : abi_ser_ntoken.binary_to_variant("account_type", data, abi_serializer_max_time);
   }

   fc::variant ntoken_get_accounts_total(const account_name &acc) {
      vector<char> data = get_row_by_account(YOSEMITE_NATIVE_TOKEN_ACCOUNT, acc, N(ntaccountstt), N(totalbal));
      return data.empty() ? fc::variant() : abi_ser_ntoken.binary_to_variant("account_total_type", data,
                                                                             abi_serializer_max_time);
   }

   action_result ntoken_set_kyc_rule(uint8_t type, uint16_t kyc_flags) {
      return push_action(N(setkycrule), mvo()
            ("type", type)
            ("kyc", kyc_flags), config::system_account_name, abi_ser_ntoken);
   }

   action_result nissue(const account_name &to, const string &token, const string &memo) {
      auto _token = yx_asset::from_string(token);
      return push_action(N(nissue), mvo()
            ("to", to)
            ("token", _token)
            ("memo", memo), _token.issuer, abi_ser_ntoken);
   }

   transaction_trace_ptr nredeem(const string &token, const string &memo) {
      auto _token = yx_asset::from_string(token);
      return base_tester::push_action(YOSEMITE_NATIVE_TOKEN_ACCOUNT, N(nredeem), _token.issuer, mvo()
            ("token", _token)
            ("memo", memo));
   }

   action_result nredeem_with_simple_result(const string &token, const string &memo) {
      try {
         nredeem(token, memo);
         return success();
      } catch (const fc::exception &ex) {
         return error(ex.top_message());
      }
   }

   action_result ntoken_transfer(account_name from, account_name to, const string &amount, const string &memo) {
      return push_action(N(transfer), mvo()
            ("from", from)
            ("to", to)
            ("amount", amount)
            ("memo", memo), from, abi_ser_ntoken);
   }

   action_result
   ntoken_wptransfer(account_name from, account_name to, const string &amount, account_name payer, const string &memo) {
      try {
         base_tester::push_action(YOSEMITE_NATIVE_TOKEN_ACCOUNT, N(wptransfer),
                                  vector<account_name>{from, payer}, mvo()
                                        ("from", from)
                                        ("to", to)
                                        ("amount", amount)
                                        ("payer", payer)
                                        ("memo", memo));
         return success();
      } catch (const fc::exception &ex) {
         return error(ex.top_message());
      }
   }

   action_result ntransfer(account_name from, account_name to, const string &token, const string &memo) {
      auto _token = yx_asset::from_string(token);
      return push_action(N(ntransfer), mvo()
            ("from", from)
            ("to", to)
            ("token", _token)
            ("memo", memo), from, abi_ser_ntoken);
   }

   action_result
   wpntransfer(account_name from, account_name to, const string &token, account_name payer, const string &memo) {
      try {
         auto _token = yx_asset::from_string(token);
         base_tester::push_action(YOSEMITE_NATIVE_TOKEN_ACCOUNT, N(wpntransfer),
                                  vector<account_name>{from, payer}, mvo()
                                        ("from", from)
                                        ("to", to)
                                        ("token", _token)
                                        ("payer", payer)
                                        ("memo", memo));
         return success();
      } catch (const fc::exception &ex) {
         return error(ex.top_message());
      }
   }

   /* Token methods */

   fc::variant token_get_stats(int64_t token_symbol, const account_name &issuer) {
      vector<char> data = get_row_by_account(YOSEMITE_USER_TOKEN_ACCOUNT, token_symbol, N(tstats), issuer);
      return data.empty() ? fc::variant() : abi_ser_token.binary_to_variant("stats_type", data, abi_serializer_max_time);
   }

   fc::variant token_get_accounts(const account_name &owner, const account_name &issuer) {
      vector<char> data = get_row_by_account(YOSEMITE_USER_TOKEN_ACCOUNT, owner, N(taccounts), issuer);
      return data.empty() ? fc::variant() : abi_ser_token.binary_to_variant("accounts_type", data, abi_serializer_max_time);
   }

   action_result token_set_kyc_rule(const string &ysymbol, uint8_t type, uint16_t kyc_flags) {
      yx_symbol _ysymbol = yx_symbol::from_string(ysymbol);
      return push_action(N(setkycrule), mvo()
            ("ysymbol", _ysymbol)
            ("type", type)
            ("kyc", kyc_flags), _ysymbol.issuer, abi_ser_token, YOSEMITE_USER_TOKEN_ACCOUNT);
   }

   action_result token_set_options(const string &ysymbol, uint16_t options, bool reset) {
      yx_symbol _ysymbol = yx_symbol::from_string(ysymbol);
      return push_action(N(setoptions), mvo()
                               ("ysymbol", _ysymbol)
                               ("options", options)
                               ("reset", reset),
                         _ysymbol.issuer, abi_ser_token, YOSEMITE_USER_TOKEN_ACCOUNT);
   }

   action_result token_freeze_account(const string &ysymbol, const vector<account_name> &accs, bool freeze) {
      yx_symbol _ysymbol = yx_symbol::from_string(ysymbol);
      return push_action(N(freezeacc), mvo()
                               ("ysymbol", _ysymbol)
                               ("accs", accs)
                               ("freeze", freeze),
                         _ysymbol.issuer, abi_ser_token, YOSEMITE_USER_TOKEN_ACCOUNT);
   }

   action_result token_create(const string &ysymbol, uint16_t can_set_options) {
      yx_symbol _ysymbol = yx_symbol::from_string(ysymbol);
      return push_action(N(create), mvo()
                               ("ysymbol", _ysymbol)
                               ("can_set_options", can_set_options),
                         _ysymbol.issuer, abi_ser_token, YOSEMITE_USER_TOKEN_ACCOUNT);
   }

   action_result token_issue(const account_name &to, const string &token, const string &memo) {
      auto _token = yx_asset::from_string(token);
      return push_action(N(issue), mvo()
            ("to", to)
            ("token", _token)
            ("memo", memo), _token.issuer, abi_ser_token, YOSEMITE_USER_TOKEN_ACCOUNT);
   }

   transaction_trace_ptr token_redeem(const string &token, const string &memo) {
      auto _token = yx_asset::from_string(token);
      return base_tester::push_action(YOSEMITE_USER_TOKEN_ACCOUNT, N(redeem), _token.issuer, mvo()
            ("token", _token)
            ("memo", memo));
   }

   action_result token_redeem_with_simple_result(const string &token, const string &memo) {
      try {
         token_redeem(token, memo);
         return success();
      } catch (const fc::exception &ex) {
         return error(ex.top_message());
      }
   }

   action_result token_transfer(account_name from, account_name to, const string &token, const string &memo) {
      auto _token = yx_asset::from_string(token);
      return push_action(N(transfer), mvo()
            ("from", from)
            ("to", to)
            ("token", _token)
            ("memo", memo), from, abi_ser_token, YOSEMITE_USER_TOKEN_ACCOUNT);
   }

   action_result
   token_wptransfer(account_name from, account_name to, const string &token, account_name payer, const string &memo) {
      try {
         auto _token = yx_asset::from_string(token);
         base_tester::push_action(YOSEMITE_USER_TOKEN_ACCOUNT, N(wptransfer),
                                  vector<account_name>{from, payer}, mvo()
                                        ("from", from)
                                        ("to", to)
                                        ("token", _token)
                                        ("payer", payer)
                                        ("memo", memo));
         return success();
      } catch (const fc::exception &ex) {
         return error(ex.top_message());
      }
   }

   /* NFT methods */

   fc::variant nft_get_stats(int64_t token_symbol, const account_name &issuer) {
      vector<char> data = get_row_by_account(YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT, token_symbol, N(tstats), issuer);
      return data.empty() ? fc::variant() : abi_ser_nft.binary_to_variant("stats_type", data, abi_serializer_max_time);
   }

   fc::variant nft_get_accounts(const account_name &owner, const account_name &issuer) {
      vector<char> data = get_row_by_account(YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT, owner, N(taccounts), issuer);
      return data.empty() ? fc::variant() : abi_ser_nft.binary_to_variant("accounts_type", data, abi_serializer_max_time);
   }

   fc::variant nft_get_token(id_type token_id, account_name issuer) {
      vector<char> data = get_row_by_account(YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT, issuer, N(nftokens), token_id);
      FC_ASSERT(!data.empty(), "empty token");
      return data.empty() ? fc::variant() : abi_ser_nft.binary_to_variant("nftokens_type", data, abi_serializer_max_time);
   }

   action_result nft_set_kyc_rule(const string &ysymbol, uint8_t type, uint16_t kyc_flags) {
      yx_symbol _ysymbol = yx_symbol::from_string(ysymbol);
      return push_action(N(setkycrule), mvo()
            ("ysymbol", _ysymbol)
            ("type", type)
            ("kyc", kyc_flags), _ysymbol.issuer, abi_ser_nft, YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT);
   }

   action_result nft_set_options(const string &ysymbol, uint16_t options, bool reset) {
      yx_symbol _ysymbol = yx_symbol::from_string(ysymbol);
      return push_action(N(setoptions), mvo()
                               ("ysymbol", _ysymbol)
                               ("options", options)
                               ("reset", reset),
                         _ysymbol.issuer, abi_ser_nft, YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT);
   }

   action_result nft_freeze_account(const string &ysymbol, const vector<account_name> &accs, bool freeze) {
      yx_symbol _ysymbol = yx_symbol::from_string(ysymbol);
      return push_action(N(freezeacc), mvo()
                               ("ysymbol", _ysymbol)
                               ("accs", accs)
                               ("freeze", freeze),
                         _ysymbol.issuer, abi_ser_nft, YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT);
   }

   action_result nft_create(const string &ysymbol, uint16_t can_set_options) {
      yx_symbol _ysymbol = yx_symbol::from_string(ysymbol);
      return push_action(N(create), mvo()
                               ("ysymbol", _ysymbol)
                               ("can_set_options", can_set_options),
                         _ysymbol.issuer, abi_ser_nft, YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT);
   }

   template<typename T>
   action_result
   nft_issue(const account_name &to, const string &token, const T &ids, const vector<string> &uris, const string &name, const string &memo) {
      auto _token = yx_asset::from_string(token);
      return push_action(N(issue), mvo()
                               ("to", to)
                               ("token", _token)
                               ("ids", ids)
                               ("uris", uris)
                               ("name", name)
                               ("memo", memo),
                         _token.issuer, abi_ser_nft, YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT
      );
   }

   template<typename T>
   action_result
   nft_transferid(account_name from, account_name to, account_name issuer, const T &ids, const string &memo) {
      return push_action(N(transferid), mvo()
                               ("from", from)
                               ("to", to)
                               ("issuer", issuer)
                               ("ids", ids)
                               ("memo", memo),
                         from, abi_ser_nft, YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT
      );
   }

   action_result nft_transfer(account_name from, account_name to, const string &token, const string &memo) {
      auto _token = yx_asset::from_string(token);
      return push_action(N(transfer), mvo()
                               ("from", from)
                               ("to", to)
                               ("token", _token)
                               ("memo", memo),
                         from, abi_ser_nft, YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT
      );
   }

   template<typename T>
   action_result nft_redeem(account_name issuer, const T &ids, const string &memo) {
      return push_action(N(redeem), mvo()
                               ("issuer", issuer)
                               ("ids", ids)
                               ("memo", memo),
                         issuer, abi_ser_nft, YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT
      );
   }
};

}} // namespace yosemite::testing

#endif //YOSEMITE_YX_TESTER_HPP
