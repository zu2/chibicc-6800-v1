#!/usr/bin/perl
#
# Simple profiler
#   Merge prof.pl results with map file data
#
#   emu6800 -d a.bin a.map |& ./prof.pl | ./pm.pl a.map
#
#
use strict;
use warnings;

# Check if file2 name is provided as a command-line argument
die "Usage: $0 file2.txt\n" unless @ARGV == 1;
my $file2 = $ARGV[0];

# Read file2 and build a hash: hex number => last field
my %hex_to_info;
open my $fh2, '<', $file2 or die "Cannot open $file2: $!";
while (<$fh2>) {
    chomp;
    next if /^\s*$/; # Skip empty lines
    my @fields = split /\s+/, $_; # Split by spaces or tabs
    my $hex = $fields[0];
    my $info = $fields[-1]; # Get the last field
    $hex_to_info{$hex} = $info;
}
close $fh2;

# Read file1 from standard input, merge with file2 info if exists, and print result
while (<STDIN>) {
    chomp;
    next if /^\s*$/; # Skip empty lines
    my @fields = split /\s+/, $_; # Split by spaces or tabs
    my $hex = $fields[0];
    print join(" ", @fields);
    if (exists $hex_to_info{$hex}) {
        print "\t$hex_to_info{$hex}";
    }
    print "\n";
}

