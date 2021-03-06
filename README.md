﻿# ATCFS

[![Build status](https://ci.appveyor.com/api/projects/status/aounl4vro2jo7w77?svg=true)](https://ci.appveyor.com/project/s520/atcfs)
[![Report](https://inspecode.rocro.com/badges/github.com/s520/ATCFS/report?token=ZDO_P6vfadA1TNYuuEyABsMakP1R4Mh0oUsMbLrFgQo&branch=master)](https://inspecode.rocro.com/reports/github.com/s520/ATCFS/branch/master/summary)

ATC-1, ATC-2, ATC-NS, KS-ATC, DS-ATC, ATS-P Plugin for BVE5 / OpenBVE On Windows

このプラグインはWin32プラグインです。.NETアセンブリ版は[こちら](https://github.com/s520/ATCFS_For_OpenBVE)。

## Description

新幹線の保安装置を再現するBVE用ATCプラグインです。

詳細な仕様は[ATCFS Specifications.pdf](https://github.com/s520/ATCFS/blob/master/doc/ATCFS%20Specifications.pdf)をご覧ください。

ドキュメントは[こちら](https://s520.github.io/ATCFS/)。

## Features

- 各種ATCの挙動を再現
- ATC-NS, KS-ATC, DS-ATCでは、予見Fuzzy制御により1段ブレーキを再現
- 各種ATCのモニタ表示を再現
- ワイパーの動作を再現

## Requirement

- Boost C++ Libraries (header only) 1.67.0 or more
- Visual Studio 2008 or more

## License

2-clause BSD license

- Since this project contains part BVEC_ATS source code, that part is provided under the 2-clause BSD license of BVEC_ATS described below.

## Postscript

本プロジェクトの英語への翻訳にご協力いただける方を募集しております。

翻訳にご協力いただける方はpull requestを送ってください。