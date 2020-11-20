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
	char* name;
	char* url;
	char* h_auth;
} service;


service* init_service()
{
	service* s = (service*) calloc(1, sizeof(service));
	s->name = (char*) calloc(64, sizeof(char));
	s->url = (char*) calloc(512, sizeof(char));
	s->h_auth = (char*) calloc(64, sizeof(char));
	
	return s;
}


void destruct_service(service* s)
{
	free(s->h_auth);
	free(s->url);
	free(s->name);
	free(s);
}


int sgdb_get_token(char* token)
{
	fprintf(stdout, "Please enter your API key ( https://www.steamgriddb.com/profile/preferences ): ");
	fflush(stdout);
	
	fscanf(stdin, " %s", token);
	
	return EXIT_SUCCESS;
}


int sgdb_verif_token(char* token)    // Function name: <service>_<action>_<text>
{
	int nbr;
	
	nbr = strlen(token);
	
	fprintf(stdout, "token validity: ");
	fflush(stdout);
	

	if (nbr != 32)
	{
		fprintf(stdout, "\033[1;31mFAILED\033[0m");
		return EXIT_FAILURE;
	}
	
	fprintf(stdout, "\033[1;32mOK\033[0m");
	return EXIT_SUCCESS;
}


int main(int argc, char* argv[])
{
	char     token[64];
	CURL*    curl;
	CURLcode res;
	service* sgdb = NULL;
	
	// Initialisation service
	sgdb = init_service();
	if (NULL == sgdb)
		return EXIT_FAILURE;
	
	sgdb_get_token(token);
	if (sgdb_verif_token(token) != EXIT_SUCCESS)
	{
		destruct_service(sgdb);
		fprintf (stdout, "\n");
		return EXIT_FAILURE;
	}
	
	sgdb->url = strncat(sgdb->url,
	                    "https://www.steamgriddb.com/api/v2/search/autocomplete/cyberpunk",
	                    strlen("https://www.steamgriddb.com/api/v2/search/autocomplete/cyberpunk") + 1);
	sgdb->h_auth = strncat(sgdb->h_auth, "Authorization: Bearer ", strlen("Authorization: Bearer ") + 1);
	sgdb->h_auth = strncat(sgdb->h_auth, token, strlen(token) + 1);
	
	fprintf(stdout, "\n================ Json File =================\n");
	// Start of curl implementation
	// FIXME Find the memory leak (use --leak-check=full --show-leak-kinds=all)
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if(curl)
	{
		struct curl_slist* headers = NULL;
		
		curl_easy_setopt(curl, CURLOPT_URL, sgdb->url);
		
		headers = curl_slist_append (headers, sgdb->h_auth);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		
		res = curl_easy_perform(curl);
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		// Always cleanup
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	// End of curl implementation
	fprintf(stdout, "\n============================================\n");
	
	destruct_service(sgdb);
	fprintf (stdout, "\n");
	return EXIT_SUCCESS;
}
