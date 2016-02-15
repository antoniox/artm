#!/usr/bin/env python

from __future__ import print_function, absolute_import, division

import csv
import sys
import os
import itertools
import operator
import more_itertools

import numpy as np

from collections import defaultdict


def get_dict_reader(sample_stream, delimiter='\t'):
    import csv

    header = csv.reader(sample_stream, delimiter=delimiter).next()

    return csv.DictReader(
        sample_stream,
        fieldnames=header,
        delimiter=delimiter,
    )


def get_multitokens(stream):
    for record in stream:
        document_id = record['document_id']
        yield (  # Document
            document_id,
            (  # Multitoken
                0,  # type
                ('word', int(record['word_id'])),
                # ('nick', int(record['nick_id'])),
            ),
            record['entries'],  # entries
        )


def get_batches(records, batch_size):
    documents = (
        (key, list(group))
        for (key, group) in itertools.groupby(
            records,
            key=operator.itemgetter(0),
        )
    )

    chunked = more_itertools.chunked(documents, batch_size)

    for chunk in chunked:
        documents_count = len(chunk)

        multitokens_counts = defaultdict(int)
        multitokens_indices = defaultdict(dict)

        tokens_counts = defaultdict(lambda: defaultdict(int))
        tokens_indices = defaultdict(lambda: defaultdict(dict))

        for _, multitokens in chunk:
            for _, multitoken, _ in multitokens:
                type = multitoken[0]

                if multitoken not in multitokens_indices[type]:
                    multitoken_index = multitokens_counts[type]

                    multitokens_indices[type][multitoken] = multitoken_index
                    multitokens_counts[type] += 1

                for token in multitoken[1:]:
                    modality, full_index = token

                    if full_index not in tokens_indices[type][modality]:
                        index = tokens_counts[type][modality]
                        tokens_indices[type][modality][full_index] = index
                        tokens_counts[type][modality] += 1

        counters = {}
        for type, count in multitokens_counts.iteritems():
            counters[type] = np.matrix(
                np.zeros((count, documents_count), dtype='int32'),
            )

        for document_index, (_, multitokens) in enumerate(chunk):
            for _, multitoken, entries in multitokens:
                type = multitoken[0]

                multitoken_index = multitokens_indices[type][multitoken]
                counters[type][multitoken_index, document_index] = entries

        yield (
            documents_count,
            counters,
            multitokens_indices,
            tokens_indices,
        )


def get_initial_phi(prefix, topics_count):
    reader = get_dict_reader(
        open(os.path.join(prefix, 'vocabulary'))
    )

    modality_counts = defaultdict(int)

    for record in reader:
        modality_counts[record['modality']] += 1

    # XXX: think about many types
    phi = {
        0: {
            modality: np.matrix(np.random.random((count, topics_count)))
            for modality, count in modality_counts.iteritems()
        }
    }

    normalize_phi(phi)

    return phi


def fill_negligible(matrix, value):
    matrix[matrix < 1e-100] = value
    return matrix


def normalize_matrix(matrix):
    np.divide(
        matrix,
        fill_negligible(matrix.sum(axis=0), 1),
        matrix
    )


def normalize_phi(phi):
    for type, modality_phis in phi.iteritems():
        for modality, modality_phi in modality_phis.iteritems():
            normalize_matrix(modality_phi)


def slice_phi(phi, tokens_indices):
    sliced_phi = defaultdict(dict)

    for type in tokens_indices:
        for modality, indices in tokens_indices[type].iteritems():
            rows = [None] * len(indices)

            for full_index, sliced_index in indices.iteritems():
                rows[sliced_index] = phi[type][modality][full_index, ]

            sliced_phi[type][modality] = np.vstack(rows)

    return sliced_phi


def get_multi_phi(phi, multitokens_indices, tokens_indices, topics_count):
    multi_phi = {}

    for type, multitokens in multitokens_indices.iteritems():
        matrix = np.matrix(
            np.ones((len(multitokens), topics_count), dtype='float64'),
        )

        indices = tokens_indices[type]
        phi_element = phi[type]

        for multitoken, row_index in multitokens.iteritems():
            row = matrix[row_index, ]

            for token in multitoken[1:]:
                modality, full_index = token
                sliced_index = indices[modality][full_index]

                np.multiply(row, phi_element[modality][sliced_index, ], row)

        multi_phi[type] = matrix

    # XXX: should normalize multi_phi?

    return multi_phi


def get_phi_from_multi_phi(
    sliced_phi, multi_phi, multitokens_indices, tokens_indices
):
    zeros = defaultdict(lambda: defaultdict(dict))

    for type, modalities in sliced_phi.iteritems():
        for modality, matrix in modalities.iteritems():
            zeros[type][modality] = np.matrix(
                np.zeros(matrix.shape, dtype='float64'),
            )

    for type, multitokens in multitokens_indices.iteritems():
        multi_phi_element = multi_phi[type]
        phi_element = zeros[type]
        indices = tokens_indices[type]

        for multitoken, row_index in multitokens.iteritems():
            row = multi_phi_element[row_index, ]

            for token in multitoken[1:]:
                modality, full_index = token
                sliced_index = indices[modality][full_index]

                target_row = phi_element[modality][sliced_index, ]

                np.add(target_row, row, target_row)
    #
    # for type, modalities in zeros.iteritems():
    #     for modality, matrix in modalities.iteritems():
    #         for index in xrange(matrix.shape[0]):
    #             row = matrix[index, ]
    #             
    #             if row[row == 0].shape == row.shape:
    #                 raise Exception(type, modality, index)
    #
    return zeros


def update_phi_from_slice(phi, sliced_phi, tokens_indices):

    for type in tokens_indices:
        for modality, indices in tokens_indices[type].iteritems():
            phi_element = phi[type][modality]
            sliced_phi_element = sliced_phi[type][modality]

            for full_index, sliced_index in indices.iteritems():
                phi_element[full_index, ] = sliced_phi_element[sliced_index, ]

    normalize_phi(phi)


def dump_phi(prefix, phi, topics_count, *args):
    path = os.path.join(
        prefix,
        'phi_{}'.format('_'.join('{}'.format(a) for a in args))
    )

    writer = csv.writer(
        open(path, 'w'),
        delimiter='\t'
    )

    writer.writerow(
        ('type', 'modality', 'index') +
        tuple('topic_{}'.format(t) for t in xrange(topics_count))
    )

    for type, modalities in phi.iteritems():
        for modality, matrix in modalities.iteritems():
            rows_count, _ = matrix.shape

            for index in xrange(rows_count):
                writer.writerow(
                    (type, modality, index) +
                    tuple(matrix[index, t] for t in xrange(topics_count))
                )


def main():
    prefix = sys.argv[1]

    batch_size = 100
    if len(sys.argv) > 2:
        batch_size = int(sys.argv[2])

    topics_count = 100
    if len(sys.argv) > 3:
        topics_count = int(sys.argv[3])

    inner_iterations = 10
    if len(sys.argv) > 4:
        inner_iterations = int(sys.argv[4])

    reader = get_dict_reader(
        open(os.path.join(prefix, 'documents'))
    )

    records = get_multitokens(reader)
    batches = get_batches(records, batch_size)

    phi = get_initial_phi(prefix, topics_count)
    # XXX: use only word + nick
    phi[0].pop('date')

    for batch_index, batch in enumerate(batches):
        print(batch_index)

        documents_count, counters, multitokens_indices, tokens_indices = batch

        theta = np.matrix(np.random.random((topics_count, documents_count)))
        normalize_matrix(theta)
        #
        # dump_phi(prefix, sliced_phi, topics_count, batch_index)

        sliced_phi = slice_phi(phi, tokens_indices)

        multi_phi = get_multi_phi(
            sliced_phi, multitokens_indices, tokens_indices, topics_count
        )

        for inner_iteration in xrange(inner_iterations):
            products = {}
            for type in multi_phi:
                products[type] = counters[type] / fill_negligible(
                    multi_phi[type] * theta, 1
                )

            new_multi_phi = {}
            for type in products:
                new_multi_phi[type] = np.multiply(
                    multi_phi[type], products[type] * theta.T
                )
                #
                # normalize_matrix(new_multi_phi[type])

            multiplier = None
            for type in products:
                element = multi_phi[type].T * products[type]
                if multiplier is None:
                    multiplier = element
                else:
                    multiplier += element

            theta = np.multiply(theta, multiplier, theta)
            normalize_matrix(theta)

            multi_phi = new_multi_phi

        sliced_phi = get_phi_from_multi_phi(
            sliced_phi, multi_phi, multitokens_indices, tokens_indices
        )

        update_phi_from_slice(phi, sliced_phi, tokens_indices)

    dump_phi(
        prefix, phi, topics_count,
        topics_count, batch_size, inner_iterations
    )


if __name__ == '__main__':
    main()
