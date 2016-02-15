#!/usr/bin/env python

from __future__ import print_function, absolute_import, division

import os
import sys

from collections import defaultdict


def get_dict_reader(sample_stream, delimiter='\t'):
    import csv

    header = csv.reader(sample_stream, delimiter=delimiter).next()

    return csv.DictReader(
        sample_stream,
        fieldnames=header,
        delimiter=delimiter,
    )


def main():
    prefix = sys.argv[1]
    modality = sys.argv[2]

    sample_stream = open(os.path.join(prefix, 'documents'))
    reader = get_dict_reader(sample_stream)

    reverse_distribution = defaultdict(set)

    for record in reader:
        document_id = record['document_id']
        token_id = record[modality]
        reverse_distribution[token_id].add(document_id)

    distribution = defaultdict(int)

    for token_id, documents in reverse_distribution.iteritems():
        distribution[len(documents)] += 1

    output = open(
        os.path.join(prefix, '{}_distrib'.format(modality)), 'w'
    )

    for count in sorted(distribution):
        line = '{:>7}\t{}\n'.format(count, distribution[count])
        output.write(line)


if __name__ == '__main__':
    main()
