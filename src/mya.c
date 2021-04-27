#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include <regex.h>

#define MIN_USERNAME_LENGTH  2
#define MAX_USERNAME_LENGTH  16
#define PAGE_SIZE            300

/* define constants for program options */
#define OPT_WATCHING     'w'
#define OPT_COMPLETED    'c'
#define OPT_ONHOLD       'h'
#define OPT_DROPPED      'd'
#define OPT_PLANTOWATCH  'p'
#define OPT_ALL          'a'

/* initialize argp vars */
const char *argp_program_version     = "mya v0.1.0";
const char *argp_program_bug_address = "<jmakhack@protonmail.com>";
static char doc[]                    = "Simple command line tool for fetching user anime data from MyAnimeList.";
static char args_doc[]               = "[USERNAME]";
static struct argp_option options[]  = {
	{ "watching",    OPT_WATCHING,    0, 0, "Fetch a user's currently watching anime" },
	{ "completed",   OPT_COMPLETED,   0, 0, "Fetch a user's completed anime"          },
	{ "onhold",      OPT_ONHOLD,      0, 0, "Fetch a user's on hold anime"            },
	{ "dropped",     OPT_DROPPED,     0, 0, "Fetch a user's dropped anime"            },
	{ "plantowatch", OPT_PLANTOWATCH, 0, 0, "Fetch a user's plan to watch anime"      },
	{ "all",         OPT_ALL,         0, 0, "Fetch all anime for a user"              },
	{ 0 }
};

/* struct to keep track of selected options and arguments */
struct arguments {
	enum { WATCHING_MODE, COMPLETED_MODE, HOLD_MODE, DROPPED_MODE, PLAN_MODE, ALL_MODE } mode;
	char *args[1];
};

/* struct to store curl fetch payload and size */
struct curl_fetch_st {
	char *payload;
	size_t size;
};

/*
 * Function: validate_username
 * ---------------------------
 * Runs validation on the inputted username by checking:
 *     - username is at least 2 characters long
 *     - username is no more than 16 characters long
 *     - username only contains letters, numbers, underscores and dashes only
 *
 * If validation fails, an error message is displayed and the program exits
 *
 * username: the username string to validate
 */
void validate_username (char *username) {
	/* create username buffer to ensure null terminated string */
	char username_buf[MAX_USERNAME_LENGTH+2];
	strncpy(username_buf, username, sizeof(username_buf)-1);
	username_buf[sizeof(username_buf)-1] = '\0';
	size_t username_len = strlen(username_buf);

	/* initialize regex vars */
	regex_t regex;
	char *pattern = "^[a-zA-Z0-9_-]+$";
	size_t nmatch = 1;
	regmatch_t pmatch[1];

	/* check if username is acceptable length */	
	if (username_len < MIN_USERNAME_LENGTH || username_len > MAX_USERNAME_LENGTH) {
		fprintf(stderr, "Username must be between %d and %d characters in length\n", MIN_USERNAME_LENGTH, MAX_USERNAME_LENGTH);
		exit(argp_err_exit_status);
	}

	/* check if regex pattern is valid */
	if (regcomp(&regex, pattern, REG_EXTENDED)) {
		fprintf(stderr, "Failed to compile username validation regex\n");
		exit(argp_err_exit_status);
	}

	/* check if username contains only letters, numbers, underscores and dashes */
	if (regexec(&regex, username, nmatch, pmatch, 0)) {
		fprintf(stderr, "Please enter a valid username (letters, numbers, underscores and dashes only)\n");
		exit(argp_err_exit_status);
	}

	/* free regex var */
	regfree(&regex);
}

/*
 * Function: parse_opt
 * -------------------
 * Parses all the various options and arguments when running the program
 *
 * key: specifies which option or non-option argument to parse
 * arg: string value of argument or NULL if unapplicable
 * state: pointer to argp_state struct
 *
 * returns: 0 on success, otherwise an error value
 */
static error_t parse_opt (int key, char *arg, struct argp_state *state) {
	struct arguments *arguments = state->input;
	switch (key) {

	/* parse available options */
	case OPT_WATCHING:    arguments->mode = WATCHING_MODE;  break;
	case OPT_COMPLETED:   arguments->mode = COMPLETED_MODE; break;
	case OPT_ONHOLD:      arguments->mode = HOLD_MODE;      break;
	case OPT_DROPPED:     arguments->mode = DROPPED_MODE;   break;
	case OPT_PLANTOWATCH: arguments->mode = PLAN_MODE;      break;
	case OPT_ALL:         arguments->mode = ALL_MODE;       break;

	/* parse arguments */
	case ARGP_KEY_ARG:
		/* show usage info if more than one argument given */
		if (state->arg_num >= 1) argp_usage(state);
		validate_username(arg);
		arguments->args[state->arg_num] = arg;
		break;

	/* validate number of arguments */
	case ARGP_KEY_END:
		if (state->arg_num < 1) argp_usage(state);
		break;

	/* error if invalid option provided */
	default: return ARGP_ERR_UNKNOWN;
	}

	/* parse success */
	return 0;
}

/* struct for specifying argument parsing behavior */
static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

/*
 * Function: curl_callback
 * -----------------------
 * Curl callback which assigns the delivered data to a properly sized struct buffer
 *
 * contents: pointer to the data
 * size: always 1
 * nmemb: size of the data
 * userp: pointer to the struct to store the data
 *
 * returns: number of bytes in processed data
 */
size_t curl_callback (void *contents, size_t size, size_t nmemb, void *userp) {
	/* calculate size of incoming data */
	size_t rsize = size * nmemb;
	struct curl_fetch_st *p = (struct curl_fetch_st *) userp;

	/* allocate enough memory to hold data payload */
	char *temp = realloc(p->payload, p->size + rsize + 1);

	/* error if memory allocation fails */
	if (!temp) {
		fprintf(stderr, "Failed to expand buffer for fetch payload");
		free(p->payload);
		return 0;
	}

	/* write incoming data into curl struct */
	p->payload = temp;
	memcpy(&(p->payload[p->size]), contents, rsize);
	p->size += rsize;
	p->payload[p->size] = 0;

	/* return size of the data payload in bytes */
	return rsize;
}

/*
 * Function: curl_fetch_url
 * ------------------------
 * Fetch the data from the url and capture the return code
 *
 * curl: curl handle
 * url: url to fetch data from
 * fetch: pointer to fetch struct
 *
 * returns: return code of the url fetch
 */
CURLcode curl_fetch_url (CURL *curl, const char *url, struct curl_fetch_st *fetch) {
	/* initialize curl struct values */
	fetch->payload = (char *) calloc(1, sizeof(fetch->payload));
	fetch->size = 0;

	/* error if memory allocation fails */
	if (!(fetch->payload)) {
		fprintf(stderr, "Failed to allocate payload");
		return CURLE_FAILED_INIT;
	}

	/* set url to retreive data from */
	curl_easy_setopt(curl, CURLOPT_URL, url);

	/* set user agent request header */
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	/* disable follow location */
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0);

	/* set curl timeout to 30 seconds */
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);

	/* set struct to copy incoming data into */
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) fetch);

	/* set callback for handling the writing of incoming data */
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_callback);

	/* return curl code returned from request */
	return curl_easy_perform(curl);
}

/*
 * Function: generate_endpoint
 * ---------------------------
 * Generates the appropriate endpoint based on the inputted mode
 *
 * endpoint: string to store the endpoint value
 * mode: type of list to retrieve
 */
void generate_endpoint (char *endpoint, size_t mode) {
	switch (mode) {
	case ALL_MODE:       strcpy(endpoint, "all");         break;
	case COMPLETED_MODE: strcpy(endpoint, "completed");   break;
	case HOLD_MODE:      strcpy(endpoint, "onhold");      break;
	case DROPPED_MODE:   strcpy(endpoint, "dropped");     break;
	case PLAN_MODE:      strcpy(endpoint, "plantowatch"); break;
	default:             strcpy(endpoint, "watching");    break;
	}
}

/*
 * Function: generate_anime_api_uri
 * --------------------------------
 * Generates the base uri for retrieving user anime list data
 *
 * uri: string to store the uri
 * username: user to fetch the data of
 * endpoint: endpoint to fetch the data from
 */
void generate_anime_api_uri (char *uri, char *username, char *endpoint) {
	strcpy(uri, "https://api.jikan.moe/v3/user/");
	strcat(uri, username);
	strcat(uri, "/animelist/");
	strcat(uri, endpoint);

	/* sort list by title descending */
	strcat(uri, "?order_by=title&sort=desc");
}

/*
 * Function: generate_paginated_uri
 * --------------------------------
 * Generates the paginated version of the uri
 *
 * paginated_uri: string to store the paginated uri
 * base_uri: the uri to paginate
 * page: the page number to retreive
 */
void generate_paginated_uri (char *paginated_uri, char *base_uri, size_t page) {
	/* convert page number to string */
	char page_buf[2];
	sprintf(page_buf, "%zu", page);
	strcpy(paginated_uri, base_uri);
	strcat(paginated_uri, "&page=");
	strcat(paginated_uri, page_buf);
}

/*
 * Function: fetch_curl_payload
 * ----------------------------
 * Fetches the payload from the uri
 *
 * curl_fetch: pointer to curl fetch struct to store payload
 * paginated_uri: uri to fetch the data from
 */
void fetch_curl_payload (struct curl_fetch_st *curl_fetch, char *paginated_uri) {
	/* initialize curl */
	CURL *curl = curl_easy_init();

	/* error on curl initialization failure */
	if (!curl) {
		fprintf(stderr, "Curl init failed\n");
		exit(EXIT_FAILURE);
	}

	/* fetch curl and cleanup */
	CURLcode res = curl_fetch_url(curl, paginated_uri, curl_fetch);
	curl_easy_cleanup(curl);

	/* error on failed curl response */
	if (res != CURLE_OK) {
		fprintf(stderr, "API fetch error: %s\n", curl_easy_strerror(res));
		free(curl_fetch->payload);
		exit(EXIT_FAILURE);
	}
}

/*
 * Function: print_anime_list
 * --------------------------
 * Prints the anime list values in a list format
 *
 * anime_list: anime list to print
 * page: page number of paginated list
 * list_name: name of the type of list being printed
 *
 * returns: number of anime printed
 */
size_t print_anime_list (struct json_object *anime_list, size_t page, char *list_name) {
	/* get number of anime in anime list */
	size_t n_anime = json_object_array_length(anime_list);

	/* print list header before the first page of data */
	if (page == 1) {
		if (n_anime == PAGE_SIZE) {
			printf("%s %d+ anime\n", list_name, PAGE_SIZE);
		} else {
			printf("%s %zu anime\n", list_name, n_anime);
		}
	}

	/* iterate through anime list and print each anime title */
	for (size_t i = 0; i < n_anime; i++) {
		struct json_object *anime = json_object_array_get_idx(anime_list, i);
		struct json_object *anime_json = json_tokener_parse(json_object_get_string(anime));
		struct json_object *anime_title;
		json_object_object_get_ex(anime_json, "title", &anime_title);

		/* print each anime title in a numbered list format */
		printf("%zu. %s\n", (i+1)+(PAGE_SIZE*(page-1)), json_object_get_string(anime_title));
	}

	/* return the number of anime printed */
	return n_anime;
}

/*
 * Function: main
 * --------------
 * Main entrypoint of program
 *
 * argc: number of arguments
 * argv[]: array of arguments
 *
 * returns: 0 if success, otherwise error number
 */
int main (int argc, char *argv[]) {
	/* parse options and arguments */
	struct arguments arguments;
	arguments.mode = WATCHING_MODE;
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	/* setup uri to fetch based on arguments */
	char endpoint[12], base_uri[94];
	generate_endpoint(endpoint, arguments.mode);
	generate_anime_api_uri(base_uri, arguments.args[0], endpoint);

	/* iterator value for paginated data */
	size_t page_num = 0;

	/* main loop for printing paginated anime list */
	while (++page_num) {
		/* create paginated uri */
		char paginated_uri[102];
		generate_paginated_uri(paginated_uri, base_uri, page_num);

		/* fetch data from uri */
		struct curl_fetch_st curl_fetch;
		fetch_curl_payload(&curl_fetch, paginated_uri);
		struct json_object *json = json_tokener_parse(curl_fetch.payload);
		free(curl_fetch.payload);

		/* create json object to store anime list data */
		struct json_object *anime_list;

		/* error when anime list is not found due to invalid user */
		if (!json_object_object_get_ex(json, "anime", &anime_list)) {
			fprintf(stderr, "User not found\n");
			exit(EXIT_FAILURE);
		}

		/* print out the anime list */
		size_t num_anime = print_anime_list(anime_list, page_num, endpoint);

		/* cleanup json */
		json_object_put(json);

		/* exit main loop after all pages have been looped through */
		if (num_anime < PAGE_SIZE) break;
	}
	return EXIT_SUCCESS;
}
