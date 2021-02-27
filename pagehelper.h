#define HTMLPAGE const char HtmlPage[] PROGMEM
#define IMAGE const char Image[] PROGMEM
#define CreatePageName String PageName() { return GetPageName(__PRETTY_FUNCTION__); }

String GetPageName(String Pretty)
{
   String PageName;
   int StartIndex = Pretty.indexOf(' ');
   int EndIndex = Pretty.indexOf(':');
   PageName = (Pretty.substring(StartIndex+1, EndIndex));
   PageName.replace("$", "/");
   PageName.replace("_", ".");
   PageName = "/" + PageName;
   return PageName;
}
