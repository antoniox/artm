from __future__ import absolute_import, print_function, division

import numpy as np

from .base import ProcessorBase


class UnimodalProcessor(ProcessorBase):

    def __init__(self, topic_count, word_count):
        self.topic_count = topic_count
        self.phi = np.matrix(
            np.random.random((word_count, topic_count)),
            dtype='float64',
        )

        self.normalize(self.phi)

    def process(self, batch_stream, inner_iterations):
        for batch_id, batch in enumerate(batch_stream.as_unimodal):
            counters = batch.counters
            token_ids = batch.token_ids

            phi = self.slice_phi(token_ids)

            theta = np.matrix(
                np.random.random((self.topic_count, batch.documents_count)),
                dtype='float64',
            )

            self.normalize(theta)

            for inner_iteration in xrange(inner_iterations):
                product = counters / self.fill_missing(phi * theta, 1)
                new_phi = np.multiply(phi, product * theta.T)
                new_theta = np.multiply(theta, phi.T * product)

                phi = self.normalize(new_phi)
                theta = self.normalize(new_theta)

            self.update_phi(phi, token_ids)

    def slice_phi(self, token_ids):
        slice = self.phi.take(token_ids, axis=0)
        return self.normalize(slice)

    def update_phi(self, update, token_ids):
        self.phi[token_ids, ] = update
        self.normalize(self.phi)

    @staticmethod
    def fill_missing(matrix, value):
        matrix[matrix < 10e-100] = value
        return matrix

    @staticmethod
    def normalize(matrix):
        np.divide(
            matrix,
            UnimodalProcessor.fill_missing(
                matrix.sum(axis=0), 1
            ),
            matrix
        )

        return matrix
