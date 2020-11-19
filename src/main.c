/*
 * main.c
 * Copyright (C) 2020 Alexis Peypelut <peypeluta@live.fr>
 *
 * lunion-steamgriddb is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * lunion-steamgriddb is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>


typedef struct
{
	char* base_url;
	char* auth;
} http_api;


int get_apikey(char* key)
{
	fprintf(stdout, "Please enter your API key (https://www.steamgriddb.com/profile/preferences): ");
	fflush(stdout);
	
	fscanf(stdin, " %s", key);
	
	return EXIT_SUCCESS;
}


int main(int argc, char* argv[])
{
	char key[64];
	CURL* curl;
	CURLcode res;
	
	http_api* sgdb = (http_api*) calloc(1, sizeof(http_api));
	sgdb->base_url = (char*) calloc(512, sizeof(char));
	sgdb->auth = (char*) calloc(64, sizeof(char));
	
	fprintf(stdout, "%s\n", sgdb->base_url);
	get_apikey(key);
	// TODO Verif the API key (32 char)
	sgdb->auth = strncat(sgdb->auth, "Authorization: Bearer ", strlen("Authorization: Bearer ") + 1);
	sgdb->auth = strncat(sgdb->auth, key, strlen(key) + 1);
	sgdb->base_url = strncat(sgdb->base_url,
	                        "https://www.steamgriddb.com/api/v2/search/autocomplete/cyberpunk",
	                        strlen("https://www.steamgriddb.com/api/v2/search/autocomplete/cyberpunk") + 1);
	fprintf(stdout, "%s\n", sgdb->auth);
	
	
	// FIXME Find the memory leak (use --leak-check=full --show-leak-kinds=all)
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if(curl)
	{
		struct curl_slist* headers = NULL;
		
		curl_easy_setopt(curl, CURLOPT_URL, sgdb->base_url);
		
		headers = curl_slist_append (headers, sgdb->auth);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		
		res = curl_easy_perform(curl);
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		// Always cleanup
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	
	free(sgdb->auth);
	free(sgdb->base_url);
	free(sgdb);
	fprintf (stdout, "\n");
	return EXIT_SUCCESS;
}
