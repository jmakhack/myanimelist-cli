## myanimelist-cli

myanimelist-cli is a minimalistic command line interface for fetching user anime data from [MyAnimeList](https://myanimelist.net/)

[![C/C++ CI](https://github.com/jmakhack/mya/actions/workflows/c-cpp.yml/badge.svg?branch=master)](https://github.com/jmakhack/mya/actions/workflows/c-cpp.yml)
[![CodeQL](https://github.com/jmakhack/mya/actions/workflows/codeql-analysis.yml/badge.svg)](https://github.com/jmakhack/mya/actions/workflows/codeql-analysis.yml)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/a30e5356c06e4381a0b5ea92aba65ada)](https://www.codacy.com/gh/jmakhack/myanimelist-cli/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=jmakhack/myanimelist-cli&amp;utm_campaign=Badge_Grade)
[![Discord](https://img.shields.io/discord/1023562225790767175?color=%20%237289DA&label=discord&logo=discord&logoColor=%20%237289DA)](https://discord.gg/RTgxfFW9mS)<!-- ALL-CONTRIBUTORS-BADGE:START - Do not remove or modify this section -->
[![All Contributors](https://img.shields.io/badge/all_contributors-4-orange.svg?style=flat-square)](#contributors-)
<!-- ALL-CONTRIBUTORS-BADGE:END -->

## Usage
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

## Example
```
$ mya -w jmak
watching 4 anime
1. Commit on Titan
2. JoJo's Bizarre Pull Request
3. My Neighbor Octocat
4. One Push Man
```

## Code of Conduct
myanimelist-cli is governed by the [Contributor Covenant v2.0](CODE_OF_CONDUCT.md).

## License
myanimelist-cli is licensed under the [MIT License](LICENSE).

## Contributors ✨

Thanks goes to these wonderful people ([emoji key](https://allcontributors.org/docs/en/emoji-key)):

<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore-start -->
<!-- markdownlint-disable -->
<table>
  <tbody>
    <tr>
      <td align="center"><a href="https://github.com/jmakhack"><img src="https://avatars.githubusercontent.com/u/1442227?v=4?s=100" width="100px;" alt="jmakhack"/><br /><sub><b>jmakhack</b></sub></a><br /><a href="https://github.com/jmakhack/myanimelist-cli/commits?author=jmakhack" title="Code">💻</a> <a href="#maintenance-jmakhack" title="Maintenance">🚧</a> <a href="https://github.com/jmakhack/myanimelist-cli/commits?author=jmakhack" title="Documentation">📖</a></td>
      <td align="center"><a href="https://allcontributors.org"><img src="https://avatars.githubusercontent.com/u/46410174?v=4?s=100" width="100px;" alt="All Contributors"/><br /><sub><b>All Contributors</b></sub></a><br /><a href="https://github.com/jmakhack/myanimelist-cli/commits?author=all-contributors" title="Documentation">📖</a></td>
      <td align="center"><a href="https://www.linkedin.com/in/tim-beeren-88355615b/"><img src="https://avatars.githubusercontent.com/u/36151761?v=4?s=100" width="100px;" alt="Tim Beeren"/><br /><sub><b>Tim Beeren</b></sub></a><br /><a href="https://github.com/jmakhack/myanimelist-cli/commits?author=TBeeren" title="Tests">⚠️</a> <a href="https://github.com/jmakhack/myanimelist-cli/commits?author=TBeeren" title="Code">💻</a> <a href="https://github.com/jmakhack/myanimelist-cli/commits?author=TBeeren" title="Documentation">📖</a></td>
      <td align="center"><a href="https://vividhpandey.netlify.app/"><img src="https://avatars.githubusercontent.com/u/91251535?v=4?s=100" width="100px;" alt="Vividh Pandey"/><br /><sub><b>Vividh Pandey</b></sub></a><br /><a href="#maintenance-VividhPandey003" title="Maintenance">🚧</a></td>
    </tr>
  </tbody>
  <tfoot>
    
  </tfoot>
</table>

<!-- markdownlint-restore -->
<!-- prettier-ignore-end -->

<!-- ALL-CONTRIBUTORS-LIST:END -->

This project follows the [all-contributors](https://github.com/all-contributors/all-contributors) specification. Contributions of any kind welcome!
