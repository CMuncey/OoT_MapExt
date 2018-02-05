# OoT_Map_Extractor

---

This is a map extractor for Zelda: Ocarina of Time. It will extract the .zscene file and any associated .zmap files for the given location. It will also create a .txt file that contains the start and end offsets for each map, as well as the scene itself.

The extractChest program will also provide a list of every chest the extractor found in the maps listed by map. This probably won't be too useful to anyone else (I'm using it for an OoT Item Randomiser), but I figured I'd include it anyway incase anyone wanted to learn from it. I wouldn't use this as "exceptional code to learn from" though, It's probably a really dumb way to go about doing this.

---

Compiling (Mac/Linux) [Choose one] : g++ -o Extract [extract.cpp/extractChest.cpp]

Compiling (Windows) : *Shrug*.. I think you can compile it since I included the endian.h (which is normally a linux only thing from what I can tell), but I haven't tried it. I would recommend VSCode if you can figure out how to link a compiler to it. If you do manage that, please share how you did so, I can't figure it out.

---

Usage: Extract [Name of file you want to create (ex: KokiriForest)] [Starting offset] [Ending offset] [Indoor/Outdoor (only checks for i/o)]

Notes: The filename can't have a space in it, you'll want to look at this site ( https://wiki.cloudmodding.com/oot/Scene_Table/NTSC_1.0 ) to find the start and end offsets, and the indoor/outdoor thing is because of how the filesystem works, I haven't found a way around that yet. If you want to know why the indoor/outdoor thing is important, there's a page about it on the same site as before, just search for "Finding zmap offsets" or something like up in the top right of that site.

---

Some general notes:

This is primarily made to work with the Utility of Time, which is a modding tool for OoT that allows you to look at the game and modify it kinda like a game engine (such as unreal or unity). I use the older version that takes zmaps and zscenes, because that's the only one I could get to work. I can't remember where I got it, but I think it was linked in a YouTube video, so just watch literally all of YouTube, and click every link you see until you find it.

You'll need a NTSC v1.0 ROM of OoT, and you'll need a decoder to decode the ROM (I suggest this one: http://www.mediafire.com/file/od4y72l0r57d7tl/newdec.7z ). This won't work with an encoded copy. You’ll also need to either rename the ROM to “ZOOTDEC.z64” (without the quotes), or change a few lines in the code.. Basically anywhere you see “ZOOTDEC.z64”. There’s one near the top, and then a few more at the bottom. This is because I didn't want to type out the name of the file every time I went to extract a map, so I just hardcoded the name in.

If you're on a Mac, and g++ doesn't work for you, you may need to install it using "sudo get-apt g++", then it should ask you for an administrator password. I don't know if it works like that on Mac, I've never really used one, but I think that'll work.

Honestly, there's some much better extractors out there if you actually want to extract maps from the game, but the one I used named the files by their offsets, which made them a pain to use, so I wrote my own.
