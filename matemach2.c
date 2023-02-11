#include <stdio.h>
#include <stdint.h>

#include "sqlite3.h"
#include "mongoose.h"

void event_handler_cb(struct mg_connection *conn, int ev, void *ev_data, void *fn_data) {
	if (ev == MG_EV_POLL) {
		return;
	}


	printf("event_handler_cb: ev: %d\n", ev);
	
	if (ev == MG_EV_HTTP_MSG) {
		struct mg_http_message *request = ev_data;
		struct mg_http_serve_opts opts = {
			.root_dir = ".\\html",
			//.page404 = "404.html"
			//.ssi_pattern = "#.html",
		};
		mg_http_serve_dir(conn, request, &opts);

		printf("huh\n");
	}
	
}

int main(int argc, const char *argv[])
{

	char *listening_url = argc > 1 ? argv[1] : "0.0.0.0:27015";


	sqlite3 *db = NULL;
	int sresult = 0;
	sresult = sqlite3_open(".\\database\\matemach.db", &db);

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