#include "corpora/livejournal.h"
#include "utils/misc.h"


LiveJournalCorpusImpl::LiveJournalCorpusImpl(const std::string & filename)
    : filename_(filename) {}


std::shared_ptr<CorpusIterator> LiveJournalCorpusImpl::iterator() {
    return std::shared_ptr<LiveJournalCorpusIterator>(
        new LiveJournalCorpusIterator(*this)
    );
}


const Vocabulary & LiveJournalCorpusImpl::vocabulary() const {
    return vocabulary_;
}


LiveJournalCorpusIterator::LiveJournalCorpusIterator(LiveJournalCorpusImpl & corpus)
    : corpus_(corpus) {

    input_ = std::ifstream(corpus_.filename_);
    skip_line(input_);

    has_more_ = parse_document();
}


bool LiveJournalCorpusIterator::has_more() const {
    return has_more_;
}


bool LiveJournalCorpusIterator::parse_document() {
	static std::string buffer;
    static boost::escaped_list_separator<char> splitter('\\', ';', '\"');

	while (true) {
		std::getline(input, buffer);

		if (! input) {
			return false;
		}

		boost::algorithm::trim_right(buffer);

		boost::tokenizer<boost::escaped_list_separator<char>>
			tokenizer(buffer, splitter);

		auto ptr = tokenizer.begin();

		auto document_id = *ptr++;
		auto text = *ptr++;
		auto nick = *ptr++;
		auto url = *ptr++;
		auto date = *ptr++;

    }
}


void LiveJournalCorpusIterator::increment() {
    has_more_ = parse_document();
}


const Document & LiveJournalCorpusIterator::dereference() const {
    return current_document_;
}
