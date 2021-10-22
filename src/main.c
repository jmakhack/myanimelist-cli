/***************************************************************************
* Copyright (C) 2020 - 2021, Joshua Mak, <jmakhack@protonmail.com>, et al.
/***************************************************************************/
#include "mya.h"

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