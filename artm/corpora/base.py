from __future__ import absolute_import, print_function, division

import abc

from itertools import imap
from more_itertools import chunked

from ..document import document_converters
from ..batch import batch_converters
from ..utils import MutableVocabulary


class CorpusBase(object):

    __metaclass__ = abc.ABCMeta

    def __init__(self, input_stream, mode, vocabulary=None):
        self.input_stream = input_stream
        self.mode = mode
        self.vocabulary = vocabulary or MutableVocabulary()
        self.cached_document_stream = None

    @abc.abstractmethod
    def parse_input_stream(self):
        pass

    @property
    def document_stream(self):
        document_stream = self.cached_document_stream
        if self.cached_document_stream is None:
            document_converter = document_converters[self.mode]
            document_stream = imap(
                document_converter, self.parse_input_stream()
            )

        return document_stream

    def cache(self):
        if self.cached_document_stream is None:
            self.cached_document_stream = list(self.document_stream)

    def clear_cache(self):
        self.cached_document_stream = None

    def batch_stream(self, batch_size):
        batch_converter = batch_converters[self.mode]
        return imap(
            batch_converter,
            chunked(self.document_stream, batch_size),
        )
