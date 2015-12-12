from __future__ import absolute_import, print_function, division

import abc


__all__ = [
    'FileStream',
    'ParsedStream',
]


class InputStreamBase(object):

    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def __iter__(self):
        pass


class FileStream(InputStreamBase):

    def __init__(self, filename):
        self.filename = filename

    def __iter__(self):
        return open(self.filename)


class ParsedStream(InputStreamBase):

    def __init__(self, collection):
        self.collection = collection

    def __iter__(self):
        return iter(self.collection)
