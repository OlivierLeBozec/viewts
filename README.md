![linux compile](https://github.com/OlivierLeBozec/viewts/workflows/linux%20compile/badge.svg)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/0fb4b49561494673ad88048db7b17943)](https://app.codacy.com/manual/OlivierLeBozec/viewts?utm_source=github.com&utm_medium=referral&utm_content=OlivierLeBozec/viewts&utm_campaign=Badge_Grade_Dashboard)
[![CodeFactor](https://www.codefactor.io/repository/github/olivierlebozec/viewts/badge/master)](https://www.codefactor.io/repository/github/olivierlebozec/viewts/overview/master)

# viewts - Display timestamps and related feature

viewts is a tool to analyse pts, pcr and dts in a ts MPEG2 stream.

Qt and QChart are needed for this project.

Already compiled binaries (for windows and linux) are available in <https://github.com/OlivierLeBozec/tstools/releases>

## Feature

Display :
*   PCR, PTS, DTS
*   continuity counter error
*   Random access point

Compute :
*   Bitrate
*   PCR jitter
*   Diff between timestamps
*   ES buffer level, basically the VBV

Other features :
*   X axis can be set to time or packet number
*   Save data into file
*   command line interface
*   ts file drag and drop
*   zoom on data

## Screenshots

![alt text](https://github.com/OlivierLeBozec/tstools/blob/master/screenshots/pcrjitter.png)

![alt text](https://github.com/OlivierLeBozec/tstools/blob/master/screenshots/pcrptsdts.png)

![alt text](https://github.com/OlivierLeBozec/tstools/blob/master/screenshots/ptspcr.png)

## compile
Github action is configure to test and compile this project. Check file .github/workflows/main.yml to see how to compile.

This project contains viewts and tstool. They can be compile separatly. tstool is a "subproject" of viewts.

Unfortunatly for tstool I had to include googletest code in this project cause download of tag 1.8.1 failed.

## How to Contribute

We welcome contributions of all kinds, including bug fixes, new features,
documentation and translations. By contributing, you agree to release
your contributions under the terms of the license.

Contribute by following the typical
[GitHub workflow](https://guides.github.com/introduction/flow/index.html)
for pull requests. Fork the repository and make changes on a new named
branch. Create pull requests against the `master` branch. Follow the
[seven guidelines](https://chris.beams.io/posts/git-commit/) to writing a
great commit message.
