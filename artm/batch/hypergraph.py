from __future__ import print_function, absolute_import, division

import itertools
import numpy as np

from collections import Counter
from recordtype import recordtype


BatchInfo = recordtype(
    'BatchInfo',
    ['counters', 'delta', 'token_counts', 'multitoken_counts', 'multitoken_tokens'],
)


class HypergraphBatch(object):

    def __init__(self, documents):
        self.document_count = sum(1 for _ in documents)
        self.documents = documents

    @property
    def info(self):
        # multitoken_counter: multitoken -> entries
        multitoken_counter = Counter()

        for document in self.documents:
            multitoken_counter.update(document.tokens)

        multitokens_entries = (
            (mt.type, mt.id, mt, e)
            for mt, e in multitoken_counter.iteritems()
        )

        ordered_multitokens_entries = sorted(
            multitokens_entries,
            key=lambda (type, id, mt, e): (type, id)
        )

        # token_counter: (type, modality) -> entries
        token_counter = Counter()

        # multitoken_tokens: ([type], [multitoken_id], [modality], [token_id])
        multitoken_tokens = ([], [], [], [])

        for (type, id, multitoken, _) in ordered_multitokens_entries:
            multitoken_id = id

            for token in sorted(
                multitoken.tokens,
                key=lambda t: t.modality,
            ):
                modality = token.modality
                token_id = token.id

                token_counter.update([(type, modality)])

                multitoken_tokens[0].append(type)
                multitoken_tokens[1].append(multitoken_id)
                multitoken_tokens[2].append(modality)
                multitoken_tokens[3].append(token_id)

        multitoken_tokens = (
            np.array(multitoken_tokens[0], dtype='int32'),
            np.array(multitoken_tokens[1], dtype='int32'),
            multitoken_tokens[2],
            np.array(multitoken_tokens[3], dtype='int32'),
        )

        # multitoken_counts: ([type], [entries])
        multitoken_counts = ([], [])

        for type, group in itertools.groupby(
            ordered_multitokens_entries,
            key=lambda r: r[0],
        ):
            entries = sum(e for (_, _, _, e) in group)
            multitoken_counts[0].append(type)
            multitoken_counts[1].append(entries)

        multitoken_counts = tuple(
            np.array(e, dtype='int32')
            for e in multitoken_counts
        )

        # token_counts: ([type], [modality], [entries])
        token_counts = ([], [], [])

        for type, modality in sorted(token_counter):
            token_counts[0].append(type)
            token_counts[1].append(modality)
            token_counts[2].append(token_counter[(type, modality)])

        token_counts = tuple(
            np.array(e, dtype='int32') for e in token_counts
        )

        total_multitoken_count = sum(multitoken_counts[1])

        print("counters: ", total_multitoken_count, self.document_count)

        # F matrix: (type, multitoken_id) x document -> entries
        counters = np.matrix(
            np.zeros((total_multitoken_count, self.document_count)),
            dtype='int32',
        )

        multitoken_indices = {
            type_id: index
            for index, (type_id, _) in enumerate(
                itertools.groupby(
                    ordered_multitokens_entries,
                    key=lambda (type, id, mt, e): (type, id),
                )
            )
        }

        for document_index, document in enumerate(self.documents):
            counter = Counter(document.tokens)

            for multitoken, entries in counter.iteritems():
                type = multitoken.type
                id = multitoken.id
                multitoken_index = multitoken_indices[(type, id)]

                counters[multitoken_index, document_index] = entries

        total_token_count = sum(token_counts[2])

        print("delta: ", total_token_count, total_multitoken_count)

        # delta matrix: (type, modality, token_id) x (type, multitoken_id)
        delta = np.matrix(
            np.zeros((total_token_count, total_multitoken_count)),
            dtype='int32',
        )

        return BatchInfo(
            counters=counters,
            delta=delta,
            token_counts=token_counts,
            multitoken_counts=multitoken_counts,
            multitoken_tokens=multitoken_tokens,
        )
