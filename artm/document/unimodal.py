from __future__ import absolute_import, print_function, division

import collections

import numpy as np


class UnimodalDocument(object):

    def __init__(self, document, modality):
        self.id = document.id

        counter = collections.Counter(
            token for token in document.tokens
            if token.modality == modality
        )

        tokens_count = len(counter)

        token_ids = np.zeros(tokens_count, dtype='uint32')
        counters = np.zeros(tokens_count, dtype='uint32')

        for token_index, token in enumerate(counter):
            token_ids[token_index] = token.id
            counters[token_index] = counter[token]

        self.token_ids = token_ids
        self.counters = counters
