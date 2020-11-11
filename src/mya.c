#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <curl/curl.h>

const char *argp_program_version = "mya v1.0";
const char *argp_program_bug_address = "<jmak2015@gmail.com>";
static char doc[] = "Simple command line tool for fetching user anime data from MyAnimeList.";
static char args_doc[] = "[username]";
static struct argp_option options[] = {
	{ "watching", 'w', 0, 0, "fetch a user's currently watching anime" },
	{ "completed", 'c', 0, 0, "fetch a user's completed anime" },
	{ "hold", 'h', 0, 0, "fetch a user's on hold anime" },
	{ "dropped", 'd', 0, 0, "fetch a user's dropped anime" },
	{ "plan", 'p', 0, 0, "fetch a user's plan to watch anime" },
	{ 0 }
};

struct arguments {
	enum { WATCHING_MODE, COMPLETED_MODE, HOLD_MODE, DROPPED_MODE, PLAN_MODE } mode;
};

int main(void) {
	CURL *curl = curl_easy_init();

	if (!curl) {
		fprintf(stderr, "Init failed\n");
		return EXIT_FAILURE;
	}

	curl_easy_setopt(curl, CURLOPT_URL, "https://api.jikan.moe/v3/user/jmak/animelist/watching");

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		fprintf(stderr, "API fetch error: %s\n", curl_easy_strerror(res));
	}

	curl_easy_cleanup(curl);
	return EXIT_SUCCESS;
}

