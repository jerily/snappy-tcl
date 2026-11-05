package require snappy 1.0.5

# Snappy works on bytes. Encode text explicitly to preserve Unicode.
set text [string repeat "Hello, κόσμε! 世界\n" 100]
set bytes [encoding convertto utf-8 $text]
set compressed [snappy::compress $bytes]

# string length counts bytes for a byte array; string bytelength measures
# Tcl's string encoding and should not be used for binary buffers.
puts "Original bytes: [string length $bytes]"
puts "Maximum compressed bytes: [snappy::max_compressed_length [string length $bytes]]"
puts "Compressed bytes: [string length $compressed]"
puts "Valid block: [snappy::is_valid_compressed $compressed]"
puts "Stored original length: [snappy::get_uncompressed_length $compressed]"

set restored [encoding convertfrom utf-8 [snappy::uncompress $compressed]]
if {$restored ne $text} {error "Text round trip failed"}
puts "Unicode round trip: OK"
