#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include <regex.h>

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

		char username[18];
		strncpy(username, arg, sizeof(username)-1);
		username[17] = '\0';
		size_t arg_len = strlen(username);
		regex_t regex;
		char *pattern = "^[a-zA-Z0-9_-]+$";
		size_t nmatch = 1;
		regmatch_t pmatch[1];

		if (arg_len < 2 || arg_len > 16) {
			printf("Username must be between 2 and 16 characters in length\n");
			exit(argp_err_exit_status);
		}

		if (regcomp(&regex, pattern, REG_EXTENDED)) {
			fprintf(stderr, "Failed to compile username validation regex\n");
			exit(argp_err_exit_status);
		}

		if (regexec(&regex, arg, nmatch, pmatch, 0)) {
			printf("Please enter a valid username (letters, numbers, underscores and dashes only)\n");
			exit(argp_err_exit_status);
		}

		regfree(&regex);

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

int main (int argc, char *argv[]) {
	struct arguments arguments;
	arguments.mode = WATCHING_MODE;
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	char *endpoint;
	switch (arguments.mode) {
	case ALL_MODE: endpoint = "all"; break;
	case COMPLETED_MODE: endpoint = "completed"; break;
	case HOLD_MODE: endpoint = "onhold"; break;
	case DROPPED_MODE: endpoint = "dropped"; break;
	case PLAN_MODE: endpoint = "plantowatch"; break;
	default: endpoint = "watching"; break;
	}

	char base_url[249];
	strcpy(base_url, "https://api.jikan.moe/v3/user/");
	strcat(base_url, arguments.args[0]);
	strcat(base_url, "/animelist/");
	strcat(base_url, endpoint);
	strcat(base_url, "?order_by=title&sort=desc&");

	short page_num = 1;
	size_t page_size = 300;
	char page[2], paginated_url[256];

	CURL *curl;
	CURLcode res;
	struct curl_fetch_st curl_fetch;
	struct curl_fetch_st *cf = &curl_fetch;

	struct json_object *anime_list, *anime_json, *anime, *anime_title, *json;
	size_t n_anime;
	json_bool is_user_exist;

	while (1) {
		sprintf(page, "%d", page_num);

	       	strcpy(paginated_url, base_url);
		strcat(paginated_url, "page=");
		strcat(paginated_url, page);

		curl = curl_easy_init();

		if (!curl) {
			fprintf(stderr, "Curl init failed\n");
			return EXIT_FAILURE;
		}

		res = curl_fetch_url(curl, paginated_url, cf);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK) {
			fprintf(stderr, "API fetch error: %s\n", curl_easy_strerror(res));
			free(cf->payload);
			return EXIT_FAILURE;
		}

		json = json_tokener_parse(cf->payload);
		free(cf->payload);

		is_user_exist = json_object_object_get_ex(json, "anime", &anime_list);

		if (!is_user_exist) {
			fprintf(stderr, "User not found\n");
			return EXIT_FAILURE;
		}

		n_anime = json_object_array_length(anime_list);

		if (page_num == 1) {
			if (n_anime == page_size) {
				printf("%s %lu+ anime\n", endpoint, page_size);
			} else {
				printf("%s %lu anime\n", endpoint, n_anime);
			}
		}

		for (size_t i = 0; i < n_anime; i++) {
			anime = json_object_array_get_idx(anime_list, i);
			anime_json = json_tokener_parse(json_object_get_string(anime));

			json_object_object_get_ex(anime_json, "title", &anime_title);
			printf("%lu. %s\n", (i+1)+(300*(page_num-1)), json_object_get_string(anime_title));
		}

		json_object_put(json);

		if (n_anime < page_size) break;

		page_num += 1;
	}

	return EXIT_SUCCESS;
}
