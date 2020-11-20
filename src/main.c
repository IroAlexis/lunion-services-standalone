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


#define TRUE  0
#define FALSE 1
#define USERAGENT     "User-Agent: lunion"
#define AUTHORIZATION "Authorization: Bearer "


// Function name: <service>_<action>_<text>


//------ Service name
// 1 Github:      https://api.github.com/repos/
// 2 Gitlab:      https://gitlab.example.com/api/v4/projects
// 3 SteamGridDB: https://www.steamgriddb.com/api/v2/
//------
typedef struct
{
	int   name;   // Number designed the service name
	char* url;    // URL service
	int   req;    // Need headers Authorization or not
	char* h_auth; // Headers Authorization
} service;


service* init_service()
{
	service* s = (service*) calloc(1, sizeof(service));
	if (NULL == s)
		return NULL;
	
	// Initialize fields
	s->name = -1;
	s->url = (char*) calloc(512, sizeof(char));
	if (NULL == s->url)
	{
		free(s);
		return NULL;
	}
	s->req = -1;
	s->h_auth = (char*) calloc(64, sizeof(char));
	if (NULL == s->h_auth)
	{
		free(s->url);
		free(s);
		return NULL;
	}
	return s;
}


int fill_service(service* s, int name, char* addr)
{
	s->name = name;
	s->url = strncpy(s->url, addr, strlen(addr) + 1);
	if (NULL == s->url)
		return EXIT_FAILURE;
	
	// Flag for determinate if we need a mandatory authentification
	//------
	// 1 Github:      https://api.github.com/repos/
	// 2 Gitlab:      https://gitlab.example.com/api/v4/projects
	// 3 SteamGridDB: https://www.steamgriddb.com/api/v2/
	//------
	switch (name)
	{
		case 1:
			s->req = FALSE;
			break;
		case 2:
			s->req = FALSE;
			break;
		case 3:
			s->req = TRUE;
			break;
	}
	
	return EXIT_SUCCESS;
}


char* get_token(service* s)
{
	char* token = NULL;
	
	token = (char*) calloc(64, sizeof(char));
	if (NULL == token)
		return NULL;
	
	switch (s->name)
	{
		case 1:
			break;
		case 2:
			break;
		case 3:
			fprintf(stdout,
			        "Please enter your API key ( https://www.steamgriddb.com/profile/preferences ): ");
			break;
	}
	fflush(stdout);
	
	fscanf(stdin, " %s", token);
	return token;
}


void destruct_service(service* s)
{
	free(s->h_auth);
	free(s->url);
	free(s);
}


int verif_token(service* s, char* token)
{
	int nbr = strlen(token);
	
	fprintf(stdout, "token validity: ");
	fflush(stdout);
	
	switch (s->name)
	{
		case 1:
			fprintf(stdout, "\033[1;32mOK\033[0m (not implemented)\n");
			break;
		case 2:
			fprintf(stdout, "\033[1;32mOK\033[0m (not implemented)\n");
			break;
		case 3:
			if (nbr != 32)
			{
				fprintf(stdout, "\033[1;31mFAILED\033[0m\n");
				return EXIT_FAILURE;
			}
			fprintf(stdout, "\033[1;32mOK\033[0m\n");
			break;
	}
	
	return EXIT_SUCCESS;
}


int main(int argc, char* argv[])
{
	int      ret;
	CURL*    curl;
	CURLcode res;
	service* serv = NULL;
	char*    token = NULL;
	
	// Initialization service
	serv = init_service();
	if (NULL == serv)
		return EXIT_FAILURE;
	
	ret = fill_service(serv, 3, "https://www.steamgriddb.com/api/v2/search/autocomplete/cyberpunk");
	if (ret == EXIT_FAILURE)
	{
		destruct_service(serv);
		return EXIT_FAILURE;
	}
	
	/*s->url = strncat(s->url,
	                   "https://www.steamgriddb.com/api/v2/search/autocomplete/cyberpunk",
	                   strlen("https://www.steamgriddb.com/api/v2/search/autocomplete/cyberpunk") + 1);
	s->url = strncat(s->url,
	                 "https://api.github.com/repos/IroAlexis/gogextract/releases",
	                 strlen("https://api.github.com/repos/IroAlexis/gogextract/releases") + 1);
	 */
	
	fprintf(stdout, "\n================ Json File =================\n");
	// Start of curl implementation
	// FIXME Find the memory leak (use --leak-check=full --show-leak-kinds=all)
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if(curl)
	{
		struct curl_slist* headers = NULL;
		
		curl_easy_setopt(curl, CURLOPT_URL, serv->url);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, USERAGENT);
		
		if (serv->req == TRUE)
		{
			token = get_token(serv);
			if (verif_token(serv, token))
			{
				// Always cleanup
				curl_easy_cleanup(curl);
				curl_global_cleanup();
				
				destruct_service(serv);
				free(token);
				
				return EXIT_FAILURE;
			}
			
			serv->h_auth = strncat(serv->h_auth, AUTHORIZATION, strlen(AUTHORIZATION) + 1);
			serv->h_auth = strncat(serv->h_auth, token, strlen(token) + 1);
			fprintf(stderr, "[DEBUG] %s\n", serv->h_auth);
			free(token);
			
			headers = curl_slist_append(headers, serv->h_auth);
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		}
		
		res = curl_easy_perform(curl);
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		// Always cleanup
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	// End of curl implementation
	fprintf(stdout, "\n============================================\n");
	
	destruct_service(serv);
	fprintf (stdout, "\n");
	return EXIT_SUCCESS;
}
