/**
 *  @file
 *  @copyright defined in yosemite-public-blockchain/LICENSE
 */
#include <yosemitelib/transaction_fee.hpp>
#include "yx.nft.hpp"

namespace yosemite { namespace non_native_token {
   using std::string;
   using eosio::asset;

   void nft::inner_check_create_parameters(const yx_symbol &ysymbol, uint16_t /*can_set_options*/) {
      eosio_assert(static_cast<uint32_t>(ysymbol.precision() == 0), "the precision of non-fungible token must be 0");
   }

   // @abi action
   void nft::issue(account_name to, const yx_asset &token, const vector<id_type> &ids, const vector<string> &uris,
                   const string &name, const string &memo) {
      check_issue_parameters(to, token, memo);
      eosio_assert(static_cast<uint32_t>(token.get_yx_symbol().precision() == 0), "the precision of non-fungible token must be 0");
      eosio_assert(static_cast<uint32_t>(token.amount == ids.size()), "mismatch between token amount and the number of ids provided");
      eosio_assert(static_cast<uint32_t>(token.amount == uris.size()), "mismatch between token amount and the number of uris provided");
      eosio_assert(static_cast<uint32_t>(uris.size() <= 100), "the number of uris cannot exceed 100");
      eosio_assert(static_cast<uint32_t>(name.size() <= 32), "name has more than 32 bytes");
      eosio_assert(static_cast<uint32_t>(!name.empty()), "name is empty");

      stats stats_table(get_self(), token.symbol.value);
      const auto &tstats = stats_table.get(token.issuer, "token is not yet created");

      stats_table.modify(tstats, 0, [&](auto &s) {
         s.supply.amount += token.amount;
         eosio_assert(static_cast<uint32_t>(s.supply.amount > 0 && s.supply.amount <= asset::max_amount),
                      "token amount cannot be more than 2^62 - 1");
      });

      // Mint nfts
      token_index tokens(get_self(), token.issuer);
      const yx_symbol &ysymbol = token.get_yx_symbol();
      yx_asset token_issued = yx_asset{1, ysymbol};

      for (unsigned int i = 0; i < ids.size(); i++) {
         auto const &mint_id = ids.at(i);
         eosio_assert(static_cast<uint32_t>(tokens.find(mint_id) == tokens.end()), "token with specified ID already exists");
         // we expect that uri would be uuid; https://stackoverflow.com/questions/13397038/uuid-max-character-length
         string uri = uris.at(i);
         eosio_assert(static_cast<uint32_t>(uri.size() <= 64), "uri has more than 64 bytes");

         tokens.emplace(get_self(), [&](auto &_token) {
            _token.id = mint_id;
            _token.uri = uri;
            _token.owner = token.issuer;
            _token.value = token_issued;
            _token.name = name;
         });
      }

      yx_asset token_to_add = yx_asset{static_cast<int64_t>(ids.size()), ysymbol};
      add_token_balance(token.issuer, token_to_add);

      if (to != token.issuer) {
         SEND_INLINE_ACTION(*this, transferid,
                            {{token.issuer, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                            {token.issuer, to, token.issuer, ids, memo});
      }

      charge_fee(token.issuer, YOSEMITE_TX_FEE_OP_NAME_TOKEN_ISSUE);
   }

   // @abi action
   void nft::redeem(account_name issuer, const vector<id_type> &ids, const string &memo) {
      eosio_assert(static_cast<uint32_t>(!ids.empty()), "ids cannot be empty");
      eosio_assert(static_cast<uint32_t>(ids.size() <= 100), "the number of ids cannot exceed 100");
      eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");
      require_auth(issuer);

      token_index tokens(get_self(), issuer);

      // Find token to burn
      for (auto token_id : ids) {
         auto burn_token = tokens.find(token_id);
         eosio_assert(static_cast<uint32_t>(burn_token != tokens.end()), "token with specified ID does not exist");
         eosio_assert(static_cast<uint32_t>(burn_token->owner == issuer), "token not owned by issuer");

         stats stats_table(get_self(), burn_token->value.symbol.value);
         const auto &tstats = stats_table.get(issuer, "token is not yet created");

         // Remove token from tokens table
         tokens.erase(burn_token);

         stats_table.modify(tstats, 0, [&](auto &s) {
            s.supply.amount -= burn_token->value.amount;
         });

         // Lower balance from owner
         sub_token_balance(issuer, burn_token->value);
      }

      charge_fee(issuer, YOSEMITE_TX_FEE_OP_NAME_TOKEN_REDEEM);
   }

   // @abi action
   void nft::transferid(account_name from, account_name to, account_name issuer, const vector<id_type> &ids, const string &memo) {
      bool is_auth_by_sysaccount = has_auth(YOSEMITE_SYSTEM_ACCOUNT);
      if (!is_auth_by_sysaccount) {
         eosio_assert(static_cast<uint32_t>(!ids.empty()), "ids cannot be empty");
         eosio_assert(static_cast<uint32_t>(from != to), "from and to account cannot be the same");
         eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");
         require_auth(from);
         eosio_assert(static_cast<uint32_t>(is_account(to)), "to account does not exist");
      }

      // Ensure token ID exists
      token_index tokens(get_self(), issuer);
      for (auto id : ids) {
         const auto &sender_token = tokens.get(id, "token with specified ID does not exist");
         eosio_assert(static_cast<uint32_t>(sender_token.owner == from), "from account does not own token with specified ID");

         if (!is_auth_by_sysaccount) {
            check_transfer_rules(from, to, sender_token.value);
         }

         // Transfer NFT from sender to receiver
         tokens.modify(sender_token, 0, [&](auto &token) {
            token.owner = to;
         });

         // Change balance of both accounts
         sub_token_balance(from, sender_token.value);
         add_token_balance(to, sender_token.value);
      }

      require_recipient(from);
      require_recipient(to);

      if (!is_auth_by_sysaccount) {
         charge_fee(from, YOSEMITE_TX_FEE_OP_NAME_TOKEN_TRANSFER);
      }
   }

   // @abi action
   void nft::transfer(account_name from, account_name to, const yx_asset &token, const string &memo) {
      bool is_auth_by_sysaccount = has_auth(YOSEMITE_SYSTEM_ACCOUNT);
      if (!is_auth_by_sysaccount) {
         check_transfer_parameters(from, to, token, memo);
      }
      eosio_assert(static_cast<uint32_t>(token.amount == 1), "token amount must be 1 with precision 0"); // it checks precision, too
      check_transfer_rules(from, to, token);

      token_index tokens(get_self(), token.issuer);
      auto symbl = tokens.get_index<N(bysymbol)>();

      bool found = false;
      id_type id = 0;
      for (const auto &it : symbl) {
         if (it.value.symbol == token.symbol && it.owner == from) {
            id = it.id;
            found = true;
            break;
         }
      }
      eosio_assert(static_cast<uint32_t>(found), "token is not found or is not owned by from account");

      require_recipient(from);
      require_recipient(to);

      vector<id_type> ids = vector<id_type>{id};
      SEND_INLINE_ACTION(*this, transferid, {{from, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
            {from, to, token.issuer, ids, memo});

      if (!is_auth_by_sysaccount) {
         charge_fee(from, YOSEMITE_TX_FEE_OP_NAME_TOKEN_TRANSFER);
      }
   }

}}

EOSIO_ABI(yosemite::non_native_token::nft, (create)(issue)(redeem)(transfer)(transferid)(setkycrule)(setoptions)(freezeacc))
