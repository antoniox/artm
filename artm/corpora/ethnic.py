from __future__ import absolute_import, print_function, division

import csv

from .base import CorpusBase
from ..document import GenericDocument
from ..utils import Token, MultiToken


class EthnicCorpus(CorpusBase):

    def parse_input_stream(self):
        reader = csv.reader(
            self.input_stream, delimiter=';',
        )

        reader.next()

        for line in reader:
            document_id = line[0]
            tokens = list(self.tokenize(line))
            yield GenericDocument(tokens, document_id)

    def tokenize(self, line):
        _, text, nick, _, date, _, _ = line

        user_token = self.vocabulary.add_token(
            Token(1, nick)
        )

        if user_token.id is None:
            return

        date_token = self.vocabulary.add_token(
            Token(2, date)
        )

        if date_token.id is None:
            return

        for word in text.split():
            word_token = self.vocabulary.add_token(
                Token(0, word)
            )

            if word_token.id is not None:
                yield MultiToken(0, (word_token, user_token))
