<HTML>

<HEAD>
<TITLE>%name%</TITLE>
<STYLE TYPE="text/css">
<!--
BODY
{
	font-family: Arial, helvetica, sans-serif;
	font-size: 75%;
	background-color: #FFFFCC;
}
TH
{
	font-size: 75%;
	background-color: #9999CC;
}
TD
{
	font-size: 70%;
	background-color: #CCCC99;
}
-->
</STYLE>
</HEAD>

<BODY>
<H1>%name%</H1>

child of <B>%father%</B> and <B>%mother%</B>

%events%
<P><H3>events</H3>
<TABLE BORDER=1 CELLSPACING=0>
<TR><TH>event</TH><TH>date</TH><TH>place</TH><TH>notes</TH><TH width=20%>source</TH></TR>
%event%
<TR>
<TD><B>%type%</B></TD>
<TD NOWRAP>%date%</TD>
<TD>%place%</TD>
<TD>%note%</TD>
<TD><FONT STYLE="{font-size:80%;}"><CITE>%citation%</CITE></FONT></TD>
</TR>
%/event%
</TABLE>
%/events%

%attributes%
<P><H3>attributes</H3>
<TABLE BORDER=1 CELLSPACING=0>
<TR><TH>attribute</TH><TH>value</TH><TH>date</TH><TH>place</TH><TH>notes</TH><TH width=20%>source</TH></TR>
%attribute%
<TR>
<TD><B>%type%</B></TD>
<TD><B>%value%</B></TD>
<TD NOWRAP>%date%</TD>
<TD>%place%</TD>
<TD>%note%</TD>
<TD><FONT STYLE="{font-size:80%;}"><CITE>%citation%</CITE></FONT></TD>
</TR>
%/attribute%
</TABLE>
%/attributes%

%partnerships%
<P><H3>partnerships</H3>
%partnership%
<P><B>partner: </B>%name%
%events%
<P><B>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;partnership events</B>
<TABLE><TR><TD style="{background-color: #FFFFCC;}">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</TD><TD>
<TABLE BORDER=1 CELLSPACING=0>
<TR><TH>event</TH><TH>date</TH><TH>place</TH><TH>notes</TH><TH width=20%>source</TH></TR>
%event%
<TR>
<TD><B>%type%</B></TD>
<TD NOWRAP>%date%</TD>
<TD>%place%</TD>
<TD>%note%</TD>
<TD><FONT STYLE="{font-size:80%;}"><CITE>%citation%</CITE></FONT></TD>
</TR>
%/event%
</TABLE>
</TD></TR></TABLE>
%/events%
%children%
<P><B>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;children:</B>
<TABLE><TR><TD style="{background-color: #FFFFCC;}">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</TD><TD>
<TABLE BORDER=1 CELLSPACING=0>
%child%
<TR><TD>%name%</TD></TR>
%/child%
</TABLE>
</TD></TR></TABLE>
%/children%
%/partnership%
%/partnerships%

</BODY>
</HTML>
