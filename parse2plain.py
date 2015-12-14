#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function, absolute_import, division

import re
import csv
import sys
import os
import itertools

from collections import defaultdict, Counter

PATTERN = re.compile(r'[а-я]{3,}$')


def main():
    sample = sys.argv[1]

    limit = None
    if len(sys.argv) == 3:
        limit = int(sys.argv[2])

    sample_stream = open(sample)
    if limit is not None:
        sample_stream = itertools.islice(sample_stream, limit + 1)

    reader = get_dict_reader(sample_stream, ';')

    full_name = sample.split('.')[0]
    base_name, original_limit = full_name.rsplit('_', 1)

    if limit is not None:
        full_name = '{}_{}'.format(base_name, limit)

    if not os.path.exists(full_name):
        os.mkdir(full_name)

    document_stream = open(os.path.join(full_name, 'documents'), 'w')
    document_writer = csv.writer(document_stream, delimiter='\t')
    document_writer.writerow(('document_id', 'date_id', 'nick_id', 'word_id', 'entries'))

    vocabulary_stream = open(os.path.join(full_name, 'vocabulary'), 'w')
    vocabulary_writer = csv.writer(vocabulary_stream, delimiter='\t')
    vocabulary_writer.writerow(('modality', 'token', 'id'))

    candidate_ids = defaultdict(int)
    vocabulary = defaultdict(dict)

    for record in reader:
        document_id = record['ID']
        nick = record['Nick']
        date = record['Date']

        text = record['lematized text'].decode('cp1251').encode('utf8')
        words = text.split()

        good_words = [
            w for w in words
            if PATTERN.match(w)
        ]

        word_counts = Counter(words)

        if not word_counts:
            continue

        nick_id = get_id(vocabulary, candidate_ids, 'nick', nick)
        date_id = get_id(vocabulary, candidate_ids, 'date', date)

        for word, entries in word_counts.iteritems():
            word_id = get_id(vocabulary, candidate_ids, 'word', word)

            document_writer.writerow(
                (document_id, date_id, nick_id, word_id, entries)
            )

    for modality, tokens in vocabulary.iteritems():
        for token, id in tokens.iteritems():
            vocabulary_writer.writerow(
                (modality, token, id)
            )


def get_dict_reader(sample_stream, delimiter='\t'):
    import csv

    header = csv.reader(sample_stream, delimiter=delimiter).next()

    return csv.DictReader(
        sample_stream,
        fieldnames=header,
        delimiter=delimiter,
    )


def get_id(vocabulary, candidate_ids, modality, value):
    id = vocabulary[modality].get(value)

    if id is not None:
        return id

    id = candidate_ids[modality]
    candidate_ids[modality] += 1

    vocabulary[modality][value] = id

    return id


if __name__ == '__main__':
    main()
