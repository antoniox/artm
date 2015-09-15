from __future__ import absolute_import, print_function, division

import collections

import numpy as np


__all__ = [
    'Document',
]


class Document(object):

    def __init__(self, tokens, id):
        self.tokens = tokens
        self.id = id

    @property
    def as_unimodal(self):
        counter = collections.Counter(
            token for token in iter(self.tokens)
            if token.modality == 0
        )

        tokens_count = len(counter)

        token_ids = np.zeros(tokens_count, dtype='int32')
        counters = np.zeros(tokens_count, dtype='int32')

        for token_index, token in enumerate(counter):
            token_ids[token_index] = token.id
            counters[token_index] = counter[token]

        return token_ids, counters

    @property
    def as_multimodal(self):
        counter = collections.Counter(self.tokens)

        modality_counts = collections.defaultdict(int)
        for token in counter:
            modality_counts[token.modality] += 1

        modalities = dict()
        for modality, tokens_count in modality_counts.iteritems():
            modalities[modality] = (
                np.zeros(tokens_count, dtype='int32'),
                np.zeros(tokens_count, dtype='int32'),
            )

        modality_counts.clear()
        for token, token_count in counter.iteritems():
            modality = token.modality
            token_index = modality_counts[modality]
            token_ids, counters = modalities[modality]
            token_ids[token_index] = token.id
            counters[token_index] = token_count
            modality_counts[modality] += 1

        return modalities
