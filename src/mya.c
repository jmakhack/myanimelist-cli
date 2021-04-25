#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include <regex.h>

#define MYA_MIN_USERNAME_LENGTH 2
#define MYA_MAX_USERNAME_LENGTH 16
#define JIKAN_PAGE_SIZE 300

const char *argp_program_version = "mya v0.1.0";
const char *argp_program_bug_address = "<jmakhack@protonmail.com>";
static char doc[] = "Simple command line tool for fetching user anime data from MyAnimeList.";
static char args_doc[] = "[USERNAME]";
static struct argp_option options[] = {
	{ "watching", 'w', 0, 0, "Fetch a user's currently watching anime" },
	{ "completed", 'c', 0, 0, "Fetch a user's completed anime" },
	{ "onhold", 'h', 0, 0, "Fetch a user's on hold anime" },
	{ "dropped", 'd', 0, 0, "Fetch a user's dropped anime" },
	{ "plantowatch", 'p', 0, 0, "Fetch a user's plan to watch anime" },
	{ "all", 'a', 0, 0, "Fetch all anime for a user" },
	{ 0 }
};

struct arguments {
	enum { WATCHING_MODE, COMPLETED_MODE, HOLD_MODE, DROPPED_MODE, PLAN_MODE, ALL_MODE } mode;
	char *args[1];
};

void validate_username (char *username) {
	char username_buf[MYA_MAX_USERNAME_LENGTH+2];
	strncpy(username_buf, username, sizeof(username_buf)-1);
	username_buf[sizeof(username_buf)-1] = '\0';
	size_t username_len = strlen(username_buf);
	regex_t regex;
	char *pattern = "^[a-zA-Z0-9_-]+$";
	size_t nmatch = 1;
	regmatch_t pmatch[1];

	if (username_len < MYA_MIN_USERNAME_LENGTH || username_len > MYA_MAX_USERNAME_LENGTH) {
		fprintf(stderr, "Username must be between %d and %d characters in length\n", MYA_MIN_USERNAME_LENGTH, MYA_MAX_USERNAME_LENGTH);
		exit(argp_err_exit_status);
	}

	if (regcomp(&regex, pattern, REG_EXTENDED)) {
		fprintf(stderr, "Failed to compile username validation regex\n");
		exit(argp_err_exit_status);
	}

	if (regexec(&regex, username, nmatch, pmatch, 0)) {
		fprintf(stderr, "Please enter a valid username (letters, numbers, underscores and dashes only)\n");
		exit(argp_err_exit_status);
	}

	regfree(&regex);
}

static error_t parse_opt (int key, char *arg, struct argp_state *state) {
	struct arguments *arguments = state->input;
	switch (key) {
	case 'w': arguments->mode = WATCHING_MODE; break;
	case 'c': arguments->mode = COMPLETED_MODE; break;
	case 'h': arguments->mode = HOLD_MODE; break;
	case 'd': arguments->mode = DROPPED_MODE; break;
	case 'p': arguments->mode = PLAN_MODE; break;
	case 'a': arguments->mode = ALL_MODE; break;
	case ARGP_KEY_ARG:
		if (state->arg_num >= 1) argp_usage(state);
		validate_username(arg);
		arguments->args[state->arg_num] = arg;
		break;
	case ARGP_KEY_END:
		if (state->arg_num < 1) argp_usage(state);
		break;
	default: return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

struct curl_fetch_st {
	char *payload;
	size_t size;
};

size_t curl_callback (void *contents, size_t size, size_t nmemb, void *userp) {
	size_t rsize = size * nmemb;
	struct curl_fetch_st *p = (struct curl_fetch_st *) userp;

	char *temp = realloc(p->payload, p->size + rsize + 1);

	if (!temp) {
		fprintf(stderr, "Failed to expand buffer for fetch payload");
		free(p->payload);
		return 0;
	}

	p->payload = temp;
	memcpy(&(p->payload[p->size]), contents, rsize);
	p->size += rsize;
	p->payload[p->size] = 0;

	return rsize;
}

CURLcode curl_fetch_url (CURL *curl, const char *url, struct curl_fetch_st *fetch) {
	fetch->payload = (char *) calloc(1, sizeof(fetch->payload));
	fetch->size = 0;

	if (!(fetch->payload)) {
		fprintf(stderr, "Failed to allocate payload");
		return CURLE_FAILED_INIT;
	}

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) fetch);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_callback);
	
	return curl_easy_perform(curl);
}

void generate_endpoint (char *endpoint, size_t mode) {
	switch (mode) {
	case ALL_MODE: strcpy(endpoint, "all"); break;
	case COMPLETED_MODE: strcpy(endpoint, "completed"); break;
	case HOLD_MODE: strcpy(endpoint, "onhold"); break;
	case DROPPED_MODE: strcpy(endpoint, "dropped"); break;
	case PLAN_MODE: strcpy(endpoint, "plantowatch"); break;
	default: strcpy(endpoint, "watching"); break;
	}
}

void generate_anime_api_uri (char *uri, char *username, char *endpoint) {
	strcpy(uri, "https://api.jikan.moe/v3/user/");
	strcat(uri, username);
	strcat(uri, "/animelist/");
	strcat(uri, endpoint);
	strcat(uri, "?order_by=title&sort=desc");
}

void generate_paginated_uri (char *paginated_uri, char *base_uri, short page) {
	char page_buf[2];
	sprintf(page_buf, "%hu", page);
	strcpy(paginated_uri, base_uri);
	strcat(paginated_uri, "&page=");
	strcat(paginated_uri, page_buf);
}

struct curl_fetch_st get_anime_list (char *paginated_uri) {
	CURL *curl = curl_easy_init();

	if (!curl) {
		fprintf(stderr, "Curl init failed\n");
		exit(EXIT_FAILURE);
	}

	struct curl_fetch_st curl_fetch;
	struct curl_fetch_st *cf = &curl_fetch;
	CURLcode res = curl_fetch_url(curl, paginated_uri, cf);
	curl_easy_cleanup(curl);

	if (res != CURLE_OK) {
		fprintf(stderr, "API fetch error: %s\n", curl_easy_strerror(res));
		free(cf->payload);
		exit(EXIT_FAILURE);
	}

	return curl_fetch;
}

size_t print_anime_list (struct json_object *anime_list, short page, char *list_name) {
	size_t n_anime = json_object_array_length(anime_list);

	if (page == 1) {
		if (n_anime == JIKAN_PAGE_SIZE) {
			printf("%s %d+ anime\n", list_name, JIKAN_PAGE_SIZE);
		} else {
			printf("%s %lu anime\n", list_name, n_anime);
		}
	}

	for (size_t i = 0; i < n_anime; i++) {
		struct json_object *anime = json_object_array_get_idx(anime_list, i);
		struct json_object *anime_json = json_tokener_parse(json_object_get_string(anime));
		struct json_object *anime_title;
		json_object_object_get_ex(anime_json, "title", &anime_title);
		printf("%lu. %s\n", (i+1)+(JIKAN_PAGE_SIZE*(page-1)), json_object_get_string(anime_title));
	}

	return n_anime;
}

int main (int argc, char *argv[]) {
	struct arguments arguments;
	arguments.mode = WATCHING_MODE;
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	char endpoint[12], base_uri[94];
	generate_endpoint(endpoint, arguments.mode);
	generate_anime_api_uri(base_uri, arguments.args[0], endpoint);

	short page_num = 0;

	while (++page_num) {
		char paginated_uri[102];
		generate_paginated_uri(paginated_uri, base_uri, page_num);

		struct curl_fetch_st curl_fetch = get_anime_list(paginated_uri);
		struct json_object *json = json_tokener_parse(curl_fetch.payload);
		free(curl_fetch.payload);

		struct json_object *anime_list;

		if (!json_object_object_get_ex(json, "anime", &anime_list)) {
			fprintf(stderr, "User not found\n");
			exit(EXIT_FAILURE);
		}
		
		size_t num_anime = print_anime_list(anime_list, page_num, endpoint);

		json_object_put(json);
		if (num_anime < JIKAN_PAGE_SIZE) break;
	}
	return EXIT_SUCCESS;
}

