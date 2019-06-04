#!/usr/bin/env python

import sys


def _split_labels(args):
    for arg in args:
        for item in arg.split(','):
            item = item.strip()
            if not item:
                continue
            yield item


def _main():
    expected = ['base', 'time', 'frequencies']
    expected.extend(_split_labels(sys.argv[1:]))
    expected_labels = set(
        '/kvm-info-cap-hyperv-%s' % label for label in expected
    )
    found = set(line.strip() for line in sys.stdin)
    missing = expected_labels.difference(found)
    if missing:
        for label in missing:
            sys.stderr.write('missing: %s\n' % label)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(_main())
