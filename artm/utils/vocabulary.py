from __future__ import absolute_import, print_function, division

import abc
import collections


__all__ = [
    'MutableVocabulary',
    'ImmutableVocabulary',
]


class VocabularyBase(object):

    __metaclass__ = abc.ABCMeta

    def __init__(self, data=None):
        data = data or dict()
        self.data = collections.defaultdict(dict, data)
        self.candidate_token_ids = collections.defaultdict(int)

        for modality, tokens in self.data.iteritems():
            self.candidate_token_ids[modality] = max(
                token.id for token in tokens
            ) + 1

    def has_token(self, token):
        return token in self.data[token.modality]

    @abc.abstractmethod
    def add_token(self, token):
        pass

    def token_count(self, modality=None):
        pass

    def suppress_modalities(self, modalities):
        pass


class MutableVocabulary(VocabularyBase):

    def add_token(self, token):
        possible_token_id = self.data[token.modality].get(token)

        if possible_token_id is not None:
            token.id = possible_token_id
            return token

        token_id = self.candidate_token_ids[token.modality]
        if token_id > 20000:
            token.id = None
            return token

        token.id = token_id
        self.data[token.modality][token] = token_id
        self.candidate_token_ids[token.modality] += 1
        return token


class ImmutableVocabulary(VocabularyBase):

    def add_token(self, token):
        return None
