# 1.5.1 - 11. February 2010

* implemented status bar icons
* implemented two stage regex
* replacing many more character entity references. now supports all the French characters as well.
* New / updated searches:
** amazon and amazonde (much improved!)
** imdb (improved)

# 1.5.0 - 5. February 2010

* implemented sortOrder parameter
* implemented isHidden parameter
* implemented context submenus
* now supports more than 9 context menu items
* now clears context menus for subsearches
* subsearches / abbreviations are now case-insensitive
* now prints the number of listed searches in the status bar
* updated help file
* New / updated searches:
** onelook (by Perry Mowbray)
** timeanddate (by Perry Mowbray)
** autohotkey (ewemoa)
** cheatsheet
** amazon (fixed typo)

# 1.4.2 - 29. January 2010

* Now filters for subsearch abbreviations too
* Added some logging capability. Type 'wms !' to enable / disable logging.
* Added {{{%PLUGINALIAS%}}} variable
* Updated help file
* New / updated searches:
** onelook (by Perry Mowbray) - with more subsearches and help file!

# 1.4.1 - 26. January 2010

* subsearch filtering now matches at the start (no longer anywhere in the string)
* show abbreviation of subsearches (if they have one). e.g. wms amazon +books (b)
* Shift + Enter launches search result in FARR's internal HTML view
* replace more special characters in FARR output (&nbsp; and &deg;)
* fixed problem with subsearches having isFeed=true

# 1.4.0 - 24. January 2010

* Shift + Enter shows info about search (using FARR's internal HTML view)
* Subsearches can now have abbreviations ( {{{[Subsearch|Abb]}}} )
* Searches can now define context menu items (see play search for an example)
* Fixed 'feeds not showing subsearches on entering +'
* new / updated searches:
** onelook (by Perry Mowbray) - added four new subsearches
** jamendo (by Perry Mowbray)
** cppreference - replaced with a different one providing better information
** play - added 'buy item' and 'show basket' context menu items 

# 1.3.0 - 15. January 2010

* search term ({{{%SEARCHTERM%}}}) and search URL ({{{%SEARCHURL%}}}) are now available as variables in the resultPattern, farrPath, farrCaption and farrGroup parameters
* search term ({{{%SEARCHTERM%}}}) is now available as a variable in the searchUrl parameter. If searchUrl does not contain the {{{%SEARCHTERM%}}} variable, the search term is appended at the end (as it was until now)
* properly handle and report server errors
* show down-arrow icon on status bar to switch to FARR's extended view (which doesn't handle html tags properly though)
* new / updated searches:
** onelook (by Perry Mowbray) - added name subsearch

# 1.2.0 - 14. January 2010

* Fixed bug with search   defined as empty string
* Updated help file [[Search config files]]
* new / updated searches:
** onelook (by Perry Mowbray)
** kindleforum (feed)
** play.com

# 1.1.0 - 9. January 2010

* Type wms ? + Enter to show help file
* Added about list items
* Updated help file (still needs better treatment of .config files)

# 1.0.0 - 8. January 2010

* Changed search behaviour for news feeds
* Improved amazon.com search. Now gives more relevant results when searching for 'neural control' (and other titles with more than one author ;)
* replace more special characters in FARR output, e.g. © and ®
* resultPattern can now contain the %SEARCHTERM% variable, which will be replaced by the entered search terms
* added new searches:
** onelook (thanks Perry Mowbray!)
** engadget (feed)
** twitter
* changed to [[TiddlyWiki|http://www.tiddlywiki.com/]] help file format (although it's far from complete)

# 0.9.1 - 31. December 2009

* use FARR's launching functionality to open URLs

# 0.9.0 - 29. December 2009

* First release
