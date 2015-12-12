from __future__ import absolute_import, print_function, division

import collections

import numpy as np


class MultimodalDocument(object):

    def __init__(self, document):
        self.id = document.id

        counter = collections.Counter(document.tokens)

        modality_counts = collections.defaultdict(int)
        for token in counter:
            modality_counts[token.modality] += 1

        modalities = dict()
        for modality, tokens_count in modality_counts.iteritems():
            modalities[modality] = (
                np.zeros(tokens_count, dtype='uint32'),
                np.zeros(tokens_count, dtype='uint32'),
            )

        modality_counts.clear()
        for token, token_count in counter.iteritems():
            modality = token.modality
            token_index = modality_counts[modality]
            token_ids, counters = modalities[modality]
            token_ids[token_index] = token.id
            counters[token_index] = token_count
            modality_counts[modality] += 1

        self.modalities = modalities
