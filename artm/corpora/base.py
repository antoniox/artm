from __future__ import absolute_import, print_function, division

import abc
import collections
import itertools
import operator

import more_itertools

from ..utils import Batch, Vocabulary


class CorpusBase(object):

    __metaclass__ = abc.ABCMeta

    attributes = {
        'unimodal': 'as_unimodal',
        'multimodal': 'as_multimodal',
        'hypergraph': 'as_hypergraph',
    }

    def __init__(self, input_stream, vocabulary=None, mutate_vocabulary=False):
        self.input_stream = input_stream
        self.vocabulary = vocabulary or Vocabulary()
        self.mutate_vocabulary = mutate_vocabulary

    @abc.abstractmethod
    def parse_input_stream(self):
        pass

    def document_stream(self, mode='unimodal'):
        document_stream = self.parse_input_stream()
        attribute_name = self.attributes[mode]
        return itertools.imap(
            operator.attrgetter(attribute_name),
            document_stream
        )

    def batch_stream(self, batch_size, mode='unimodal'):
        return itertools.imap(
            Batch,
            more_itertools.chunked(self.document_stream(mode), batch_size)
        )

    def add_token(self, token):
        vocabulary = self.vocabulary
        if not self.mutate_vocabulary and not vocabulary.has_token(token):
            return None

        if not vocabulary.has_token(token):
            vocabulary.add_token(token)

        return vocabulary.token_id(token)

    @property
    def inversed_index(self):
        inversed_index = collections.defaultdict(list)

        for document in self.document_stream:
            for token_id in document.token_ids:
                inversed_index[token_id].append(document.id)

        return inversed_index
