## myanimelist-cli

myanimelist-cli is a minimalistic command line interface for fetching user anime data from [MyAnimeList](https://myanimelist.net/)

[![C/C++ CI](https://github.com/jmakhack/mya/actions/workflows/c-cpp.yml/badge.svg?branch=master)](https://github.com/jmakhack/mya/actions/workflows/c-cpp.yml)
[![CodeQL](https://github.com/jmakhack/mya/actions/workflows/codeql-analysis.yml/badge.svg)](https://github.com/jmakhack/mya/actions/workflows/codeql-analysis.yml)


Usage
-----
```
$ mya --help
Usage: mya [OPTION...] [USERNAME]
Simple command line tool for fetching user anime data from MyAnimeList.

  -a, --all                  Fetch all anime for a user
  -c, --completed            Fetch a user's completed anime
  -d, --dropped              Fetch a user's dropped anime
  -h, --onhold               Fetch a user's on hold anime
  -p, --plantowatch          Fetch a user's plan to watch anime
  -w, --watching             Fetch a user's currently watching anime
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Report bugs to <jmakhack@protonmail.com>.
```

Example
-----
```
$ mya -w jmak
watching 4 anime
1. Commit on Titan
2. JoJo's Bizarre Pull Request
3. My Neighbor Octocat
4. One Push Man
```
