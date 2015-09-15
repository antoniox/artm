from __future__ import absolute_import, print_function, division

import collections


__all__ = [
    'Vocabulary',
]


class Vocabulary(dict):

    def __init__(self, data=None):
        data = data or dict()
        self.update(data)

        candidate_token_ids = collections.defaultdict(int)

        # suppose ids are contigious
        for token in self:
            candidate_token_ids[token.modality] += 1

        self.candidate_token_ids = candidate_token_ids

    def has_token(self, token):
        return token in self

    def add_token(self, token):
        modality = token.modality
        self[token] = self.candidate_token_ids[modality]
        self.candidate_token_ids[modality] += 1

    def token_id(self, token):
        return self[token]

    def reduce(self, token_ids):
        data = dict()

        for token, token_id in self.iteritems():
            if token_id in token_ids:
                data[token] = token_id

        return Vocabulary(data)

    def reduce_by_inverted_index(
        self, inverted_index,
        lower_bound=None, upper_bound=None
    ):
        lower_bound = lower_bound or 0
        upper_bound = upper_bound or len(self)

        token_ids = frozenset([
            token_id for token_id in inverted_index
            if lower_bound <= len(inverted_index[token_id]) <= upper_bound
        ])

        return self.reduce(token_ids)
