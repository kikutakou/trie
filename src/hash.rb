#!/usr/local/bin/ruby


require 'time'
require 'objspace'




h = Hash.new

start_time = Time.now


File.foreach(ARGV[0]){ |line|
  line.chomp.split.each{ |w|
    h[w] = 1
  }
}

puts "construct #{Time.now - start_time}s"
memsize = ObjectSpace.memsize_of(h)
start_time = Time.now

File.foreach(ARGV[0]){ |line|
  line.chomp.split.each{ |w|
    puts "#{w} not found" unless h[w]
  }
}

puts "search #{Time.now - start_time}s"

puts "memory #{memsize / 1024 / 1024} MB"

