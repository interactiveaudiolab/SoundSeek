# SoundSeek
SoundSeek is a query by example tool for exploring sound libraries. Unlike most tools for managing sound libraries, which organize sounds using text tags, SoundSeek organizes sounds using a handpicked set of timbral features. The interactive sound map then allows users to explore their library of sounds in a fun and intuitive way.

![SoundSeek Screenshot](https://github.com/interactiveaudiolab/SoundSeek/blob/master/SoundSeek_screenshot.png)

## Supported platforms
SoundSeek was developed for MacOS. It has been confirmed to work on MacOS 10.11, 10.12, 10.13. 

## Installation
SoundSeek requires ffmpeg to be installed using hombrew.

- Install homebrew from [https://brew.sh](https://brew.sh)

- Run `brew install ffmpeg`

Install SoundSeek

- Download the latest SoundSeek.dmg from the [releases page.](https://github.com/interactiveaudiolab/SoundSeek/releases/latest)

- Mount the dmg, and copy SoundSeek.app to your applications folder.

## Using SoundSeek

- Start up SoundSeek

- Click the "open" button to select a directory tree that contains audio files. Anything that ffmpeg supports is a valid audio file.

- Click the "record" button to record a query OR select an existing audio file as a query by clicking "open"

- Sound seek will display connected graph of nodes. Each node is the closest audio file in the directory, as measured along one feature.

- Click on a node to hear that file

- Double click on a node to have SoundSeek return a new set of files that are close to this file

- Once you've found the file you want, right click on its node to open in finder
