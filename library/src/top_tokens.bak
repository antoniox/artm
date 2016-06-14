#include <deque>

#include "top_tokens.h"
#include "utils.h"


TopTokens::TopTokens(
    size_type topics_count,
    const std::vector<size_type> & token_counts
) : token_counts_(token_counts) {

    FOR(id_type, type_id, TYPE_COUNT) {
        auto & type_slice = matrix[type_id];
        type_slice.reserve(topics_count);

        FOR(id_type, topic_id, topics_count) {
            type_slice.push_back(
                std::array<
                    boost::heap::priority_queue<TokenProbability>,
                    MODALITY_COUNT
                >()
            );

            auto & topic_slice = type_slice.back();

            FOR(id_type, modality_id, MODALITY_COUNT) {
                Modality modality = static_cast<Modality>(modality_id);
                size_type tokens_count = token_counts[modality_id];

                auto & modality_slice = topic_slice[modality];
                modality_slice.reserve(tokens_count);
            }
        }
    }
}


void TopTokens::fill_from(const Phi & phi) {
    FOR(id_type, type_id, matrix.size()) {
        Type type = static_cast<Type>(type_id);
        auto & type_slice = matrix[type];
        auto & phi_type_slice = phi[type];

        FOR(id_type, topic_id, type_slice.size()) {
            auto & topic_slice = type_slice[topic_id];
            auto & phi_topic_slice = phi_type_slice[topic_id];

            FOR(id_type, modality_id, topic_slice.size()) {
                Modality modality = static_cast<Modality>(modality_id);
                auto & modality_slice = topic_slice[modality];
                auto & phi_modality_slice = phi_topic_slice[modality];

                FOR(id_type, token_id, phi_modality_slice.size()) {
                    auto & phi_value = phi_modality_slice[token_id];

                    TokenProbability token_probability = {{modality, token_id}, phi_value};
                    
                    if (modality_slice.size() < token_counts_[modality_id]) {
                        modality_slice.push(token_probability);
                    } else if (token_probability < modality_slice.top()) {
                        modality_slice.pop();
                        modality_slice.push(token_probability);
                    }
                }
            }
        }
    }
}


void TopTokens::print(const Vocabulary & vocabulary, std::ostream & output) {
    FOR(id_type, type_id, matrix.size()) {
        Type type = static_cast<Type>(type_id);
        auto & type_slice = matrix[type];

        FOR(id_type, topic_id, type_slice.size()) {
            auto & topic_slice = type_slice[topic_id];

            FOR(id_type, modality_id, topic_slice.size()) {
                Modality modality = static_cast<Modality>(modality_id);
                auto & modality_slice = topic_slice[modality];

                output << type << '\t' << topic_id << '\t' << modality << '\t';

                std::deque<TokenProbability> tokens;

                while (modality_slice.size() > 0) {
                    tokens.push_front(modality_slice.top());
                    modality_slice.pop();
                }

                for (const auto & token_probability : tokens) {
                    std::string token = vocabulary.token(token_probability.token);
                    float_type probability = token_probability.value;

                    output << token << ", " << probability << '\t';
                }

                output << std::endl;
            }
        }
    }
}
