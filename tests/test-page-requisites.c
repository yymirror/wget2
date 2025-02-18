/*
 * Copyright (c) 2013 Tim Ruehsen
 * Copyright (c) 2015-2024 Free Software Foundation, Inc.
 *
 * This file is part of Wget
 *
 * Wget is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Wget is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Wget  If not, see <https://www.gnu.org/licenses/>.
 *
 *
 * Testing --page-requisites
 *
 * Changelog
 * 27.05.2014  Tim Ruehsen  created
 *
 */

#include <config.h>

#include <stdlib.h> // exit()
#include "libtest.h"

int main(void)
{
	wget_test_url_t urls[]={
		{	.name = "/index.html",
			.code = "200 Dontcare",
			.body =
				"<html><head><title>Main Page</title></head><body><p>A link to a" \
				" <a href=\"http://localhost:{{port}}/secondpage.html\">second page</a>." \
				" Hey, a picture <img src=\"picture.png\"/>." \
				" Hey, a srcset <img srcset=\"picture1.png, picture2.png 150w,picture3.png 100x\"/>." \
				"<link rel=\"stylesheet\" href=\"test1.css\" />"
				"<link href=\"test2.css\" rel=\"stylesheet\" />"
				"<link rel=\"shortcut icon\" href=\"myfavicon.ico\" />"
				"<link rel=\"icon\" href=\"icon.ico\" />"
				"<link rel=\"alternate icon\" href=\"icon2.ico\" />"
				"<link rel=\"alternate icon whatever\" href=\"icon3.ico\" />"
				"<link rel=\" icon whatever\" href=\"icon4.ico\" />"
				"<link rel=\"wontdownload\" href=\"wontdownload.txt\" />"
				"<link href=\"not.txt\" rel=\"whatever\" />"
				"<link href=\"preload.css\" rel=\"preload\" as=\"style\"/>"
			"</p></body></html>",
			.headers = {
				"Content-Type: text/html",
			}
		},
		{	.name = "/secondpage.html",
			.code = "200 Dontcare",
			.body =
				"<html><head><title>Second Page</title></head><body><p>A link to a" \
				" <a href=\"http://localhost:{{port}}/thirdpage.html\">third page</a>." \
				" Also, a <a href=\"http://localhost:{{port}}/nonexistent\">broken link</a>." \
				"</p></body></html>",
			.headers = {
				"Content-Type: text/html",
				"Content-Disposition: attachment; filename=\"filename.html\"",
			}
		},
		{	.name = "/thirdpage.html",
			.code = "200 Dontcare",
			.body =
				"<html><head><title>Third Page</title></head><body><p>A link to a" \
				" <a href=\"http://localhost:{{port}}/dummy.txt\">text file</a>." \
				" Also, a <a href=\"http://localhost:{{port}}/againnonexistent\">broken link</a>." \
				"</p></body></html>",
			.headers = {
				"Content-Type: text/html",
			}
		},
		{	.name = "/firstlevel/index.html",
			.code = "200 Dontcare",
			.body =
				"<html><head><title>Main Page</title></head><body><p>A link to a" \
				" <a href=\"/secondpage.html\">second page</a>." \
				" Hey, a picture <img src=\"../picture.png\"/>." \
				" Hey, a srcset <img srcset=\"../picture1.png, /picture2.png 150w,http://localhost:{{port}}/picture3.png 100x\"/>." \
				"<link rel=\"stylesheet\" href=\"/test1.css\" />"
				"<link href=\"../test2.css\" rel=\"stylesheet\" />"
				"<link rel=\"shortcut icon\" href=\"http://localhost:{{port}}/myfavicon.ico\" />"
				"<link href=\"/not.txt\" rel=\"whatever\" />"
				"<link href=\"../preload.css\" rel=\"preload\" as=\"style\"/>"
			"</p></body></html>",
			.headers = {
				"Content-Type: text/html",
			}
		},
		{	.name = "/picture.png",
			.code = "200 Dontcare",
			.body = "PNG data",
			.headers = {
				"Content-Type: image/png",
			}
		},
		{	.name = "/picture1.png",
			.code = "200 Dontcare",
			.body = "PNG  descriptor 1",
			.headers = {
				"Content-Type: image/png",
			}
		},
		{	.name = "/picture2.png",
			.code = "200 Dontcare",
			.body = "PNG descriptor 2",
			.headers = {
				"Content-Type: image/png",
			}
		},
		{	.name = "/picture3.png",
			.code = "200 Dontcare",
			.body = "PNG descriptor 3",
			.headers = {
				"Content-Type: image/png",
			}
		},
		{	.name = "/dummy.txt",
			.code = "200 Dontcare",
			.body = "What ever",
			.headers = {
				"Content-Type: text/plain",
			}
		},
		{	.name = "/test1.css",
			.code = "200 Dontcare",
			.body = "CSS1",
			.headers = {
				"Content-Type: text/css",
			}
		},
		{	.name = "/test2.css",
			.code = "200 Dontcare",
			.body = "CSS2",
			.headers = {
				"Content-Type: text/css",
			}
		},
		{	.name = "/myfavicon.ico",
			.code = "200 Dontcare",
			.body = "MYFAVICON",
			.headers = {
				"Content-Type: image/x-icon",
			}
		},
		{	.name = "/not.txt",
			.code = "200 Dontcare",
			.body = "Not to be followed",
			.headers = {
				"Content-Type: text/plain",
			}
		},
		{	.name = "/preload.css",
			.code = "200 Dontcare",
			.body =
				"@import url(\"/import1.css\");\n" \
				"@import \"import2.css\" screen;\n" \
				"body {background:url(background.jpg);}",
			.headers = {
				"Content-Type: text/css",
			}
		},
		{	.name = "/import1.css",
			.code = "200 Dontcare",
			.body = "IMPORT1",
			.headers = {
				"Content-Type: text/css",
			}
		},
		{	.name = "/import2.css",
			.code = "200 Dontcare",
			.body = "IMPORT2",
			.headers = {
				"Content-Type: text/css",
			}
		},
		{	.name = "/background.jpg",
			.code = "200 Dontcare",
			.body = "BACKGROUND",
			.headers = {
				"Content-Type: image/jpg",
			}
		},
		{	.name = "/icon.ico",
			.code = "200 Dontcare",
			.body = "ICON",
			.headers = {
				"Content-Type: image/x-icon",
			}
		},
		{	.name = "/icon2.ico",
			.code = "200 Dontcare",
			.body = "ICON2",
			.headers = {
				"Content-Type: image/x-icon",
			}
		},
		{	.name = "/icon3.ico",
			.code = "200 Dontcare",
			.body = "ICON3",
			.headers = {
				"Content-Type: image/x-icon",
			}
		},
		{	.name = "/icon4.ico",
			.code = "200 Dontcare",
			.body = "ICON4",
			.headers = {
				"Content-Type: image/x-icon",
			}
		},
	};

	// functions won't come back if an error occurs
	wget_test_start_server(
		WGET_TEST_RESPONSE_URLS, &urls, countof(urls),
		WGET_TEST_FEATURE_MHD,
		0);

	// test--page-requisites
	wget_test(
//		WGET_TEST_KEEP_TMPFILES, 1,
//		WGET_TEST_EXECUTABLE, "wget",
		WGET_TEST_OPTIONS, "--page-requisites -nH",
		WGET_TEST_REQUEST_URL, "index.html",
		WGET_TEST_EXPECTED_ERROR_CODE, 0,
		WGET_TEST_EXPECTED_FILES, &(wget_test_file_t []) {
			{ urls[0].name + 1, urls[0].body },
			{ urls[4].name + 1, urls[4].body },
			{ urls[5].name + 1, urls[5].body },
			{ urls[6].name + 1, urls[6].body },
			{ urls[7].name + 1, urls[7].body },
			{ urls[9].name + 1, urls[9].body },   // test1.css
			{ urls[10].name + 1, urls[10].body },   // test2.css
			{ urls[11].name + 1, urls[11].body }, // myfavicon.ico
			{ urls[13].name + 1, urls[13].body }, // preload.css
			{ urls[14].name + 1, urls[14].body }, // import1.css
			{ urls[15].name + 1, urls[15].body }, // import2.css
			{ urls[16].name + 1, urls[16].body }, // background.css
			{ urls[17].name + 1, urls[17].body }, // icon.ico
			{ urls[18].name + 1, urls[18].body }, // icon2.ico
			{ urls[19].name + 1, urls[19].body }, // icon3.ico
			{ urls[20].name + 1, urls[20].body }, // icon4.ico
			{	NULL } },
		0);

	// test--page-requisites (with no-parent)
	wget_test(
		WGET_TEST_OPTIONS, "--page-requisites -np -nH",
		WGET_TEST_REQUEST_URL, "firstlevel/index.html",
		WGET_TEST_EXPECTED_ERROR_CODE, 0,
		WGET_TEST_EXPECTED_FILES, &(wget_test_file_t []) {
			{ urls[3].name + 1, urls[3].body },
			{ urls[4].name + 1, urls[4].body },
			{ urls[5].name + 1, urls[5].body },
			{ urls[6].name + 1, urls[6].body },
			{ urls[7].name + 1, urls[7].body },
			{ urls[9].name + 1, urls[9].body },   // test1.css
			{ urls[10].name + 1, urls[10].body },   // test2.css
			{ urls[11].name + 1, urls[11].body }, // myfavicon.ico
			{ urls[13].name + 1, urls[13].body }, // preload.css
			{ urls[14].name + 1, urls[14].body }, // import1.css
			{ urls[15].name + 1, urls[15].body }, // import2.css
			{ urls[16].name + 1, urls[16].body }, // background.css
			{	NULL } },
		0);

	exit(EXIT_SUCCESS);
}
