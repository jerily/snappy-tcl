# Usage: tclsh examples/file.tcl compress|uncompress input output
# This stores one raw Snappy block, not the Snappy framed stream format.
package require snappy 1.0.5

if {[llength $argv] != 3 || [lindex $argv 0] ni {compress uncompress}} {
    puts stderr "Usage: $argv0 compress|uncompress input output"
    exit 1
}
lassign $argv operation input output
set channel [open $input rb]
try {
    set bytes [read $channel]
} finally {
    close $channel
}

set result [snappy::$operation $bytes]
# Refuse to overwrite an existing file, including the input file.
set channel [open $output {WRONLY CREAT EXCL}]
try {
    fconfigure $channel -translation binary
    puts -nonewline $channel $result
} finally {
    close $channel
}
puts "$operation: [string length $bytes] -> [string length $result] bytes ($output)"
