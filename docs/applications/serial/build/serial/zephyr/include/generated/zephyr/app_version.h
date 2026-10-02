#ifndef _APP_VERSION_H_
#define _APP_VERSION_H_

/* The template values come from cmake/modules/version.cmake
 * BUILD_VERSION related template values will be 'git describe',
 * alternatively user defined BUILD_VERSION.
 */

/* #undef ZEPHYR_VERSION_CODE */
/* #undef ZEPHYR_VERSION */

#define APPVERSION                   0x3050500
#define APP_VERSION_NUMBER           0x30505
#define APP_VERSION_MAJOR            3
#define APP_VERSION_MINOR            5
#define APP_PATCHLEVEL               5
#define APP_TWEAK                    0
#define APP_EXTRAVERSION             1e92bdaf
#define APP_VERSION_STRING           "3.5.5-1e92bdaf"
#define APP_VERSION_EXTENDED_STRING  "3.5.5-1e92bdaf+0"
#define APP_VERSION_TWEAK_STRING     "3.5.5+0"

#define APP_BUILD_VERSION v3.5.5-41-g1e92bdaf5cf8


#endif /* _APP_VERSION_H_ */
