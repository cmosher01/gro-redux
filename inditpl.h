L("<h1>%name%</h1>\n")
L("<p><span class=\"child-of\">child of %father% and %mother%</span></p>\n")



L("%events%\n")
	L("<table class=\"events\">\n")
	L("<caption>Events</caption>\n")
	L("<thead><tr><th>event</th><th>date</th><th>place</th><th>notes</th><th>source</th></tr></thead>\n")
	L("<tbody>\n")

	L("%event%\n")
		L("<tr>\n")
		L("<td><span class=\"event\">%type%</span></td>\n")
		L("<td><span class=\"date\">%date%</span></td>\n")
		L("<td><span class=\"place\">%place%</span></td>\n")
		L("<td><span class=\"notes\">%note%</span></td>\n")
		L("<td><span class=\"source\">%source%</span> <span class=\"citation\">%citation%</span></td>\n")
		L("</tr>\n")
	L("%/event%\n")

	L("</tbody>\n")
	L("</table>\n")
L("%/events%\n")



L("%attributes%\n")
	L("<p>&nbsp;</p>\n")
	L("<table class=\"attributes\">\n")
	L("<caption>Attributes</caption>\n")
	L("<thead><tr><th>attribute</th><th>value</th><th>date</th><th>place</th><th>notes</th><th>source</th></tr></thead>\n")
	L("<tbody>\n")

	L("%attribute%\n")
		L("<tr>\n")
		L("<td><span class=\"attribute\">%type%</span></td>\n")
		L("<td><span class=\"attribute-value\">%value%</span></td>\n")
		L("<td><span class=\"date\">%date%</span></td>\n")
		L("<td><span class=\"place\">%place%</span></td>\n")
		L("<td><span class=\"notes\">%note%</span></td>\n")
		L("<td><span class=\"source\">%source%</span> <span class=\"citation\">%citation%</span></td>\n")
		L("</tr>\n")
	L("%/attribute%\n")

	L("</tbody>\n")
	L("</table>\n")
L("%/attributes%\n")






L("%partnerships%\n")
	L("<p><span class=\"other-caption\">Partnerships</span></p>\n")

	L("%partnership%\n")
		L("<p><span class=\"partner\">%name%</span></p>\n")

		L("%events%\n")
			L("<table class=\"partner-events\">\n")
			L("<thead><tr><th>event</th><th>date</th><th>place</th><th>notes</th><th>source</th></tr></thead>\n")
			L("<tbody>\n")

			L("%event%\n")
				L("<tr>\n")
				L("<td><span class=\"event\">%type%</span></td>\n")
				L("<td><span class=\"date\">%date%</span></td>\n")
				L("<td><span class=\"place\">%place%</span></td>\n")
				L("<td><span class=\"notes\">%note%</span></td>\n")
				L("<td><span class=\"source\">%source%</span> <span class=\"citation\">%citation%</span></td>\n")
				L("</tr>\n")
			L("%/event%\n")

			L("</tbody>\n")
			L("</table>\n")
		L("%/events%\n")

		L("<p>&nbsp;</p>\n")

		L("%children%\n")
			L("<table class=\"children\">\n")
			L("<caption>children</caption>\n")
			L("<tbody>\n")

			L("%child%\n")
				L("<tr><td rowspan=\"2\"><span class=\"name\">%name%</span></td><td><span class=\"event\">birth</span></td><td><span class=\"date\">%birthdate%</span></td><td><span class=\"place\">%birthplace%</span></td></tr>\n")
				L("<tr><td><span class=\"event\">death</span></td><td><span class=\"date\">%deathdate%</span></td><td><span class=\"place\">%deathplace%</span></td></tr>\n")
			L("%/child%\n")

			L("</tbody>\n")
			L("</table>\n")
		L("%/children%\n")

	L("%/partnership%\n")

L("%/partnerships%\n")
