<%
Private Function DataField(s)
	if len(s)=0 then
		DataField = "&nbsp;"
	else
		DataField = s
	end if
End Function
%>

<%
Private Function DataLink(sType,iObj,sTag)
	if iObj>=0 then
		Response.Write _
			"<A HREF=" & chr(34) & Request.ServerVariables("SCRIPT_NAME") & _
			"?docid=" & docid & "&" & sType & "=" & iObj & chr(34) & ">" & _
			sTag & "</A>" & vbNewLine
	else
		Response.Write "&nbsp;" & vbNewLine
	end if
End Function
%>

<%
Private Function DataLinkUnknown(sType,iObj,sTag)
	if iObj>=0 then
		Response.Write _
			"<A HREF=" & chr(34) & Request.ServerVariables("SCRIPT_NAME") & _
			"?docid=" & docid & "&" & sType & "=" & iObj & chr(34) & ">" & _
			"<B>" & sTag & "</B></A>" & vbNewLine
	else
		Response.Write "[unknown]" & vbNewLine
	end if
End Function
%>

<%
Private Function ChildBold(b,bBold)
	if b=true then
		if bBold=true then
			ChildBold = "<B>"
		else
			ChildBold = "</B>"
		end if
	else
		ChildBold = s
	end if
End Function
%>

<%
	Dim coMy
	set coMy = Server.CreateObject("Groinproc.GroGetData")

	Dim sMe
	sMe = Request.ServerVariables("SCRIPT_NAME")

	Dim rs
	Dim docpage
	Dim docid
	Dim iLevel
	Dim iBase
	Dim iIndi
	Dim iSour
	docpage = false
	iLevel = -1
	iBase = -1
	iIndi = -1
	iSour = -1
	if Request.QueryString.Count = 0 then
		' no query string specified, so show list of GEDCOM docs
		set rs = coMy.GetDocs()
		docpage = true
	else
		if Request.QueryString("docid").Count > 0 then
			docid = Request.QueryString("docid")
		end if
		if Request.QueryString("iLevel").Count > 0 then
			iLevel = Request.QueryString("iLevel")
		end if
		if Request.QueryString("iBase").Count > 0 then
			iBase = Request.QueryString("iBase")
		end if
		if Request.QueryString("iIndi").Count > 0 then
			iIndi = Request.QueryString("iIndi")
		end if
		if Request.QueryString("iSour").Count > 0 then
			iSour = Request.QueryString("iSour")
		end if
	end if

	if iIndi >= 0 then
		set rs = coMy.GetIndi(docid,iIndi)
	end if
	if iLevel >= 0 then
		set rs = coMy.GetIndex(docid,iLevel,iBase)
	end if
	if iSour >= 0 then
		set rs = coMy.GetSource(docid,iSour)
	end if
	rs.StayInSync = true

	Dim sTitle
	if iIndi >= 0 then
		Dim rsEvts
		set rsEvts = Server.CreateObject("ADODB.Recordset")
		rsEvts = rs.Fields.Item("events")

		Dim rsAttrs
		set rsAttrs = Server.CreateObject("ADODB.Recordset")
		rsAttrs = rs.Fields.Item("attributes")

		Dim rsParts
		set rsParts = Server.CreateObject("ADODB.Recordset")
		rsParts = rs.Fields.Item("partners")
		rsParts.StayInSync = true

		Dim rsPEvts
		set rsPEvts = Server.CreateObject("ADODB.Recordset")

		Dim rsChildren
		set rsChildren = Server.CreateObject("ADODB.Recordset")

		sTitle = rs.Fields("name")
	end if

	if iSour >= 0 then
		sTitle = rs.Fields("title")
	end if

	Dim iPrev
	Dim iNext
	Dim bLeaf
	if iLevel >= 0 then
		iPrev = rs.Fields("iPrev")
		iNext = rs.Fields("iNext")
		bLeaf = rs.Fields("name2").ActualSize = 0
	end if
%>

<HTML>
<HEAD>
<TITLE><% = sTitle %></TITLE>
<STYLE TYPE="text/css">
<!--
BODY
{
	font-family: Arial, helvetica, sans-serif;
	background-color: #FFFFCC;
}
TH
{
	background-color: #9999CC;
}
TABLE.datasheet
{
	background-color: #666633;
}
TABLE.datasheet TD
{
	padding: 1px;
	background-color: #CCCC99;
}
TABLE.indexouter
{
	padding: 6px;
}
TD.source
{
	padding-right: 3px;
}
TD.indexnames
{
	border-bottom: 1px solid #666633;
}
TD.next
{
}
TD.indexthru
{
	font-size: 75%;
	border-bottom: 1px solid #666633;
}
TD.name
{
	border-bottom: 1px solid #666633;
}
TD.dates
{
	font-size: 75%;
	padding: 0px;
	border-bottom: 1px solid #666633;
}
TD.datesend
{
	font-size: 75%;
	padding: 0px;
	padding-right: 6px;
	border-bottom: 1px solid #666633;
}
-->
</STYLE>
</HEAD>
<BODY>
<% if docpage then %>
<% if not (rs.EOF = false) then %>
There are no GEDCOM documents currently available on this GRO server.
Please check back soon
<% else %>
The following GEDCOM documents are currently available on this GRO server.
Please choose one to go to the index of its people.
<P>
<TABLE cellspacing="1">
<% while (rs.EOF = false) %>
	<TR>
		<TD><B><A HREF="default.asp?docid=<% = rs.Fields("id") %>&iLevel=0&iBase=0"><% = rs.Fields("title") %></A></B></TD>
	</TR>
	<% rs.MoveNext %>
<% wend %>
</TABLE>
<% end if %>
<% else %>


<% if iIndi >= 0 then %>
(<A HREF="<% = sMe %>">home</A>)

<!-- PERSON PAGE -->



<!-- name -->
<H1><% = rs.Fields("name") %></H1>



<!-- parents -->
child of
<% = DataLinkUnknown("iIndi",rs.Fields("iFather"),rs.Fields("nameFather")) %>
and
<% = DataLinkUnknown("iIndi",rs.Fields("iMother"),rs.Fields("nameMother")) %>





<% if rs.Fields("bPrivate") and Request.Form("password") <> coMy.GetPassword() then %>
<HR>Access to this information is restricted.<BR>Please enter your password to gain access:
<FORM action="<% = Request.ServerVariables("SCRIPT_NAME") %>?docid=<% = docid %>&iIndi=<% = iIndi %>" name="auth" method="post">
	<INPUT type="password" name="password">
	<INPUT type="submit" value="OK">
</FORM>
<% else %>

<!-- events -->
<% if not (rsEvts.EOF = true) then %>
	<H2>Events</H2>
	<TABLE class="datasheet" cellspacing="1">
	<TR><TH>event</TH><TH>date</TH><TH>place</TH><TH>notes</TH><TH width=20%>source</TH></TR>
	<% while not (rsEvts.EOF = true) %>
		<TR>
			<TD><B><% = DataField(rsEvts.Fields("name")) %></B></TD>
			<TD nowrap><% = DataField(rsEvts.Fields("date")) %></TD>
			<TD><% = DataField(rsEvts.Fields("place")) %></TD>
			<TD><% = DataField(rsEvts.Fields("notes")) %></TD>
			<TD><FONT size="1"><CITE>
				<% = DataLink("iSour",rsEvts.Fields("iSource"),rsEvts.Fields("source")) %>
				<% if rsEvts.Fields("citationtext").ActualSize > 0 then %>
					"<% = rsEvts.Fields("citationtext") %>"
				<% end if %>
			</CITE></FONT></TD>
		</TR>
		<% rsEvts.MoveNext %>
	<% wend %>
	</TABLE>
<% end if %>



<!-- attributes -->
<% if not (rsAttrs.EOF = true) then %>
	<H2>Attributes</H2>
	<TABLE class="datasheet" cellspacing="1">
	<TR><TH>attribute</TH><TH>value</TH><TH>date</TH><TH>place</TH><TH>notes</TH><TH width=20%>source</TH></TR>
	<% while not (rsAttrs.EOF = true) %>
		<TR>
			<TD><B><% = DataField(rsAttrs.Fields("name")) %></B></TD>
			<TD><% = DataField(rsAttrs.Fields("value")) %></TD>
			<TD nowrap><% = DataField(rsAttrs.Fields("date")) %></TD>
			<TD><% = DataField(rsAttrs.Fields("place")) %></TD>
			<TD><% = DataField(rsAttrs.Fields("notes")) %></TD>
			<TD><FONT size="1"><CITE>
				<% = DataLink("iSour",rsAttrs.Fields("iSource"),rsAttrs.Fields("source")) %>
				<% if rsAttrs.Fields("citationtext").ActualSize > 0 then %>
					"<% = rsAttrs.Fields("citationtext") %>"
				<% end if %>
			</CITE></FONT></TD>
		</TR>
		<% rsAttrs.MoveNext %>
	<% wend %>
	</TABLE>
<% end if %>

<!-- partnerships -->
<% if not (rsParts.EOF = true) then %>
	<H2>Partnerships</H2>
	 <% while not (rsParts.EOF = true) %>
		<! -- name -->
		<H4>
			partner:
			<A HREF="<% = sMe %>?docid=<% = docid %>&iIndi=<% = rsParts.Fields("iPartner") %>">
			<% = rsParts.Fields("name") %></A>
		</H4>

		<TABLE><TR><TD STYLE="background-color: #FFFFCC;">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</TD>
		<TD STYLE="background-color: #FFFFCC;">
		<! -- events -->
		<% rsPEvts = rsParts.Fields.Item("events") %>
		<% if not (rsPEvts.EOF = true) then %>
			<TABLE class="datasheet" cellspacing="1">
				<TR><TH>event</TH><TH>date</TH><TH>place</TH><TH>notes</TH><TH width=20%>source</TH></TR>
				<% while not (rsPEvts.EOF = true) %>
					<TR>
						<TD><B><% = DataField(rsPEvts.Fields("name")) %></B></TD>
						<TD nowrap><% = DataField(rsPEvts.Fields("date")) %></TD>
						<TD><% = DataField(rsPEvts.Fields("place")) %></TD>
						<TD><% = DataField(rsPEvts.Fields("notes")) %></TD>
						<TD><FONT size="1"><CITE>
							<% = DataLink("iSour",rsPEvts.Fields("iSource"),rsPEvts.Fields("source")) %>
							<% if rsPEvts.Fields("citationtext").ActualSize > 0 then %>
								"<% = rsPEvts.Fields("citationtext") %>"
							<% end if %>
						</CITE></FONT></TD>
					</TR>
					<% rsPEvts.MoveNext %>
				<% wend %>
			</TABLE>
		<% end if %>

		<! -- children -->
		<BR>
		<% rsChildren = rsParts.Fields.Item("children") %>
		<% if not (rsChildren.EOF = true) then %>
			<B>children:</B>
			<TABLE class="datasheet" cellspacing="1">
				<% while not (rsChildren.EOF = true) %>
					<TR>
						<TD nowrap>
							<% = ChildBold(rsChildren.Fields("bMore"),true) %>
							<A HREF="<% = sMe %>?docid=<% = docid %>&iIndi=<% = rsChildren.Fields("iChild") %>">
							<% = rsChildren.Fields("name") %></A>
							<% = ChildBold(rsChildren.Fields("bMore"),false) %>
						</TD>
					</TR>
					<% rsChildren.MoveNext %>
				<% wend %>
			</TABLE>
		<% end if %>
		</TD></TR></TABLE>

		<% rsParts.MoveNext %>
	<% wend %>
<% end if %>



<%
	set rsChildren = Nothing
	set rsPEvts = Nothing
	set rsParts = Nothing
	set rsAttrs = Nothing
	set rsEvts = Nothing
%>


<% end if 'private %>

<% else %>
<% if iSour >= 0 then %>

(<A HREF="<% = sMe %>">home</A>)

<!-- SOURCE PAGE -->

<%
	Dim sAuthor
	sAuthor = rs.Fields("author")
	if right(sAuthor,1) = "." then
		sAuthor = left(sAuthor,len(sAuthor)-1)
	end if
%>

<%
	Dim sPubl
	sPubl = rs.Fields("publ")
	if left(sPubl,5) = "http:" then
		sPubl = "<A HREF=" & chr(34) & sPubl & chr(34) & ">" & sPubl & "</A>"
	end if
%>
<P>
<TABLE>
	<TR><TD class="source" align="right" valign="bottom"><SPAN STYLE="color: gray; font-size: 75%">author:</SPAN></TD><TD class="source"><B><% = sAuthor %>.</TD></TR>
	<TR><TD class="source" align="right" valign="bottom"><SPAN STYLE="color: gray; font-size: 75%">title:</SPAN></TD><TD class="source"><B><U><% = rs.Fields("title") %></U>.</B></TD></TR>
	<TR><TD class="source" align="right" valign="bottom"><SPAN STYLE="color: gray; font-size: 75%">publ.:</SPAN></TD><TD class="source"><SPAN STYLE="font-size: 75%">(<% = sPubl %>).</SPAN></TD></TR>
</TABLE>

<% repos = rs.Fields("repository") %>
<% if len(repos) > 0 then %>
		(located at <% = repos %>, <% = rs.Fields("address") %>.)
<% end if %>

<% if rs.Fields("text").ActualSize > 0 then %>
		<P><SPAN STYLE="color: gray; font-size: 75%">actual text from source:</SPAN>
		<HR>
		<% = rs.Fields("text") %>
		<HR>
<% end if %>

<% else %>
<% if iLevel >= 0 then %>




<!-- INDEX PAGE -->

<% if iLevel = 0 then %>
This web site is generated by Genealogy Research Organizer (GRO).<BR>
This is the index of all people currently in the database.<BR>
Click on a range of people to drill-down to the next level.
<P><P>
<% end if %>
<TABLE class="indexouter" cellspacing="0">
<TR>
<% if iLevel>0 then %>
<TD>(<A HREF="<% = sMe %>?docid=<% = docid %>&iLevel=0&iBase=0">top level</A>)</TD>
<% end if %>
<% if iLevel>1 then %>
<TD>(<A HREF="<% = sMe %>?docid=<% = docid %>&iLevel=<% = iLevel-1 %>&iBase=<% = rs.Fields("iUp") %>">one level up</A>)</TD>
<% end if %>
<TD>

<TABLE cellpadding="6" cellspacing="0">
<TR>
<% if iPrev >= 0 then %>
<TD class="indexnames">(<A HREF="<% = sMe %>?docid=<% = docid %>&iLevel=<% = iLevel %>&iBase=<% = iPrev %>">previous</A>)</TD>
<% else %>
<% if iLevel > 0 then %>
<TD class="indexnames">(beginning of index)</TD>
<% else %>
<TD class="indexnames">&nbsp;</TD>
<% end if %>
<% end if %>
<TD class="indexnames">&nbsp;</TD>
<TD class="indexnames">&nbsp;</TD>
<% if bLeaf then %>
<TD class="indexnames">&nbsp;</TD>
<TD class="indexnames">&nbsp;</TD>
<TD class="indexnames">&nbsp;</TD>
<% end if %>
</TR>

<% while not (rs.EOF = true) %>
		<% if bLeaf then %>
			<%
				Dim sBirth
				if rs.Fields("birth") > 0 then
					sBirth = rs.Fields("birth")
				else
					sBirth = "?"
				end if
				Dim sDeath
				if rs.Fields("death") > 0 then
					sDeath = rs.Fields("death")
				else
					sDeath = "?"
				end if
			%>
			<TR>
			<TD class="name"><A HREF="<% = sMe %>?docid=<% = docid %>&iIndi=<% = rs.Fields("iBase") %>"><% = rs.Fields("name1") %></A></TD>
			<TD class="dates" align="right">(</TD>
			<TD class="dates" align="center"><% = sBirth %></TD>
			<TD class="dates" align="center">&mdash;</TD>
			<TD class="dates" align="center"><% = sDeath %></TD>
			<TD class="datesend" align="left">)</TD>
			</TR>
		<% else %>
			<TR>
			<TD class="indexnames" valign="bottom"><A HREF="<% = sMe %>?docid=<% = docid %>&iLevel=<% = iLevel+1 %>&iBase=<% = rs.Fields("iBase") %>"><% = rs.Fields("name1") %></A></TD>
			<TD class="indexthru" valign="bottom"><A HREF="<% = sMe %>?docid=<% = docid %>&iLevel=<% = iLevel+1 %>&iBase=<% = rs.Fields("iBase") %>">through</A></TD>
			<TD class="indexnames" valign="bottom"><A HREF="<% = sMe %>?docid=<% = docid %>&iLevel=<% = iLevel+1 %>&iBase=<% = rs.Fields("iBase") %>"><% = rs.Fields("name2") %></A></TD>
			</TR>
		<% end if %>
	<% rs.MoveNext %>
<% wend %>
<TR>
<% if iNext >= 0 then %>
<TD class="next">(<A HREF="<% = sMe %>?docid=<% = docid %>&iLevel=<% = iLevel %>&iBase=<% = iNext %>">next</A>)</TD>
<% else %>
<% if iLevel > 0 then %>
<TD class="next">(end of index)</TD>
<% end if %>
<% end if %>
<TD class="next">&nbsp;</TD>
<TD class="next">&nbsp;</TD>
<% if bLeaf then %>
<TD class="next">&nbsp;</TD>
<TD class="next">&nbsp;</TD>
<TD class="next">&nbsp;</TD>
<% end if %>
</TR>
</TABLE>
</TD>
</TR>
</TABLE>




<% end if %>
<% end if %>
<% end if %>
<% end if %>
<P><P><FONT size="1">This page was generated by <A HREF="http://go.to/gro">Genealogy Research Organizer</A>.</FONT>
<%
	set rs = Nothing
	set coMy = Nothing
%>

</BODY>
</HTML>
