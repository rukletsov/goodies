
#!/usr/bin/env python

import argparse, os, re, subprocess, sys


# Parse command-line arguments.
parser = argparse.ArgumentParser(
    description="Measure contributors' impact.",
    epilog="File moves do not account in statistics. " \
           "Total changes is the sum of insertions, deletions and binary modifications.")
parser.add_argument("--filter",
                    help="limit statistics to specified folder (e.g. 'docs/')",
                    default="")
args = parser.parse_args()

print "Gathering git stats from {0}".format(os.getcwd())
if args.filter != "":
    print "Excluding stats outside {0}".format(args.filter)

# Get a list of all authors.
# `git log FILTER | grep "^Author: " | sort | uniq -c`
# prints out the list of authors together with the number of commits.
authors = subprocess.check_output(
    "git log {0} | grep \"^Author: \" | sort | uniq -c" \
        .format(args.filter),
    shell=True)

# A dictionary to hold results. Key is author signature, value is a
# tuple <total commits,
#        touched files,
#        insertions,
#        deletions,
#        binary files,
#        total changes>
# NOTE: File renames are excluded and do not count towards insertions and deletions.
stats = {}

# Process authors one by one.
for line in authors.splitlines():
    # Extract total commits and author signature.
    num_commits, _, author_signature = line.split(None, 2)
    author_commits = int(num_commits)

    # We expect at most one email per author.
    author_emails = re.findall(r"<.*>", author_signature)
    if len(author_emails) > 1:
        print "[Warning] Parse error: more than one email per author {0}" \
            .format(author_emails)

    # Get insertions and deletions per author.
    # `git log --author="<author name>" --pretty=tformat: --numstat --find-renames`
    # prints the number of insertions and deletions per file excluding file renames.
    #
    # NOTE: '-' is printed for binary files.
    #
    # NOTE: We do not want renames to be accounted in statistics. However, when a filter
    # is used, changes moved out or moved in to the target dir do account as new changes
    # in `git log FILTER`, because they are not local moves from the target directory's
    # point of view. Hence we opt for manual filtering.
    diff_stats = subprocess.check_output(
        "git log --author='{0}' --pretty=tformat: --numstat --find-renames" \
            .format(author_signature),
        shell=True)

    # Sum up insertions, deletions and count binary files across all diffs.
    author_ins = 0
    author_del = 0
    author_bin = 0
    for diff in diff_stats.splitlines():
        # Some versions of Git include empty lines for commit messages, skip them.
        if len(diff) == 0 or diff.isspace():
            continue

        ins, dele, filename = diff.split(None, 2)

        # Filter out diff if necessary. See the note above why we do it manually.
        if (args.filter != "") and (not filename.startswith(args.filter)):
            continue

        if ins == '-' and dele == '-':
            author_bin += 1
        else:
            author_ins += int(ins)
            author_del += int(dele)

    author_total = author_ins + author_del + author_bin

    # Get the number of touched files per author.
    # `git log --author="<author name>" --pretty=tformat: --name-only FILTER | sort | uniq`
    # returns a list of files touched by the author.
    touched_files = subprocess.check_output(
        "git log --author='{}' --pretty=tformat: --name-only {} | sort | uniq" \
            .format(author_signature, args.filter),
        shell=True)

    author_touched = len(touched_files.splitlines())

    # Add authors stats to the dictionary.
    author_stats = author_commits, author_touched, author_ins, author_del, \
        author_bin, author_total
    stats[author_signature] = author_stats


# Print stats.
print "{:<6} {:<8} {:<10} [{:<10} {:^10} {:>6}] {}" \
        .format("total", "touched", "total", "ins", "del", "bin", "author")
print "{:<6} {:<8} {:<10}" \
        .format("commits", "files", "changes",)

for author_signature, s in sorted(stats.items()):
    print "{:<6} {:<8} {:<10} [{:<10} {:^10} {:>6}] {}" \
        .format(s[0], s[1], s[5], s[2], s[3], s[4], author_signature)
