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
	char* key;
	char* auth;
} sgdb;

int get_apikey(char** key)
{
	fprintf(stdout, "Please enter your API key (https://www.steamgriddb.com/profile/preferences): ");
	fflush(stdout);
	
	fscanf(stdin, "%s", *key);
	// Delete '\n' charactere
	fgetc(stdin);
	
	return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
	CURL* curl;
	CURLcode res;
	
	sgdb* api = (sgdb*) calloc(1, sizeof(sgdb));
	api->base_url = (char*) calloc(512, sizeof(char));
	api->key = (char*) calloc(33, sizeof(char));
	api->auth = (char*) calloc(64, sizeof(char));
	
	fprintf(stdout, "%s\n", api->base_url);
	get_apikey(&(api->key));
	// TODO Verif the API key
	api->auth = strncat(api->auth, "Authorization: Bearer ", strlen("Authorization: Bearer ") + 1);
	api->auth = strncat(api->auth, api->key, strlen(api->key) + 1);
	api->base_url = strncat(api->base_url, "https://www.steamgriddb.com/api/v2/search/autocomplete/cyberpunk", strlen("https://www.steamgriddb.com/api/v2/search/autocomplete/cyberpunk") + 1);
	fprintf(stdout, "%s\n", api->auth);
	
	
	// FIXME Find the memory leak (use --leak-check=full --show-leak-kinds=all)
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if(curl)
	{
		struct curl_slist* headers = NULL;
		
		curl_easy_setopt(curl, CURLOPT_URL, api->base_url);
		
		headers = curl_slist_append (headers, api->auth);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		
		res = curl_easy_perform(curl);
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		// Always cleanup
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	
	free(api->key);
	free(api->auth);
	free(api->base_url);
	free(api);
	fprintf (stdout, "\n");
	return EXIT_SUCCESS;
}
