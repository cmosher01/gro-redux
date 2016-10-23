#define menuFile \
POPUP "&File"\
BEGIN\
    MENUITEM "&New\tCtrl+N",                ID_FILE_NEW\
    MENUITEM "&Open...\tCtrl+O",            ID_FILE_OPEN\
    MENUITEM "&Close",                      ID_FILE_CLOSE\
    MENUITEM "&Save\tCtrl+S",               ID_FILE_SAVE\
    MENUITEM "Save &As...",                 ID_FILE_SAVE_AS\
    MENUITEM SEPARATOR\
    MENUITEM "&Print...",                   ID_FILE_PRINT\
	MENUITEM "Page Set&up...",              ID_FILE_PAGE_SETUP\
    MENUITEM "Show Page Brea&ks",           ID_OPTIONS_PAGEBREAKS\
    MENUITEM SEPARATOR\
    MENUITEM "Export tree as &RTF...",      ID_FILE_RTF\
    MENUITEM "Export tree as &HTML...",     ID_FILE_HTML\
	MENUITEM "&Web server..."               ID_FILE_SERVE\
    MENUITEM SEPARATOR\
    MENUITEM "Recent File",                 ID_FILE_MRU_FILE1, GRAYED\
    MENUITEM SEPARATOR\
    MENUITEM "Se&tup...",                   ID_FILE_SETUP\
    MENUITEM "Register...",                 ID_FILE_REGISTER\
    MENUITEM SEPARATOR\
    MENUITEM "E&xit",                       ID_APP_EXIT

/*	MENUITEM "Print Pe&view...", ID_FILE_PRINT_PREVIEW\ */

#define menuEdit \
POPUP "&Edit"\
BEGIN\
    MENUITEM "&Undo\tCtrl+Z",               ID_EDIT_UNDO\
    MENUITEM SEPARATOR\
    MENUITEM "Cu&t\tCtrl+X",                ID_EDIT_CUT\
    MENUITEM "&Copy\tCtrl+C",               ID_EDIT_COPY\
    MENUITEM "&Paste\tCtrl+V",              ID_EDIT_PASTE

#define menuView \
POPUP "&View"\
BEGIN\
    MENUITEM "&Toolbar",                    ID_VIEW_TOOLBAR\
    MENUITEM "&Status Bar",                 ID_VIEW_STATUS_BAR\
    MENUITEM SEPARATOR\
    MENUITEM "&Drop-Line Chart",            ID_VIEW_DROPLINECHART\
    MENUITEM "&Individual",                 ID_VIEW_INDI\
    MENUITEM "&Pedigree",                   ID_VIEW_OPENPEDIGREE\
    MENUITEM "Inde&x of Individuals",       ID_VIEW_INDEXOFINDIVIDUALS\
    MENUITEM "S&ources",                    ID_VIEW_SOURCES\
    MENUITEM "&Repositories",               ID_VIEW_REPOSITORIES\
    MENUITEM "Document &Notes",             ID_VIEW_NOTES

#define menuWindow \
POPUP "&Window"\
BEGIN\
    MENUITEM "&Cascade",                    ID_WINDOW_CASCADE\
    MENUITEM "&Tile",                       ID_WINDOW_TILE_HORZ\
    MENUITEM "&Arrange Icons",              ID_WINDOW_ARRANGE

#define menuTools \
POPUP "&Tools"\
BEGIN\
    MENUITEM "&Holiday",                    ID_TOOLS_HOLIDAY\
    MENUITEM "&Birth Calculator",           ID_BIRTH_CALC

#define menuOptions \
POPUP "&Options"\
BEGIN\
    MENUITEM "&Font",                       ID_OPTIONS_FONT\
    MENUITEM "&Submitter",                  ID_OPTIONS_SUBMITTER\
    MENUITEM "&Unicode",                    ID_OPTIONS_UNICODE

#define menuHelp \
POPUP "&Help"\
BEGIN\
    MENUITEM "http://go.to/gro",            ID_WEB_SITE\
    MENUITEM SEPARATOR\
    MENUITEM "&About GRO...",               ID_APP_ABOUT
