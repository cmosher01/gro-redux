---
layout: default
---

# v1.5.0

New build system. Few bug fixes. New web site (completely hosted on github).

# V1.4

New Features (see help for explanations)
Greatly improved automatic layout of entire chart (clean)
Copy and Paste Individuals
Drop-Line Chart rescales with font changes
Web Server
Temporarily dim some individuals
Chart traversal keyboard shortcuts
Use HTML in source transcriptions
Handles complete ANSEL character set
Build your own family tree CD-ROM
Improved select relatives command
Normal 1:1 Scale command
Bug Fixes
Children sorted--All lists of children are shown in sorted order, by birth date (if known).
Spouses sorted--For people with multiple spouses, those spouses are shown in sorted order, by marriage date (if known).
Standards-compliant web pages--The web server will serve web pages that adhere to W3C standards (as much as is possible while still being compatible with popular web browsers). The pages strive to stay within the XHTML 1.1 standard, and CSS standard.
Better defaults when connecting individuals--When connecting two individuals, the defaults have changed for V1.4. The order of the two people has switched; the new order seems to be more intuitive. Also, when connecting as a spouse to a person that already has a family, the default is to add to the existing family, rather than to create a new family.
Family Tree Maker enhancements--Some versions of Family Tree Maker export some non-standard GEDCOM. GRO will recognize (at least some) of these anomalies and import them correctly. Also, FTM uses the _SCHEMA tag, which was proposed for the GEDCOM standard some time ago. As of V1.4, GRO will recognize and correctly import any _SCHEMA data that exists in the GEDCOM file.
NOTE record bug--Some NOTE records (ones that were just links) were not getting imported correctly prior to V1.4. They would be imported as, for example, "@N23@". This has been fixed so that the full text of the note is now imported correctly.
Sorting order fixed--Lists of names and places are sorted correctly, even if those names have characters with diacritical marks, such as umlauts and accents. Previous versions (before V1.4) of GRO didn't sort such names correctly.
Places dialog on top--Bringing up the Places dialog would sometime cause another window to be brought to the front. This is fixed in V1.4.
Display names better--Some imported GEDCOM files omit spaces between the surname and the given name. This condition is checked for, and the names will display with the correct space between them on the chart and on the individual's dialog box. Note that the name is not automatically changed in the file, it is simply displayed correctly on the screen. Also, names that contained an ampersand character (&) didn't display correctly in all cases; this is fixed in V1.4.
F1 button--Pressing the F1 key brings the user to the GRO web site's home page. Previous versions of GRO would display an error message when the user pressed the F1 key.
Supports most versions of Microsoft Windows--GRO prior to V1.4 was only written to support Windows 98. GRO V1.4 (and later) supports Windows 95, 98, ME, NT, 2000, and XP. Note that 95/98/ME is a different download than NT/2000/XP. The 95/98/ME version is much larger, because it needs to include Microsoft's Unicode library code (known as MSLU, or Unicows), along with recompiled MFC and CRTL libraries. Windows NT/2000/XP has Unicode built-in, so GRO doesn't need to include any extra software when downloading for these Windows versions.
Accelerator keys--Some of the accelerator keys (the Alt-key commands, which are shown with an underlined letter on the menu) were not assigned correctly. This is fixed in V1.4.
Auxilliary files not needed--There were some auxilliary files (.RTF and .TPL files) that were included in the download and existed in the same directory as GRO.EXE. These files allowed the user to change the format of the web pages and RTF files generated by GRO. These files are no longer needed (but the user can still create his or her own files to override the default settings).
File reading improved--Reading in GEDCOM files has had some speed improvements made to it.
Notes not displayed when open--If a GEDCOM file had document notes in it, then those notes would be displayed automatically when the file was opened. This proved somewhat annoying, so as of V1.4 this feature has been removed.
Start Menu command fixed--Adding the "Genealogy Research Organizer" command to the Windows Start Menu didn't work with the newer versions of Windows. This has been fixed in GRO V1.4.
Progress bar improvements--The progress bar dialog is now wider, and the bar itself is "smooth" (i.e., it doesn't have breaks in it anymore).

# V1.3

Check for Anomalies
This new feature allows you to check for anomalies in your genealogical data. When you pick View | Anomalies from the menu, GRO will go through every currently selected person and look for potentially anomalous data. This can help you track down many typographic errors or other obvious errors in your genealogical data. Common typographic errors found can include incorrect birth months (e.g., Jan instead of Jun), wrong century (e.g., 1974 instead of 1874), or other common mistakes. Examples of anomalies it finds include children born when parents are too young, children from the same mother born less than nine months apart, and people who are over 100 years old. Obviously, the anomalies it finds are not necessarily incorrect data, but they can help focus your attention to uncommon occurrences. It can help you pinpoint areas that may require further research in order to verify.
After choosing View | Anomalies from the menu, it will construct the list and display it in your internet browser.
Place hierarchy now includes family events
The Places Hierarchy dialog in previous versions of GRO did not include events from families (i.e., partnership events). Now it includes partnership events as well as individuals' events.
Fix crashes when no printer installed
The program would crash immediately if there was no printer installed on the computer. This is fixed.
Fix ATL.DLL missing error
Previous version of GRO depended on ATL.DLL being on the user's computer. This has been changed so that there is no longer any need for ATL.DLL.

# V1.2

US Census locations of individuals
This feature allows you to see where individuals were located during US Censuses. To use it, first select the individuals you're interested in (Ctrl-click to select more than one individual). Then choose from the menu: View | Census. The individuals' events will be examined and the most likely location will be determined, for each census year. The results will be displayed in your browser in table form. Each row is one person; each column in one particular US Census. The place will be shown for each person and each census. An asterisk (*) is displayed when the census event in question is actually in the Gedcom document.
Place hierarchy
This feature allows you to see all events that occurred at a particular place. Select the individuals you are interested in, then choose from the menu: View | Place Hierarchy. The individuals' events and attributes will be examined, and organized by place into a hierarchy. The left side of the place dialog shows a tree-style control showing the places; click on the plus sign to expand a place. When you select a place, the right side of the dialog will show all the events (and attributes) that happened within that place. You can sort the events by clicking on the column titles.
Printing the drop-line chart
You can print the drop-line chart. When viewing the chart on the screen, you can see the page numbers and page breaks. (You can toggle this display on and off by choosing File | Show Page Breaks.) Also note that you can choose View | Scale to "zoom out" from the drop-line chart display. Choosing File | Page Setup... will allow you to change the printer settings (e.g., landscape mode), and the drop-line chart will reflect the changes you make. When you choose File | Print, you can choose to print all pages, or you can enter a range of pages to print.
RTF export of individuals and sources
Although GRO doesn't print individuals or sources, you can export this information to your word processor. Choose File | Export tree as RTF..., and GRO will generate an RTF file containing all individuals and sources from the document. It will then open the RTF file using your word processor, where you can print any pages you want.
Large drop-line charts are supported
Previous versions of GRO had severe limitations on the size of the drop-line chart. These were due to Windows 98's 16-bit coordinate system. GRO V1.2 uses its own (32-bit) coordinate system, allowing an almost unlimited size for the drop-line chart (thousands of printed pages wide and tall).
Integration with Windows 98 Personal Web Server
If you are running Windows 98 Personal Web Server, you can have it retrieve data directly from your GRO program running on the same machine. If you use this method, you don't need to export HTML files for web clients to be able to browse your Gedcom documents. Note that this method requires you to have Windows 98 Personal Web Server running on the same machine as GRO. To set this up, first run GRO on your web server machine, and open the Gedcom document(s) you want to make available to your web clients. Then, in the Personal Web Server's Manager program, choose Advanced options, and click Add... to add a new virtual directory. In the Add Directory dialog, click Browse... and locate the directory that GRO.EXE is installed in and click OK. Type in the alias you want (e.g., GRO) and click OK. Then web users can type your web address followed by the alias (e.g., GRO), and they will be able to view all the Gedcom document(s) you have open in GRO. Individuals that were born less than 72 years ago are considered "private," and will require web users to enter a password in order to view their details. You can set this password by choosing Options | Password.
Automatically re-open Gedcom documents
If you exit GRO with open Gedcom documents, GRO will automatically re-open those same documents the next time you run it.
HTML template files for export
If you use File | Export tree as HTML... to export your Gedcom document data to HTML pages, GRO uses template files to determine how it should build the HTML files. These two files are indi.tpl and sour.tpl, and are found the the same folder as GRO.EXE. If you want to change the appearance of the HTML files GRO generates, you can provide your own versions of these template files. The best way to do this is to keep the original indi.tpl and sour.tpl files in the GRO folder, and copy them to the folder that contains your Gedcom document, then edit these copies. GRO will use your modified templates to produce the HTML files it exports. The template files contain regular HTML code, with additional "tags" recognized only by GRO. These tags are in the format: %tag%, for example %event% or %name%.
Exported HTML files are in their own folder
When you use File | Export tree as HTML... to export your Gedcom document data to HTML files, those files will be placed into their own folder. The folder will have the same name as the Gedcom document, and will be in the same folder as the Gedcom document. For example, if you have open the Gedcom document SMITH.GED in the folder "My Family Tree", then the HTML files will be created in a folder called "SMITH", which will be located within the "My Family Tree" folder.
Birth date calculator
This feature will help you solve that frequently encountered genealogical task of computing someone's birth date, when you are given their age on a certain date. This problem can typically arise when searching census records, military records, or cemetery records, to name a few. To use this feature, choose from the menu Tools | Birth Calculator... to bring up the Birth Calculator dialog box. In the first section, enter the person's age. This can be entered in either of three formats: years, years-months-days, or "in the nth year of his/her age." Choose the appropriate "radio button" and enter the person's age. Next, click on the Date button and enter the date in question (this must be an exact date). The birth date (or possible range of dates) will be calculated and displayed at the bottom. An additional piece of information is also provided, which may be of use when searching census recors: another birth date (or range) is calculated; this date represents the birth day if the age was on the "census day" of the given year, rather than on the actual date you entered. This can be used to help you judge whether the person's age listed in the census record is based on the "census day" or on the actual day the census was taken.
The background color of the pedigree chart wasn't always correct
Now the pedigree chart will be displayed correctly, no matter what Windows 98 Display Properties are set to.
Arrow keys on drop-line chart
When viewing a drop-line chart, you can use the arrow keys to scroll the chart. You can also use the Enter key to open a currently selected individual. You can hold down the Ctrl key and use the arrow keys to move currently selected individuals on the chart; this allows for accurate positioning of individuals.
Installation and uninstallation process improvements
The installation process is easier. The uninstallation process is now cleaner (although it does require you to restart your computer).
Doesn't add "Old" to File menu
A file named Oldxxxxxx was being added to the File menu whenever a document was saved. This is fixed in this version.

# V1.1

Export HTML now creates the HTML directory if necessary.
Improved formatting of short dates.
Slashed years
GRO now shows slashed year format dates. Dates between 1 Jan and 24 Mar for years between 1583 and 1752 will be shown with a slashed year, for example, 16 Jan 1683/4.
Handle INDI.NOTE tags.
Export HTML now shows a progress bar while exporting.
On an individual's screen, events and attributes are displayed in chronological order.
Export HTML now puts events and attributes in chronological order.
Export HTML now exports partnership events.
Fixed bug where user couldn't choose a source if there was only one source in the list.
Fixed various other bugs.
