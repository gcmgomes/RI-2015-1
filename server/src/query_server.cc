#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "../../util/page.h"
#include "../html_handler.h"
#include "../mongoose.h"

using namespace std;

string html_form;
server::HtmlHandler *html_handler = nullptr;

static void send_reply(struct mg_connection *conn) {
  char query[500], var2[500], var3[500], var4[500];
  unsigned model = 0;
  double beta = 0.5;
  bool use_dl_norm = true;

  if (strcmp(conn->uri, "/handle_get_request") == 0) {
    // User has submitted a form, show submitted data and a variable value
    // Parse form data. var1 and var2 are guaranteed to be NUL-terminated
    mg_get_var(conn, "query", query, sizeof(query));
    mg_get_var(conn, "model", var2, sizeof(var2));
    mg_get_var(conn, "length_normalize", var3, sizeof(var3));
    mg_get_var(conn, "beta", var4, sizeof(var4));

    sscanf(var3, "%u", &model);
    if(!model) {
      use_dl_norm = false;
    }

    sscanf(var2, "%u", &model);
    sscanf(var4, "%lf", &beta);


    // Send reply to the client, showing submitted form values.
    // POST data is in conn->content, data length is in conn->content_len
    mg_send_header(conn, "Content-Type", "text/html");
    std::string results = "";
    html_handler->QueryPages(query, model, beta, use_dl_norm, results);
    mg_printf_data(conn, "%s", results.c_str());
  } else {
    // Show HTML form.
    mg_send_data(conn, html_handler->standard_home_page().c_str(),
                 html_handler->standard_home_page().size());
  }
}

static int ev_handler(struct mg_connection *conn, enum mg_event ev) {
  if (ev == MG_REQUEST) {
    send_reply(conn);
    return MG_TRUE;
  } else if (ev == MG_AUTH) {
    return MG_TRUE;
  } else {
    return MG_FALSE;
  }
}

int main(int argc, char **argv) {
  if (argc < 7) {
    cout << argv[0]
         << " [home page] [results page] [vocabulary file] [binary index file] "
            "[binary anchor index file] [rankng metadata file]" << endl;
    return -1;
  }

  std::cout << "Initializing service" << std::endl;
  html_handler = new server::HtmlHandler(argv[1], argv[2], argv[3], argv[4],
                                         argv[5], argv[6]);

  struct mg_server *server = mg_create_server(NULL, ev_handler);

  mg_set_option(server, "listening_port", "8080");

  printf("Starting on port %s\n", mg_get_option(server, "listening_port"));
  for (;;) {
    mg_poll_server(server, 100000);
  }

  mg_destroy_server(&server);

  delete html_handler;
  return 0;
}
