package require tcltest 2

if {[llength $argv] == 0} {
    puts stderr "Usage: $argv0 library ?tcltest options?"
    exit 1
}
set ::snappy_test_library [file normalize [lindex $argv 0]]
load $::snappy_test_library Snappy
::tcltest::configure -singleproc true -testdir [file dirname [info script]] {*}[lrange $argv 1 end]
exit [::tcltest::runAllTests]
