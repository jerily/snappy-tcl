package require snappy 1.0.5

# Include every possible byte value, including NUL and non-ASCII bytes.
set values {}
for {set i 0} {$i < 256} {incr i} {lappend values $i}
set bytes [binary format c* $values]
set compressed [snappy::compress $bytes]
set restored [snappy::uncompress $compressed]
if {$restored ne $bytes} {error "Binary round trip failed"}

puts "Binary round trip: [string length $restored] bytes, OK"
puts "First 16 bytes (hex): [binary encode hex [string range $restored 0 15]]"

# Truncated input raises a Tcl error; never use partially decoded output.
set truncated [string range $compressed 0 end-1]
puts "Truncated block valid: [snappy::is_valid_compressed $truncated]"
if {[catch {snappy::uncompress $truncated} message]} {
    puts "Decompression error: $message"
} else {
    error "Expected truncated input to fail"
}
