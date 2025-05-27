<img src="/.github/img/banner.png" alt="OrgMaker 3">

OrgMaker 3 is an updated fork of OrgMaker, a simple piano roll music sequencer. It was the software used to create the music for [Cave Story](https://www.cavestory.org/game-info/about-cave-story.php).

<img src="/.github/img/screenshot.png" width="100%" alt="Screenshot of OrgMaker 3">

## History

In 1999, Daisuke Amaya, better known online as [Studio Pixel](https://studiopixel.jp), created the Organya music engine for use in Cave Story, although it was also used in a few other games. Pixel created the original version of OrgMaker (which was also named Organya in Japan) to produce music in this format.

In 2003, Pixel gave the source code of OrgMaker to [Rxo Inverse](http://www5b.biglobe.ne.jp/~kiss-me/aji/index.html) for use in the game [STARGAZER](http://www5b.biglobe.ne.jp/~kiss-me/aji/star/). Rxo would continue updating the program, now as OrgMaker 2, until the end of 2014. It was eventually [open-sourced](https://github.com/shbow/organya) in 2018.

## Features

Organya allows for use of up to 8 melody channels, and 8 percussion channels. Melodic instruments are selected from 100 waveforms, and percussive instruments are selected from 42 samples. Melody channels also support finetuning, and a 'pizzicato' mode, which makes notes produce a short blip instead of sustaining.

## Download

Check the Releases for the latest version: [![Releases](https://img.shields.io/github/downloads/Strultz/orgmaker-3/total?style=plastic&logo=github)](https://github.com/Strultz/orgmaker-3/releases)

## Compiling

You'll need Visual Studio 2022 with the `Desktop development with C++` workload. You should also select the `C++ MFC for latest v143 build tools` component. After installing, open `source/Org3.sln` and build the project.
