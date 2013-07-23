#!/usr/bin/env ruby
# encoding: utf-8
#
# 'bcdfghjklmnpqrstvz'


HARD_PLOSIVES   = [ 'k', 'p', 't' ]
SOFT_PLOSIVES   = [ 'g', 'b', 'd' ]
NASALS          = [ 'n' ]
LABIALS         = [ 'm' ]
LIQUIDS         = [ 'l', 'r' ]

# c prounounched "ch" as in "choice", x as sh in "she"
HARD_FRICATIVES = [ 'c', 'f', 'x', 'h' ]
# J as "j" in "jump", 
SOFT_FRICATIVES = [ 'j', 'v',  nil, nil]



SEMIVOWELS      = [ 'j', 'w', 'y' ]

LANG_CONSONANTS = HARD_PLOSIVES + HARD_FRICATIVES + NASALS + [ 'l' ]

VSYLLABLES = [
  'aa', 'ee', 'yi', 'oo', 'wu'
]

#  č = c = ch (tʃ), š = x = sh (ʃ)  ʃ  ʒ
#  č = c = ch (tʃ), š = x = sh (ʃ)
#  ž = j = (ʒ) ,      ʒ      š, č, ř, c, j, ď, ť, ň


CSYLLABLES = [
# 'fa', 'fe', 'fi', 'fo', 'fu',
# 'va', 've', 'vi', 'vo', 'vu', 
# 'ba', 'be', 'bi', 'bo', 'bu',
# 'da', 'de', 'di', 'do', 'du',
# 'ga', 'ge', 'gi', 'go', 'gu',
# 'za', 'ze', 'zi', 'zo', 'zu',
  'ča', 'če', 'či', 'čo', 'ču',
  'ha', 'he', 'hi', 'ho', 'hu',
  'ka', 'ke', 'ki', 'ko', 'ku',
  'la', 'le', 'li', 'lo', 'lu',
  'ma', 'me', 'mi', 'mo', 'mu',
  'na', 'ne', 'ni', 'no', 'nu',
  'pa', 'pe', 'pi', 'po', 'pu',
  'sa', 'se', 'si', 'so', 'su',
  'ta', 'te', 'ti', 'to', 'tu',
]

NSYLLABLES = [ 'nn' ]

ASSONANCE =  {
  'ka' => 'ga', 'ke' => 'ge', 'ki' => 'gi', 'ko' => 'go', 'ku' => 'gu',
  'pa' => 'ba', 'pe' => 'be', 'pi' => 'bi', 'po' => 'bo', 'pu' => 'bu',
  'sa' => 'za', 'se' => 'ze', 'si' => 'zi', 'so' => 'zo', 'su' => 'zu',
  'ta' => 'da', 'te' => 'de', 'ti' => 'di', 'to' => 'so', 'tu' => 'du',
  'na' => 'ma', 'ne' => 'me', 'ni' => 'mi', 'no' => 'mo', 'nu' => 'mu',
  'ma' => 'na', 'me' => 'ne', 'mi' => 'ni', 'mo' => 'no', 'mu' => 'nu',
  'ha' => 'va', 'he' => 've', 'hi' => 'vi', 'ho' => 'vo', 'hu' => 'vu',
}

VASSONANCE = {
  'aa' => 'ngaa', 'ee' => 'ngee', 'yi' => 'ngii', 'oo' => 'ngoo', 'wu' => 'nguu'
}

def assonance(syl)
  res = ASSONANCE[syl]
  return res if res
  return 'nm' + syl
end

def vassonance(syl)
  res = VASSONANCE[syl]
  return res if res
  return 'ng' + syl
end


words = []

CSYLLABLES.each do | cs1 |
  CSYLLABLES.each do | cs2 |
    words << cs1 + cs2
  end
  VSYLLABLES.each do | vs1 |
    next if cs1[1] == vs1[0]
    words << cs1 + vs1
  end
  # words << cs1 + 'nn'
end

VSYLLABLES.each do | vs1 |
  CSYLLABLES.each do | cs2 |
    words << vs1 + cs2
  end
  VSYLLABLES.each do | vs2 |
    next if vs1    == vs2
    next if vs1[1] == vs2[0]
    words << vs1 + vs2
  end
  # words << vs1 + 'nn'
end

words2 = words.dup
words3 = []

words2.each do |word |
  CSYLLABLES.each do | cs1 |
    aid = word + assonance(cs1)
    puts aid
    words3 << aid
  end
  VSYLLABLES.each do | vs1 |
    next if word[-1] == vs1[0]
    aid = word + vassonance(vs1)
    puts aid
    words3 << aid
  end
end

words4 = []

words3.each do |word |
  CSYLLABLES.each do | cs1 |
    aid = word + assonance(cs1)
    puts aid
    words4 << aid
  end
  VSYLLABLES.each do | vs1 |
    next if word[-1] == vs1[0]
    aid = word + vassonance(vs1)
    puts aid
    words4 << aid
  end
end

words += words3
words += words4

words.sort!.uniq!

p words.sort
p words.size

# duals.each do  |d|
#     mid = 'y'
#     mid = '' if d[0] == 'y'
#     mid = '' if d[0] == 'w'
#     base v + mid + d
#   end





