from __future__ import print_function, absolute_import, division


class HypergraphDocument(object):

    def __init__(self, document):
        self.id = document.id
        self.tokens = document.tokens
