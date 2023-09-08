package require snappy

set data "hello world"
set max_compressed_length [::snappy::max_compressed_length [string bytelength $data]]
puts max_compressed_length=$max_compressed_length

# compress
set bytes [::snappy::compress $data]

set actual_compressed_length [string bytelength $bytes]
puts actual_compressed_length=$actual_compressed_length
puts is_valid_compressed=[::snappy::is_valid_compressed $bytes]
set uncompressed_length [::snappy::get_uncompressed_length $bytes]
puts uncompressed_length=$uncompressed_length

# decompress
set uncompressed [::snappy::uncompress $bytes]
puts uncompressed=$uncompressed
