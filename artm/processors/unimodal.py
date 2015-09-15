from __future__ import absolute_import, print_function, division

import numpy as np

from .base import ProcessorBase


class UnimodalProcessor(ProcessorBase):
    def __init__(self, topics, words):
        self.topics = topics
        self.phi = np.matrix(
            np.random.random((words, topics)),
            dtype='float64',
        )

        self.processed = 0

    def process(self, batch_stream, inner_iterations, outer_iterations):
        log = open('log', 'wr')
        for _ in xrange(outer_iterations):
            for batch_id, batch in enumerate(batch_stream):
                self.processed += 1
                log.write(str(batch_id) + '\n')

                counters = batch.counters
                token_ids = np.asarray(
                    counters[:, 0], dtype='uint32'
                ).reshape(-1)

                phi = self.slice_phi(token_ids)
                self.normalize(phi, axis=0)

                theta = np.matrix(
                    np.random.random((self.topics, len(batch.documents))),
                    dtype='float64',
                )

                self.normalize(theta, axis=0)

                for _ in xrange(inner_iterations):
                    product = counters / self.fill_missing(phi * theta, 1)
                    new_phi = np.multiply(phi, product * theta.T)
                    new_theta = np.multiply(theta, phi.T * product)

                    self.normalize(new_phi, axis=0)
                    self.normalize(new_theta, axis=0)

                    phi = new_phi
                    theta = new_theta

                self.update_phi(phi, token_ids)

    def slice_phi(self, token_ids):
        return self.phi.take(token_ids, axis=0)

    def update_phi(self, update, token_ids):
        self.phi[token_ids, ] = update

    @staticmethod
    def fill_missing(matrix, value):
        matrix[matrix < 10e-100] = value
        return matrix

    @staticmethod
    def normalize(matrix, axis):
        np.divide(
            matrix,
            UnimodalProcessor.fill_missing(
                matrix.sum(axis=axis), 1
            ),
            matrix
        )
