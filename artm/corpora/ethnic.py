from __future__ import absolute_import, print_function, division

import csv
import itertools

from .base import CorpusBase
from ..utils import Document, Token


class EthnicCorpus(CorpusBase):

    def parse_input_stream(self):
        reader = csv.reader(
            self.input_stream, delimiter=';',
        )

        reader.next()

        for line in reader:
            document_id = line[0]
            tokens = self.tokenize(line)
            yield Document(tokens, document_id)

    def tokenize(self, line):
        _, text, nick, _, date, _, _ = line

        iterable = itertools.starmap(
            Token,
            itertools.chain(
                ((0, word) for word in text.split()),
                [(1, nick), (2, date)],
            )
        )

        for token in iterable:
            token_id = self.add_token(token)
            if token_id is not None:
                token.id = token_id
                yield token
