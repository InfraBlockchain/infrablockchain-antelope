#ifndef YX_SEQUENCE_GENERATOR_HPP
#define YX_SEQUENCE_GENERATOR_HPP

#include <eosiolib/multi_index.hpp>

namespace yosemite {

    struct sequence_holder {
        const static uint64_t PRIMARY_KEY = N(seqgenerator);

        uint64_t sequence;

        uint64_t primary_key() const { return PRIMARY_KEY; }

        EOSLIB_SERIALIZE(sequence_holder, (sequence))
    };

    typedef eosio::multi_index<N(seqholders), sequence_holder> seqholders;

    class sequence_generator {
    public:
        static uint64_t new_sequence(uint64_t contract, uint64_t scope) {
            seqholders _sequences(contract, scope);
            auto itr = _sequences.find(sequence_holder::PRIMARY_KEY);

            if (itr == _sequences.end()) {
                _sequences.emplace(contract, [&](auto &seq_holder) {
                    seq_holder.sequence = 1;
                });
                return 1;
            } else {
                auto &current_seq = *itr;
                _sequences.modify(current_seq, 0, [&](auto &seq_holder) {
                    seq_holder.sequence += 1;
                });
                return current_seq.sequence;
            }
        }
    };

}

#endif // YX_SEQUENCE_GENERATOR_HPP
