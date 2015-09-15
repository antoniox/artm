from __future__ import absolute_import, print_function, division

import abc

from .document import Document

__all__ = [
    'FileStream',
    'StructureStream',
]


class InputStreamBase(object):
    @abc.abstractmethod
    def __iter__(self):
        pass


class FileStream(InputStreamBase):
    def __init__(self, filename):
        self.filename = filename

    def __iter__(self):
        return iter(open(self.filename))


class StructureStream(InputStreamBase):
    def __init__(self, iterable):
        self.iterable = iterable

    def __iter__(self):
        return iter(self.iterable)
