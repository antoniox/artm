#!/usr/bin/env python

from __future__ import print_function, absolute_import, division

import csv
import sys
import os

from collections import defaultdict


def parse_thresholds(line):
    split = line.split()
    subsplit = [e.split('=') for e in split]

    thresholds = dict()

    for modality, threshold in subsplit:
        rule = eval(threshold)
        if isinstance(rule, int):
            rule = (rule, sys.maxint)
        elif not isinstance(rule, tuple) or len(rule) != 2:
            raise ValueError()

        thresholds[modality] = rule

    return thresholds


def get_shrink_prefix(prefix, thresholds):
    shrink_prefix = [prefix]
    for threshold in sorted(thresholds):
        shrink_prefix.append(threshold)
        value = thresholds[threshold]
        if value[1] == sys.maxint:
            value = str(value[0])
        else:
            value = '{}_{}'.format(*value)
        shrink_prefix.append(value)

    return '_'.join(shrink_prefix)


def get_dict_reader(sample_stream, delimiter='\t'):
    import csv

    header = csv.reader(sample_stream, delimiter=delimiter).next()

    return csv.DictReader(
        sample_stream,
        fieldnames=header,
        delimiter=delimiter,
    )


def get_document_counts(prefix):
    stream = open(os.path.join(prefix, 'documents'))
    reader = get_dict_reader(stream)

    modalities = ('nick_id', 'date_id', 'word_id')
    document_counts = defaultdict(lambda: defaultdict(set))

    for record in reader:
        for modality in modalities:
            document_counts[modality][record[modality]].add(record['document_id'])

    return document_counts


def shrink_document_counts(document_counts, thresholds):
    shrunk_document_counts = defaultdict(set)

    for modality, tokens in document_counts.iteritems():
        for token_id, documents in tokens.iteritems():
            rule = thresholds.get(modality)
            if rule is None or rule[0] <= len(documents) < rule[1]:
                shrunk_document_counts[modality].add(token_id)

    return shrunk_document_counts


def get_vocabulary_remapping(shrunk_document_counts):
    remapping = defaultdict(dict)

    for modality, tokens in shrunk_document_counts.iteritems():
        for new_token_id, token_id in enumerate(tokens):
            remapping[modality][token_id] = str(new_token_id)

    return remapping


def remap_vocabulary(prefix, shrink_prefix, vocabulary_remapping):
    reader = get_dict_reader(open(os.path.join(prefix, 'vocabulary')))

    if not os.path.exists(shrink_prefix):
        os.mkdir(shrink_prefix)

    writer = csv.writer(open(os.path.join(shrink_prefix, 'vocabulary'), 'w'), delimiter='\t')
    writer.writerow(
        ('modality', 'token', 'id'),
    )

    for record in reader:
        modality = record['modality']
        token = record['token']
        id = record['id']

        new_id = vocabulary_remapping[modality + '_id'].get(id)

        if new_id is not None:
            writer.writerow(
                (modality, token, new_id),
            )


def remap_documents(prefix, shrink_prefix, vocabulary_remapping):
    reader = get_dict_reader(open(os.path.join(prefix, 'documents')))

    if not os.path.exists(shrink_prefix):
        os.mkdir(shrink_prefix)

    writer = csv.writer(open(os.path.join(shrink_prefix, 'documents'), 'w'), delimiter='\t')
    writer.writerow(
        ('document_id', 'date_id', 'nick_id', 'word_id', 'entries'),
    )

    for record in reader:
        document_id = record['document_id']

        date_id = record['date_id']
        nick_id = record['nick_id']
        word_id = record['word_id']

        entries = record['entries']

        new_date_id = vocabulary_remapping['date_id'].get(date_id)
        new_nick_id = vocabulary_remapping['nick_id'].get(nick_id)
        new_word_id = vocabulary_remapping['word_id'].get(word_id)

        if new_date_id and new_nick_id and new_word_id:
            writer.writerow(
                (document_id, new_date_id, new_nick_id, new_word_id, entries)
            )


def main():
    prefix = sys.argv[1]
    thresholds = parse_thresholds(' '.join(sys.argv[2:]))

    shrink_prefix = get_shrink_prefix(prefix, thresholds)

    document_counts = get_document_counts(prefix)
    shrunk_document_counts = shrink_document_counts(document_counts, thresholds)
    del document_counts

    vocabulary_remapping = get_vocabulary_remapping(shrunk_document_counts)

    remap_vocabulary(prefix, shrink_prefix, vocabulary_remapping)
    remap_documents(prefix, shrink_prefix, vocabulary_remapping)


if __name__ == '__main__':
    main()
