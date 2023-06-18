# Contributing to the Project

Hey there!

Thanks for looking into how you're able to contribute to the project!

Check the [Issues](https://github.com/jmakhack/myanimelist-cli/issues) tab on GitHub to check the open issues.

Feel free to ping @jmakhack in the comments of any issue.

## Building the Project

To build the project, execute the following steps:

```
- cmake CMakeLists.txt
- make all
- ./bin/mya -V
```

If any errors surface, make sure to download and install all the needed libraries.

View the `Included Libraries` section below for more info.

## Using the MyAnimeList API

In order to properly setup the project, a Client ID from the MyAnimeList API is required.

Go to [https://myanimelist.net/apiconfig](https://myanimelist.net/apiconfig) and create a new Client ID.

Note that this also requires having/creating a MyAnimeList account.

Afterwards, set the `CLIENT_ID` field in `src/mya.c` to your generated Client ID.

This should allow you to run the project with actual data coming from MyAnimeList.

For more info on the API docs, visit [https://myanimelist.net/clubs.php?cid=13727](https://myanimelist.net/clubs.php?cid=13727)

## Contributing

In case you'd like to contribute to this project, there are a couple things that you'll have to do:

1.  Fork the project from [https://github.com/jmakhack/myanimelist-cli](https://github.com/jmakhack/myanimelist-cli)
2.  Clone the github project which you just forked
3.  Make the adjustments to solve the issue/ticket
4.  Add your changes and push them to any branch
5.  Visit [https://github.com/jmakhack/myanimelist-cli](https://github.com/jmakhack/myanimelist-cli) and press on the `Contribute` button.
6.  Create the pull request, and wait for reviews and/or approval.

And that's it! Thanks for contributing to the project! 

## Code Quality / Conventions
Please apply this style to your code before setting up the pull request, so we're able to remain a similar and professional code style.

For coding in C, it's a good practice to follow along the following code standard: [https://users.ece.cmu.edu/~eno/coding/CCodingStandard.html](https://users.ece.cmu.edu/~eno/coding/CCodingStandard.html) 

## Testing

At moment of writing, testing is not possible yet for this project.

## Included Libraries

The following artifacts / tools are used by and included in the extension as-is:

-   Argp: [https://www.gnu.org/software/libc/manual/html_node/Argp.html](https://www.gnu.org/software/libc/manual/html_node/Argp.html)
-   Curl: [https://curl.se/libcurl/c/](https://curl.se/libcurl/c/)
-   Json-C: [http://json-c.github.io/json-c/](http://json-c.github.io/json-c/)
-   Libbsd: [https://libbsd.freedesktop.org/wiki/](https://libbsd.freedesktop.org/wiki/)
