#!/usr/bin/perl
#
# Simple profiler
#   Parse emu6800 output to count executions per address.
#
#   emu6800 -d a.bin a.map |& ./prof.pl 
#

use strict;
use warnings;

my %count;
while (<>) {
    if (/^([0-9A-Fa-f]{4})\b/) {
        $count{uc $1}++;
    }
}

foreach my $hex (sort keys %count) {
    printf "%s\t%d\n", $hex, $count{$hex};
}
