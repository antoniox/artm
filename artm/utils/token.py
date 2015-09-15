from __future__ import absolute_import, print_function, division

__all__ = [
    'Token',
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
