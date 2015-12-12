from __future__ import absolute_import, print_function, division

__all__ = [
    'Token',
    'MultiToken',
]


class Token(object):

    __slots__ = (
        'modality',
        'value',
        'id',
    )

    def __init__(self, modality, value, id=None):
        self.modality = modality
        self.value = value
        self.id = id

    def __eq__(self, other):
        return (
            self.modality == other.modality and
            self.value == other.value
        )

    def __hash__(self):
        return hash((self.modality, self.value))


class MultiToken(object):

    __slots__ = (
        'type',
        'tokens',
    )

    def __init__(self, type, tokens):
        self.type = type
        self.tokens = tokens

    def __eq__(self, other):
        return self.type == other.type and self.tokens == other.tokens

    def __hash__(self):
        return hash((self.type, self.tokens))

    @property
    def id(self):
        return hash(self) % 2 ** 32
