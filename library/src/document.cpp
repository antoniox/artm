#include "document.h"
#include "modality.h"
#include "typedefs.h"
#include "utils.h"



Token::Token(const Modality & modality, id_type id)
    : modality(modality), id(id) {
}


Edge::Edge(std::vector<Token> && tokens, const Type & type, id_type id)
    : tokens(std::move(tokens)), type(type), id(id) {
}


EdgeEntry::EdgeEntry(Edge && edge, size_type entries)
    : edge(std::move(edge)), entries(entries) {
}


DocumentParser::DocumentParser(std::istream & input)
    : input_(input), has_more_(false) {

    if (input_) {
        cached_document_record_.load(input_);
        has_more_ = true;
    }
}


bool DocumentParser::has_more() const {
    return has_more_;
}


void DocumentParser::parse(Document & document) {
    id_type edge_id = 0;

    id_type first_document_id = cached_document_record_.document_id;
    id_type document_id = cached_document_record_.document_id;

    while (document_id == first_document_id) {
        id_type word_id = cached_document_record_.word_id;
        id_type nick_id = cached_document_record_.nick_id;
        id_type date_id = cached_document_record_.date_id;
        size_type entries = cached_document_record_.entries;

        EdgeEntry edge_entry = {
            Edge {
                {
                    Token(WORD, word_id),
                    Token(NICK, nick_id),
                    Token(DATE, date_id)
                },
                FIRST,
                edge_id
            },
            entries
        };

        document.edge_entries.push_back(edge_entry); 

        ++edge_id;

        if (input_) {
            cached_document_record_.load(input_);
            document_id = cached_document_record_.document_id;
        } else {
            has_more_ = false;
            break;
        }
    }
}
