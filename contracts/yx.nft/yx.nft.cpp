/**
 *  @file
 *  @copyright defined in yosemite-public-blockchain/LICENSE
 */
#include "yx.nft.hpp"

namespace yosemite { namespace non_native_token {
   using std::string;
   using eosio::asset;

   // @abi action
   void nft::issue(account_name to, const yx_asset &token, const vector<string> &uris, const string &name,
                   const string &memo) {
      check_issue_parameters(to, token, memo);
      eosio_assert(static_cast<uint32_t>(name.size() <= 32), "name has more than 32 bytes");
      eosio_assert(static_cast<uint32_t>(!name.empty()), "name is empty");
      eosio_assert(static_cast<uint32_t>(token.amount == uris.size()), "mismatch between the number of tokens and uris provided");

      stats stats_table(get_self(), token.symbol.value);
      const auto &tstats = stats_table.get(token.issuer, "token is not yet created");

      // Increase supply
      stats_table.modify(tstats, 0, [&](auto &s) {
         s.supply.amount += token.amount;
         eosio_assert(static_cast<uint32_t>(s.supply.amount > 0 && s.supply.amount <= asset::max_amount),
                      "token amount cannot be more than 2^62 - 1");
      });

      // Mint nfts
      const yx_symbol &ysymbol = token.get_yx_symbol();
      for (auto const &uri: uris) {
         mint(to, yx_asset{1, ysymbol}, uri, name);
      }

      if (to != token.issuer) {
         INLINE_ACTION_SENDER(nft, transfer)
               (get_self(),
                {{token.issuer, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                {token.issuer, to, token, memo});
      }

      //TODO:transaction fee
   }

   // @abi action
   void nft::transferid(account_name from, account_name to, id_type id, const string &memo) {
      eosio_assert(static_cast<uint32_t>(from != to), "from and to account cannot be the same");
      eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");
      require_auth(from);
      eosio_assert(static_cast<uint32_t>(is_account(to)), "to account does not exist");

      // Ensure token ID exists
      auto sender_token = tokens.find(id);
      eosio_assert(static_cast<uint32_t>(sender_token != tokens.end()), "token with specified ID does not exist");
      eosio_assert(static_cast<uint32_t>(sender_token->owner == from), "from account does not own token with specified ID");

      //TODO:transfer rule check

      const auto &st = *sender_token;

      require_recipient(from);
      require_recipient(to);

      // Transfer NFT from sender to receiver
      tokens.modify(st, 0, [&](auto &token) {
         token.owner = to;
      });

      // Change balance of both accounts
      sub_token_balance(from, st.value);
      add_token_balance(to, st.value);

      //TODO:transaction fee
   }

   // @abi action
   void nft::transfer(account_name from, account_name to, const yx_asset &token, const string &memo) {
      check_transfer_parameters(from, to, token, memo);
      eosio_assert(static_cast<uint32_t>(token.amount == 1), "token amount must be 1");
      check_transfer_rules(from, to, token);

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

      SEND_INLINE_ACTION(*this, transferid, { from, N(active) }, { from, to, id, memo });
   }

   void nft::mint(account_name owner, const yx_asset &value, const string &uri, const string &name) {
      tokens.emplace(get_self(), [&](auto &_token) {
         _token.id = tokens.available_primary_key();
         _token.uri = uri;
         _token.owner = owner;
         _token.value = value;
         _token.name = name;
      });
   }

   // @abi action
   void nft::redeem(account_name owner, id_type token_id) {
      require_auth(owner);

      // Find token to burn
      auto burn_token = tokens.find(token_id);
      eosio_assert(static_cast<uint32_t>(burn_token != tokens.end()), "token with id does not exist");
      eosio_assert(static_cast<uint32_t>(burn_token->owner == owner), "token not owned by specifed owner");

      stats stats_table(get_self(), burn_token->value.symbol.value);
      const auto &tstats = stats_table.get(burn_token->value.issuer, "token is not yet created");

      // Remove token from tokens table
      tokens.erase(burn_token);

      stats_table.modify(tstats, 0, [&](auto &s) {
         s.supply.amount -= burn_token->value.amount;
      });

      // Lower balance from owner
      sub_token_balance(owner, burn_token->value);

      //TODO:transaction fee
   }

   void nft::inner_check_create_parameters(const yx_symbol &ysymbol, uint16_t can_set_options) {
      eosio_assert(static_cast<uint32_t>(ysymbol.precision() == 0), "non-fungible token doesn't support precision");
   }

}}

EOSIO_ABI(yosemite::non_native_token::nft, (create)(issue)(redeem)(transfer)(transferid)(setkycrule)(setoptions)(freezeacc))
