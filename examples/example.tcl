package require snappy

set data "hello world"
set bytes [::snappy::compress $data]
puts is_valid_compressed=[::snappy::is_valid_compressed $bytes]
set uncompressed [::snappy::uncompress $bytes]
puts $uncompressed
