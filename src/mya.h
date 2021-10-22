#ifndef MYA_H
#define MYA_H

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
void validate_username(char *username);
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
static error_t parse_opt (int key, char *arg, struct argp_state *state);
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

size_t curl_callback (void *contents, size_t size, size_t nmemb, void *userp);
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
CURLcode curl_fetch_url (CURL *curl, const char *url, struct curl_fetch_st *fetch);
/*
 * Function: generate_endpoint
 * ---------------------------
 * Generates the appropriate endpoint based on the inputted mode
 *
 * endpoint: string to store the endpoint value
 * mode: type of list to retrieve
 */
void generate_endpoint (char *endpoint, size_t mode);
/*
 * Function: generate_anime_api_uri
 * --------------------------------
 * Generates the base uri for retrieving user anime list data
 *
 * uri: string to store the uri
 * username: user to fetch the data of
 * endpoint: endpoint to fetch the data from
 */
void generate_anime_api_uri (char *uri, char *username, char *endpoint);
/*
 * Function: generate_paginated_uri
 * --------------------------------
 * Generates the paginated version of the uri
 *
 * paginated_uri: string to store the paginated uri
 * base_uri: the uri to paginate
 * page: the page number to retreive
 */
void generate_paginated_uri (char *paginated_uri, char *base_uri, size_t page);
/*
 * Function: fetch_curl_payload
 * ----------------------------
 * Fetches the payload from the uri
 *
 * curl_fetch: pointer to curl fetch struct to store payload
 * paginated_uri: uri to fetch the data from
 */
void fetch_curl_payload (struct curl_fetch_st *curl_fetch, char *paginated_uri);
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
size_t print_anime_list (struct json_object *anime_list, size_t page, char *list_name);

#endif /* MYA_H */