from __future__ import absolute_import, print_function, division

from .generic import GenericDocument
from .unimodal import UnimodalDocument
from .multimodal import MultimodalDocument
from .hypergraph import HypergraphDocument


__all__ = [
    'GenericDocument',
    'document_converters',
]

document_converters = {
    'unimodal': UnimodalDocument,
    'multimodal': MultimodalDocument,
    'hypergraph': HypergraphDocument,
}
