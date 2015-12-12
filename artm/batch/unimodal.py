from __future__ import absolute_import, print_function, division

import itertools

import numpy as np


class UnimodalBatch(object):

    def __init__(self, stream):
        documents = stream
        documents_count = sum(1 for _ in stream)

        self.documents_count = documents_count
        self.token_ids = self._infer_token_ids(documents)
        self.counters = self._infer_counters(
            documents, documents_count, self.token_ids
        )

    def _infer_token_ids(self, documents):
        token_ids = set()

        for document in documents:
            token_ids.update(document.token_ids)

        return list(token_ids)

    def _infer_counters(self, documents, documents_count, token_ids):
        token_indices = dict(
            itertools.izip(token_ids, itertools.count())
        )

        counters = np.matrix(
            np.zeros((len(token_ids), documents_count)),
            dtype='uint32',
        )

        for document_id, document in enumerate(documents):
            for token_id, count in itertools.izip(
                document.token_ids, document.counters
            ):
                token_index = token_indices[token_id]
                counters[token_index, document_id] = count

        return counters
