from __future__ import absolute_import, print_function, division

import itertools

import numpy as np


class MultimodalBatch(object):

    def __init__(self, stream):
        documents = stream

        self.documents_count = len(documents)

        self.token_ids = self._infer_token_ids(documents)
        self.token_indices = self._infer_token_indices(self.token_ids)

        self.counters = self._infer_counters(
            documents, self.token_ids, self.token_indices
        )

    def _infer_token_ids(self, documents):
        token_ids = set()

        for document in documents:
            token_ids.update(document.token_ids)

        return sorted(token_ids)

    def _infer_token_indices(self, token_ids):
        return dict(
            itertools.izip(token_ids, itertools.count())
        )

    def _infer_counters(self, documents, token_ids, token_indices):
        counters = np.matrix(
            np.zeros((len(token_ids), len(documents))),
            dtype='uint32',
        )

        for document_id, document in enumerate(documents):
            for token_id, count in itertools.izip(
                document.token_ids, document.counters
            ):
                token_index = token_indices[token_id]
                counters[token_index, document_id] = count

        return counters
