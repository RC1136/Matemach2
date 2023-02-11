#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "sqlite3.h"
#include "mongoose.h"

#define HTML_PATH ".\\html\\"

sqlite3 *db = NULL;

void event_handler_cb(struct mg_connection *conn, int ev, void *ev_data, void *fn_data) {
	if (ev == MG_EV_POLL) {
		return;
	}
	int sres = 0;

	printf("event_handler_cb: ev: %d\n", ev);
	
	if (ev == MG_EV_HTTP_CHUNK) {
		struct mg_http_message *request = ev_data;
		struct mg_http_serve_opts opts = {
			//.root_dir = ".\\html",
			.page404 = "404.html"
			//.ssi_pattern = "#.html",
		};
		/*
		if (mg_http_match_uri(request, "/index.html")) {
			mg_http_serve_file(conn, request, ".\\html\\index.html", &opts);
		}
		else {
			mg_http_serve_file(conn, request, ".\\html\\404.html", &opts);
		}
		*/


		if (mg_http_match_uri(request, "/")) {

			FILE *file = NULL;
			file = fopen(HTML_PATH "home.html", "r");
			assert(file);

			char *buf = NULL;
			char *divptr = NULL;
			uint32_t filesize = 0;
			
			fseek(file, 0, SEEK_END);
			filesize = ftell(file);
			fseek(file, 0, SEEK_SET);
			
			buf = malloc(filesize + 1);
			assert(buf);

			fread(buf, sizeof(*buf), filesize, file);
			buf[filesize] = '\0';
			

			divptr = strstr(buf, "class=\"board_list\"");
			assert(divptr);

			divptr = strstr(divptr, "</div>");
			assert(divptr);


			mg_http_reply(conn, 200,
				"Transfer-Encoding: chunked\r\n"
				"Content-Type: text/html\r\n"
			,"");


			mg_http_write_chunk(conn, buf, divptr - buf);
			//mg_send(conn, buf, divptr - buf);

			static const char sql_stmt[] = "SELECT * FROM Board;";

			sqlite3_stmt *stmt = NULL;
			char *err = NULL;
			sres = sqlite3_prepare(db, sql_stmt, strlen(sql_stmt), &stmt, &err);
			assert(sres == SQLITE_OK);

			while ((sres = sqlite3_step(stmt)) == SQLITE_ROW) {

				char *board_name = NULL, *board_url = NULL;

				for (int i = 0; i < sqlite3_column_count(stmt); i++) {
					if (strcmp(sqlite3_column_name(stmt, i), "name") == 0) {
						board_name = sqlite3_column_text(stmt, i);
					}
					else if (strcmp(sqlite3_column_name(stmt, i), "url") == 0) {
						board_url = sqlite3_column_text(stmt, i);
					}
				}
				assert(board_name);
				assert(board_url);
				
				mg_http_printf_chunk(conn, "<a href=\"%s\">%s</a><br>", board_url, board_name);
				//char tmpbuf[1024] = { 0 };
				//int len = sprintf(tmpbuf, "<a href=\"%s\">%s</a><br>", board_url, board_name);
				//mg_send(conn, tmpbuf, len);
			}
			mg_http_write_chunk(conn, divptr, strlen(divptr));
			//mg_send(conn, divptr, strlen(divptr));
			sres = sqlite3_finalize(stmt);
			free(buf);
			mg_http_write_chunk(conn, "", 0);
		}
		printf("huh\n");
	}
	
}

int main(int argc, const char *argv[])
{

	char *listening_url = argc > 1 ? argv[1] : "0.0.0.0:27015";


	
	int sres = 0;
	sres = sqlite3_open(".\\database\\matemach.db", &db);

	struct mg_mgr mgr = { 0 };

	mg_mgr_init(&mgr);


	struct mg_connection *conn = NULL;
	mg_event_handler_t cb = event_handler_cb;
	conn = mg_http_listen(&mgr, listening_url, cb, NULL);


	while (1) {
		mg_mgr_poll(&mgr, 1000);
	}

	return 0;
}