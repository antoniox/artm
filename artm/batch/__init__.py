from __future__ import absolute_import, print_function, division

from .unimodal import UnimodalBatch
from .multimodal import MultimodalBatch
from .hypergraph import HypergraphBatch


__all__ = [
    'batch_converters',
]

batch_converters = {
    'unimodal': UnimodalBatch,
    'multimodal': MultimodalBatch,
    'hypergraph': HypergraphBatch,
}
