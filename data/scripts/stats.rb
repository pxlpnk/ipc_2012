#!/usr/bin/env ruby
# Add statistical measures to Enumerable
#
# Copyright 2011 Trey Kinkead
#

module Enumerable

  def mean
    sum = self.inject(0) { |sum,x| sum+x}
    sum.to_f / self.size
  end

  def median
    self.size%2==0 ?
      self.sort[self.size/2-1,2].mean : # even
      self.sort[self.size/2].to_f       # odd
  end

  def population_standard_deviation
    mean = self.mean
    sum_diff_squared=self.inject(0) { |accum,x| accum+((x-mean)**2)}
    Math.sqrt(sum_diff_squared/self.size)
  end

  # Sample Variance
  def variance
    mean = self.mean
    sum_diff_squared=self.inject(0) { |accum,x| accum+((x-mean)**2)}
    sum_diff_squared/(self.size-1)
  end

  # Sample Standard Deviation
  def standard_deviation
    Math.sqrt(variance)
  end

  def mean_absolute_deviation
    mean = self.mean
    sum_abs_diff=self.inject(0) { |accum,x| accum+((x-mean).abs)}
    sum_abs_diff/(self.size)
  end

  # computed by NIST method:
  # http://www.itl.nist.gov/div898/handbook/prc/section2/prc252.htm
  #
  # I believe this corresponds to R's type=6
  # http://stat.ethz.ch/R-manual/R-patched/library/stats/html/quantile.html
  #
  def percentile(p)
    sorted = self.sort
    n = p*(sorted.size+1)
    # alternate method used by Excel:
    # n = p*(sorted.size-1)+1
    k = n.floor
    d = n-k
    if (k==0.0)
      sorted.first
    elsif (k>=sorted.size)
      sorted.last
    else
      # note that our indexes are 0-based, rather than 1-based
      i=k-1
      sorted[i]+d*(sorted[i+1]-sorted[i])
    end
  end
end




def aggregate(data, fn)
  computed = []
  data.each do |k,dat|
    computed << dat.send(fn)
  end
  computed
end


def usage
  "usage: ./stats.rb <list of files>"
end

def prepare_data( files )
  @hash = Hash.new{|h, k| h[k] = []}
  file_counter = 0
  files.each do |file|
    CSV.foreach(file) do |threads,time|
      @hash[threads.to_i][file_counter] = time.to_f
    end
    file_counter += 1
  end

  @hash
end


if __FILE__ == $0
  require 'csv'

  @files = []
  ARGV.each do|arg|
    @files << arg if File.exists?(arg)
  end

  if @files.empty?
    puts usage
    exit
  end

  data = prepare_data(@files)

  functions = [:max, :min, :mean, :median]
  output = {}

  functions.each do |fn|
    output[fn] = aggregate(data, fn)
  end

  p output

end
