# BSD 2 - Clause License
#
# Copyright(c) 2019, Maxime Bacoux
# All rights reserved.
# 
# Redistributionand use in sourceand binary forms, with or without
# modification, are permitted provided that the following conditions are met :
# 
# 1. Redistributions of source code must retain the above copyright notice, this
# list of conditionsand the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditionsand the following disclaimer in the documentation
# and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

require 'nokogiri'

Game = Struct.new(
  :name,
  :crc32,
  :region,
  :mapper,
  :mirror,
  :battery,
  :prg_rom_size,
  :prg_ram_size,
  :chr_rom_size,
  :chr_ram_size
) do
  def serialize
    n = name.tr('"', '')
    %Q[{ "#{n}", #{"0x%08x" % crc32}, #{region}, #{mapper}, #{mirror}, #{battery}, #{"0x%0x" % prg_rom_size}, #{"0x%0x" % prg_ram_size}, #{"0x%0x" % chr_rom_size}, #{"0x%0x" % chr_ram_size} },]
  end
end

def convert_size(x)
  x.to_i * 1024
end

db_file = ARGV[0]
db = File.open(db_file) { |f| Nokogiri::XML(f) }

games = []

db.root.css('game').each do |g|
  g.css('cartridge').each do |c|
    game = Game.new
    name = g.attr('name')
    region = g.attr('region')
    rev = c.attr('revision')
    game.name = name + " (" + region + ")"
    game.name.tr!('.<>:|?*', '')
    if (rev)
      game.name += " (Rev " + rev + ")"
    end
    game.name.tr!('\\/', '-')
    game.crc32 = c.attr('crc').to_s.to_i(16)
    game.region = 0
    if c.attr('system').include?('PAL')
      game.region = 1
    end

    board = c.css('board').first
    pad = c.css('pad').first

    game.mirror = 0
    game.mapper = board.attr('mapper').to_i
    if pad && pad.attr("h") == "1"
      game.mirror = 1
    end

    game.prg_rom_size = 0
    game.prg_ram_size = 0
    game.chr_rom_size = 0
    game.chr_ram_size = 0
    game.battery = 0

    prg = c.css('prg').first
    if prg
      game.prg_rom_size = convert_size(prg.attr('size'))
    end

    chr = c.css('chr').first
    if chr
      game.chr_rom_size = convert_size(chr.attr('size'))
    end

    wram = c.css('wram').first
    if wram
      game.prg_ram_size = convert_size(wram.attr('size'))
      game.battery = wram.attr('battery') ? 1 : 0
    end

    vram = c.css('vram').first
    if vram
      game.chr_ram_size = convert_size(vram.attr('size'))
    end

    games << game
  end
end

games.uniq! { |x| x.crc32 }
games.sort_by! { |x| x.crc32 }

File.open("inc/db.inc", "wb") do |f|
  games.each do |g|
    f << g.serialize
    f << "\n"
  end
end
