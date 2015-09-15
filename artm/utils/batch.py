from __future__ import absolute_import, print_function, division

import itertools
import numpy as np

__all__ = [
    'Batch',
]


class Batch(object):
    def __init__(self, documents):
        self.documents = list(documents)

    @property
    def token_ids(self):
        token_ids = set()

        for document in self.documents:
            token_ids.update(document.token_ids)

        return sorted(token_ids)

    @property
    def counters(self):
        token_ids = self.token_ids

        token_indices = dict(
            itertools.izip(
                token_ids, itertools.count()
            )
        )

        counters = np.matrix(
            np.zeros((len(token_ids), len(self.documents))),
            dtype='uint32',
        )

        for document_id, document in enumerate(self.documents):
            for token_id, count in itertools.izip(
                document.token_ids, document.counters
            ):
                token_index = token_indices[token_id]
                counters[token_index, document_id] = count

        return counters
