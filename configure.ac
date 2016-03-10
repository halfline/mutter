AC_PREREQ(2.62)

m4_define([mutter_major_version], [3])
m4_define([mutter_minor_version], [19])
m4_define([mutter_micro_version], [4])

m4_define([mutter_version],
          [mutter_major_version.mutter_minor_version.mutter_micro_version])

m4_define([mutter_plugin_api_version], [3])

AC_INIT([mutter], [mutter_version],
        [http://bugzilla.gnome.org/enter_bug.cgi?product=mutter])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR(src/core/display.c)
AC_CONFIG_HEADERS(config.h)

AM_INIT_AUTOMAKE([1.11 foreign no-dist-gzip dist-xz tar-ustar subdir-objects])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])],)
AM_MAINTAINER_MODE([enable])

MUTTER_MAJOR_VERSION=mutter_major_version
MUTTER_MINOR_VERSION=mutter_minor_version
MUTTER_MICRO_VERSION=mutter_micro_version
MUTTER_PLUGIN_API_VERSION=mutter_plugin_api_version
AC_SUBST(MUTTER_MAJOR_VERSION)
AC_SUBST(MUTTER_MINOR_VERSION)
AC_SUBST(MUTTER_MICRO_VERSION)
AC_SUBST(MUTTER_PLUGIN_API_VERSION)

MUTTER_PLUGIN_DIR="$libdir/$PACKAGE/plugins"
AC_SUBST(MUTTER_PLUGIN_DIR)

# Honor aclocal flags
AC_SUBST(ACLOCAL_AMFLAGS, "\${ACLOCAL_FLAGS}")

GETTEXT_PACKAGE=mutter
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE",[Name of default gettext domain])

LT_PREREQ([2.2.6])
LT_INIT([disable-static])
IT_PROG_INTLTOOL([0.41])
AC_PROG_CC
AC_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_SED
AC_HEADER_STDC
PKG_PROG_PKG_CONFIG([0.21])

# Sets GLIB_GENMARSHAL and GLIB_MKENUMS
AM_PATH_GLIB_2_0()

CANBERRA_GTK=libcanberra-gtk3
CANBERRA_GTK_VERSION=0.26

CLUTTER_PACKAGE=clutter-1.0

MUTTER_PC_MODULES="
   gtk+-3.0 >= 3.19.8
   gio-unix-2.0 >= 2.35.1
   pango >= 1.2.0
   cairo >= 1.10.0
   gsettings-desktop-schemas >= 3.19.3
   $CLUTTER_PACKAGE >= 1.25.1
   cogl-1.0 >= 1.17.1
   upower-glib >= 0.99.0
   gnome-desktop-3.0
   xcomposite >= 0.2
   xcursor
   xdamage
   xext
   xfixes
   xi >= 1.6.0
   xkbfile
   xkeyboard-config
   xkbcommon >= 0.4.3
   xkbcommon-x11
   xrender
   x11-xcb
   xcb-randr
"

GLIB_GSETTINGS

AC_ARG_ENABLE(verbose-mode,
  AC_HELP_STRING([--disable-verbose-mode],
                 [disable mutter's ability to do verbose logging, for embedded/size-sensitive custom builds]),,
  enable_verbose_mode=yes)

if test x$enable_verbose_mode = xyes; then
    AC_DEFINE(WITH_VERBOSE_MODE,1,[Build with verbose mode support])
fi

AC_ARG_ENABLE(sm,
  AC_HELP_STRING([--disable-sm],
                 [disable mutter's session management support, for embedded/size-sensitive custom non-GNOME builds]),,
  enable_sm=auto)

AC_ARG_ENABLE(startup-notification,
  AC_HELP_STRING([--disable-startup-notification],
                 [disable mutter's startup notification support, for embedded/size-sensitive custom non-GNOME builds]),,
  enable_startup_notification=auto)

AC_ARG_WITH(libcanberra,
  AC_HELP_STRING([--without-libcanberra],
                 [disable the use of libcanberra for playing sounds]),,
  with_libcanberra=auto)

AC_ARG_WITH([xwayland-path],
            [AS_HELP_STRING([--with-xwayland-path], [Absolute path for an X Wayland server])],
            [XWAYLAND_PATH="$withval"],
            [XWAYLAND_PATH="$bindir/Xwayland"])

AC_ARG_ENABLE(installed_tests,
              AS_HELP_STRING([--enable-installed-tests],
                             [Install test programs (default: no)]),,
              [enable_installed_tests=no])
AM_CONDITIONAL(BUILDOPT_INSTALL_TESTS, test x$enable_installed_tests = xyes)

## here we get the flags we'll actually use

# Unconditionally use this dir to avoid a circular dep with gnomecc
GNOME_KEYBINDINGS_KEYSDIR="${datadir}/gnome-control-center/keybindings"
AC_SUBST(GNOME_KEYBINDINGS_KEYSDIR)

STARTUP_NOTIFICATION_VERSION=0.7
AC_MSG_CHECKING([Startup notification library >= $STARTUP_NOTIFICATION_VERSION])
if $PKG_CONFIG --atleast-version $STARTUP_NOTIFICATION_VERSION libstartup-notification-1.0; then
   have_startup_notification=yes
else
   have_startup_notification=no
fi
AC_MSG_RESULT($have_startup_notification)

if test x$enable_startup_notification = xyes; then
   have_startup_notification=yes
   echo "startup-notification support forced on"
elif test x$enable_startup_notification = xauto; then
   true
else
   have_startup_notification=no
fi

if test x$have_startup_notification = xyes; then
  echo "Building with libstartup-notification"
  MUTTER_PC_MODULES="$MUTTER_PC_MODULES libstartup-notification-1.0 >= $STARTUP_NOTIFICATION_VERSION"
  AC_DEFINE(HAVE_STARTUP_NOTIFICATION, , [Building with startup notification support])
else
  echo "Building without libstartup-notification"
fi

have_libcanberra=no
AC_MSG_CHECKING([libcanberra-gtk])
if test x$with_libcanberra = xno ; then
  AC_MSG_RESULT([disabled])
else
  if $PKG_CONFIG --exists $CANBERRA_GTK '>=' $CANBERRA_GTK_VERSION; then
    have_libcanberra=yes
    AC_MSG_RESULT(yes)
    MUTTER_PC_MODULES="$MUTTER_PC_MODULES $CANBERRA_GTK"
    AC_DEFINE([HAVE_LIBCANBERRA], 1, [Building with libcanberra for playing sounds])
  else
    AC_MSG_RESULT(no)
    if test x$with_libcanberra = xyes ; then
      AC_MSG_ERROR([libcanberra forced and libcanberra-gtk was not found])
    fi
  fi
fi

INTROSPECTION_VERSION=0.9.5
GOBJECT_INTROSPECTION_CHECK([$INTROSPECTION_VERSION])

if test x$found_introspection != xno; then
  AC_DEFINE(HAVE_INTROSPECTION, 1, [Define if GObject introspection is available])
  MUTTER_PC_MODULES="$MUTTER_PC_MODULES gobject-introspection-1.0"
  # Since we don't make any guarantees about stability and we don't support
  # parallel install, there's no real reason to change directories, filenames,
  # etc. as we change the Mutter tarball version. Note that this must match
  # api_version in src/Makefile.am
  META_GIR=Meta_3_0_gir
  # META_GIR=[Meta_]mutter_major_version[_]mutter_minor_version[_gir]
  AC_SUBST(META_GIR)
fi

AC_SUBST(XWAYLAND_PATH)

PKG_CHECK_MODULES(MUTTER, $MUTTER_PC_MODULES)

MUTTER_NATIVE_BACKEND_MODULES="clutter-egl-1.0 libdrm libsystemd libinput gudev-1.0 gbm >= 10.3"

AC_ARG_ENABLE(native-backend,
  AS_HELP_STRING([--disable-native-backend], [disable mutter native (KMS) backend]),,
  enable_native_backend=auto
)
AS_IF([test "$enable_native_backend" = "yes"], [have_native_backend=yes],
      [test "$enable_native_backend" = "auto"], PKG_CHECK_EXISTS([$MUTTER_NATIVE_BACKEND_MODULES], [have_native_backend=yes]))

AS_IF([test "$have_native_backend" = "yes"], [
  PKG_CHECK_MODULES([MUTTER_NATIVE_BACKEND], [$MUTTER_NATIVE_BACKEND_MODULES])
  AC_DEFINE([HAVE_NATIVE_BACKEND],[1], [Define if you want to enable the native (KMS) backend based on systemd])
])
AM_CONDITIONAL([HAVE_NATIVE_BACKEND],[test "$have_native_backend" = "yes"])

MUTTER_WAYLAND_MODULES="clutter-wayland-1.0 clutter-wayland-compositor-1.0 wayland-server >= 1.6.90"

AC_ARG_ENABLE(wayland,
  AS_HELP_STRING([--disable-wayland], [disable mutter on wayland support]),,
  enable_wayland=auto
)
AS_IF([test "$enable_wayland" = "yes"], [have_wayland=yes],
      [test "$enable_wayland" = "auto"], PKG_CHECK_EXISTS([$MUTTER_WAYLAND_MODULES], [have_wayland=yes]))

AS_IF([test "$have_wayland" = "yes"], [
  PKG_CHECK_MODULES([MUTTER_WAYLAND], [$MUTTER_WAYLAND_MODULES])
  AC_PATH_PROG([WAYLAND_SCANNER],[wayland-scanner],[no])
  AS_IF([test $WAYLAND_SCANNER = "no"],
    [AC_MSG_ERROR([Could not find wayland-scanner in your PATH, required for parsing wayland extension protocols])])
  AC_SUBST([WAYLAND_SCANNER])
  AC_DEFINE([HAVE_WAYLAND],[1],[Define if you want to enable Wayland support])

  PKG_CHECK_MODULES(WAYLAND_PROTOCOLS, [wayland-protocols >= 1.1],
		    [ac_wayland_protocols_pkgdatadir=`$PKG_CONFIG --variable=pkgdatadir wayland-protocols`])
  AC_SUBST(WAYLAND_PROTOCOLS_DATADIR, $ac_wayland_protocols_pkgdatadir)
])
AM_CONDITIONAL([HAVE_WAYLAND],[test "$have_wayland" = "yes"])

PKG_CHECK_EXISTS([xi >= 1.6.99.1],
                 AC_DEFINE([HAVE_XI23],[1],[Define if you have support for XInput 2.3 or greater]))

# This is used for plugins
AC_SUBST(CLUTTER_PACKAGE)
PKG_CHECK_MODULES(CLUTTER, $CLUTTER_PACKAGE)

AC_PATH_XTRA

ALL_X_LIBS="$X_LIBS $X_PRE_LIBS -lX11 $X_EXTRA_LIBS"

# Check for Xinerama extension - we only support the "XFree86" style,
# and not the older Solaris-only version; recent Solaris supports the
# XFree86 style.
mutter_save_cppflags="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $X_CFLAGS"

have_xinerama=yes
AC_CHECK_LIB(Xinerama, XineramaQueryExtension,
    [AC_CHECK_HEADER(X11/extensions/Xinerama.h,
		     [X_EXTRA_LIBS="-lXinerama $X_EXTRA_LIBS"
		     if test -z "`echo $ALL_X_LIBS | grep "\-lXext" 2> /dev/null`"; then
			 X_EXTRA_LIBS="-lXext $X_EXTRA_LIBS"
		     fi],
		     have_xinerama=no,
		     [#include <X11/Xlib.h>])],
    have_xinerama=no, -lXext $ALL_X_LIBS)
AC_MSG_CHECKING(for Xinerama support)
AC_MSG_RESULT($have_xinerama)

CPPFLAGS="$mutter_save_cppflags"

if test x$have_xinerama = xno; then
   AC_MSG_ERROR([Xinerama extension was not found])
fi

AC_DEFINE_UNQUOTED([XKB_BASE], ["`$PKG_CONFIG --variable xkb_base xkeyboard-config`"],
                               [XKB base dir])

RANDR_LIBS=
found_randr=no
AC_CHECK_LIB(Xrandr, XRRUpdateConfiguration,
               [AC_CHECK_HEADER(X11/extensions/Xrandr.h,
                                RANDR_LIBS=-lXrandr found_randr=yes,,
				[#include <X11/Xlib.h>])],
               , -lXext $ALL_X_LIBS)

if test "x$found_randr" = "xyes"; then
   AC_DEFINE(HAVE_RANDR, , [Have the Xrandr extension library])
   PKG_CHECK_EXISTS([xrandr >= 1.5.0],
                 AC_DEFINE([HAVE_XRANDR15],[1],[Define if you have support for XRandR 1.5 or greater]))
fi

MUTTER_LIBS="$MUTTER_LIBS $RANDR_LIBS $X_LIBS $X_PRE_LIBS -lX11 $X_EXTRA_LIBS -lm"

found_sm=no
case "$MUTTER_LIBS" in
 *-lSM*)
    found_sm=yes
    ;;
 *)
  AC_CHECK_LIB(SM, SmcSaveYourselfDone,
               [AC_CHECK_HEADERS(X11/SM/SMlib.h,
                                MUTTER_LIBS="-lSM -lICE $MUTTER_LIBS" found_sm=yes)],
               , $MUTTER_LIBS)
    ;;
esac

if test x$enable_sm = xno; then
   found_sm=no
fi

if test x$enable_sm = xyes; then
   if test "$found_sm" = "no"; then
      AC_MSG_ERROR([--enable-sm forced and -lSM not found])
      exit 1
   fi
fi

if test "$found_sm" = "yes"; then
   AC_DEFINE(HAVE_SM, , [Building with SM support])
fi

AM_CONDITIONAL(HAVE_SM, test "$found_sm" = "yes")

AC_PATH_PROG(ZENITY, zenity, no)
if test x"$ZENITY" = xno; then
  AC_MSG_ERROR([zenity not found in your path - needed for dialogs])
fi

AC_ARG_ENABLE(debug,
	[  --enable-debug		enable debugging],,
	enable_debug=no)
if test "x$enable_debug" = "xyes"; then
	CFLAGS="$CFLAGS -g -O"
fi

AC_CHECK_DECL([GL_EXT_x11_sync_object],
              [],
              [AC_MSG_ERROR([GL_EXT_x11_sync_object definition not found, please update your GL headers])],
              [#include <GL/glx.h>])

#### Warnings (last since -Werror can disturb other tests)

# Stay command-line compatible with the gnome-common configure option. Here
# minimum/yes/maximum are the same, however.
AC_ARG_ENABLE(compile_warnings,
  AS_HELP_STRING([--enable-compile-warnings=@<:@no/minimum/yes/maximum/error@:>@],[Turn on compiler warnings]),,
  enable_compile_warnings=error)

changequote(,)dnl
if test "$enable_compile_warnings" != no ; then
  if test "x$GCC" = "xyes"; then
    case " $CFLAGS " in
    *[\ \	]-Wall[\ \	]*) ;;
    *) CFLAGS="$CFLAGS -Wall" ;;
    esac

#   case " $CFLAGS " in
#   *[\ \	]-Wshadow[\ \	]*) ;;
#   *) CFLAGS="$CFLAGS -Wshadow" ;;
#   esac

    case " $CFLAGS " in
    *[\ \	]-Wchar-subscripts[\ \	]*) ;;
    *) CFLAGS="$CFLAGS -Wchar-subscripts" ;;
    esac

    case " $CFLAGS " in
    *[\ \	]-Wmissing-declarations[\ \	]*) ;;
    *) CFLAGS="$CFLAGS -Wmissing-declarations" ;;
    esac

    case " $CFLAGS " in
    *[\ \	]-Wmissing-prototypes[\ \	]*) ;;
    *) CFLAGS="$CFLAGS -Wmissing-prototypes" ;;
    esac

    case " $CFLAGS " in
    *[\ \	]-Wnested-externs[\ \	]*) ;;
    *) CFLAGS="$CFLAGS -Wnested-externs" ;;
    esac

    case " $CFLAGS " in
    *[\ \	]-Wpointer-arith[\ \	]*) ;;
    *) CFLAGS="$CFLAGS -Wpointer-arith" ;;
    esac

    case " $CFLAGS " in
    *[\ \	]-Wcast-align[\ \	]*) ;;
    *) CFLAGS="$CFLAGS -Wcast-align" ;;
    esac

    case " $CFLAGS " in
    *[\ \	]-Wsign-compare[\ \	]*) ;;
    *) CFLAGS="$CFLAGS -Wsign-compare" ;;
    esac

    if test "$enable_compile_warnings" = error; then
      case " $CFLAGS " in
      *[\ \	]-Werror[\ \	]*) ;;
      *) CFLAGS="$CFLAGS -Werror -Wno-error=deprecated-declarations" ;;
      esac
    fi
  fi
fi
changequote([,])dnl

if test x$enable_verbose_mode = xno; then
        echo "*** WARNING WARNING WARNING WARNING WARNING"
        echo "*** Building without verbose mode"
        echo "*** This means there's no way to debug mutter problems."
        echo "*** Please build normal desktop versions of mutter"
        echo "*** with verbose mode enabled so users can use it when they report bugs."
fi

dnl ==========================================================================
echo "
mutter-$VERSION

	prefix:                   ${prefix}
	source code location:	  ${srcdir}
	compiler:		  ${CC}

	Startup notification:     ${have_startup_notification}
	libcanberra:              ${have_libcanberra}
	Introspection:            ${found_introspection}
	Session management:       ${found_sm}
	Wayland:                  ${have_wayland}
	Native (KMS) backend:     ${have_native_backend}
"


MUTTER_MINOR_VERSION=mutter_minor_version
if expr $MUTTER_MINOR_VERSION % 2 > /dev/null ; then
   stable_version=`expr $MUTTER_MINOR_VERSION - 1`
   echo "This is the UNSTABLE branch of mutter"
   echo -n "Use 3.$stable_version.x for stable "
   echo    "(gnome-3-$stable_version branch in git)"
else
   echo "This is the stable branch of mutter"
fi

m4_define([clutter_major_version], [1])
m4_define([clutter_minor_version], [25])
m4_define([clutter_micro_version], [7])
m4_define([clutter_interface_age], [0])
m4_define([clutter_binary_age], [m4_eval(100 * clutter_minor_version + clutter_micro_version)])
m4_define([clutter_release_status],
          [m4_if(m4_eval(clutter_micro_version % 2), [1], [git],
                 [m4_if(m4_eval(clutter_minor_version % 2), [1], [snapshot],
                                                                 [release])])])

m4_define([clutter_version], [clutter_major_version.clutter_minor_version.clutter_micro_version])

m4_define([clutter_api_version], [1.0])

AC_SUBST([CLUTTER_MAJOR_VERSION],  [clutter_major_version])
AC_SUBST([CLUTTER_MINOR_VERSION],  [clutter_minor_version])
AC_SUBST([CLUTTER_MICRO_VERSION],  [clutter_micro_version])
AC_SUBST([CLUTTER_VERSION],        [clutter_version])
AC_SUBST([CLUTTER_API_VERSION],    [clutter_api_version])
AC_SUBST([CLUTTER_API_VERSION_AM], [$CLUTTER_MAJOR_VERSION\_0])
AC_SUBST([CLUTTER_RELEASE_STATUS], [clutter_release_status])

m4_define([lt_current],  [m4_eval(100 * clutter_minor_version + clutter_micro_version - clutter_interface_age)])
m4_define([lt_revision], [clutter_interface_age])
m4_define([lt_age],      [m4_eval(clutter_binary_age - clutter_interface_age)])
CLUTTER_LT_CURRENT=lt_current
CLUTTER_LT_REV=lt_revision
CLUTTER_LT_AGE=lt_age
CLUTTER_LT_VERSION="$CLUTTER_LT_CURRENT:$CLUTTER_LT_REV:$CLUTTER_LT_AGE"
CLUTTER_LT_LDFLAGS="-version-info $CLUTTER_LT_VERSION"
AC_SUBST([CLUTTER_LT_CURRENT],  [lt_current])
AC_SUBST([CLUTTER_LT_REVISION], [lt_revision])
AC_SUBST([CLUTTER_LT_VERSION],  [$CLUTTER_LT_VERSION])

AC_DEFINE([OS_LINUX], [1], [Define to 1 if building for Linux])

AC_SUBST(CLUTTER_LT_LDFLAGS)

m4_define([glib_req_version],           [2.44.0])
m4_define([cogl_req_version],           [1.21.2])
m4_define([json_glib_req_version],      [0.12.0])
m4_define([atk_req_version],            [2.5.3])
m4_define([cairo_req_version],          [1.14.0])
m4_define([pango_req_version],          [1.30])
m4_define([gi_req_version],             [1.39.0])
m4_define([xcomposite_req_version],     [0.4])
m4_define([gdk_req_version],            [3.3.18])
m4_define([libinput_req_version],       [0.19.0])
m4_define([libudev_req_version],        [136])

AC_SUBST([GLIB_REQ_VERSION],       [glib_req_version])
AC_SUBST([COGL_REQ_VERSION],       [cogl_req_version])
AC_SUBST([JSON_GLIB_REQ_VERSION],  [json_glib_req_version])
AC_SUBST([ATK_REQ_VERSION],        [atk_req_version])
AC_SUBST([CAIRO_REQ_VERSION],      [cairo_req_version])
AC_SUBST([PANGO_REQ_VERSION],      [pango_req_version])
AC_SUBST([GI_REQ_VERSION],         [gi_req_version])
AC_SUBST([XCOMPOSITE_REQ_VERSION], [xcomposite_req_version])
AC_SUBST([GDK_REQ_VERSION],        [gdk_req_version])
AC_SUBST([LIBINPUT_REQ_VERSION],   [libinput_req_version])
AC_SUBST([LIBUDEV_REQ_VERSION],    [libudev_req_version])

FLAVOUR_LIBS=""
FLAVOUR_CFLAGS=""
CLUTTER_BACKENDS=""
CLUTTER_INPUT_BACKENDS=""
CLUTTER_CONFIG_DEFINES=

experimental_backend=no
experimental_input_backend=no

# base dependencies for core
CLUTTER_BASE_PC_FILES="cogl-1.0 >= $COGL_REQ_VERSION cogl-path-1.0 cairo-gobject >= $CAIRO_REQ_VERSION gio-2.0 >= glib_req_version atk >= $ATK_REQ_VERSION pangocairo >= $PANGO_REQ_VERSION cogl-pango-1.0 json-glib-1.0 >= $JSON_GLIB_REQ_VERSION"

# private base dependencies
CLUTTER_BASE_PC_FILES_PRIVATE=""

# backend specific pkg-config files
BACKEND_PC_FILES="x11 egl libinput"

# private backend specific dependencies
BACKEND_PC_FILES_PRIVATE=""

dnl === Clutter windowing system backend ======================================

CLUTTER_BACKENDS="$CLUTTER_BACKENDS x11"
CLUTTER_INPUT_BACKENDS="$CLUTTER_INPUT_BACKENDS x11"

SUPPORT_X11=1
SUPPORT_GLX=1
SUPPORT_COGL=1

# we use fontconfig API and pango-ft2 when the fontconfig
# configuration changes; we don't expose any API for this
# so we add pango-ft2 to the private Requires.
PKG_CHECK_EXISTS([pangoft2],
                 [
                         AC_DEFINE([HAVE_PANGO_FT2], [1], [Supports PangoFt2])
                         BACKEND_PC_FILES_PRIVATE="$BACKEND_PC_FILES_PRIVATE pangoft2"
                 ],
                 [])

PKG_CHECK_EXISTS([wayland-server],
                 [BACKEND_PC_FILES="$BACKEND_PC_FILES wayland-server"], [])

SUPPORT_WAYLAND_COMPOSITOR=1
SUPPORT_COGL=1

CLUTTER_CONFIG_DEFINES="$CLUTTER_CONFIG_DEFINES
#define CLUTTER_HAS_WAYLAND_COMPOSITOR_SUPPORT 1"

CLUTTER_BACKENDS="$CLUTTER_BACKENDS egl"

SUPPORT_EGL=1
SUPPORT_COGL=1
SUPPORT_EVDEV=1

AC_DEFINE([CLUTTER_EGL_BACKEND_GENERIC], [1], [Use Generic EGL backend])
AC_DEFINE([OS_LINUX], [1], [Define to 1 if building for Linux])

CLUTTER_INPUT_BACKENDS="$CLUTTER_INPUT_BACKENDS evdev"
BACKEND_PC_FILES_PRIVATE="$BACKEND_PC_FILES_PRIVATE libudev >= $LIBUDEV_REQ_VERSION libinput >= $LIBINPUT_REQ_VERSION xkbcommon"

dnl Instead of using AM_CFLAGS to ensure
dnl COGL_ENABLE_EXPERIMENTAL_2_0_API is defined while compiling clutter
dnl and cogl code we use a define in config.h instead. This helps ensure
dnl other tools such as glib-mkenums and gir-scanner don't end up
dnl using the define also.
AC_DEFINE([COGL_ENABLE_EXPERIMENTAL_2_0_API], [1], [Can use Cogl 2.0 API internally])
AC_DEFINE_UNQUOTED([CLUTTER_DRIVERS], ["*"], [List of Cogl drivers])

dnl strip leading spaces
CLUTTER_BACKENDS=${CLUTTER_BACKENDS#* }
AC_SUBST(CLUTTER_BACKENDS)

CLUTTER_INPUT_BACKENDS=${CLUTTER_INPUT_BACKENDS#* }
AC_SUBST(CLUTTER_INPUT_BACKENDS)

AC_CACHE_SAVE

dnl === Clutter configuration =================================================

# windowing systems
AS_IF([test "x$SUPPORT_X11" = "x1"],
      [CLUTTER_CONFIG_DEFINES="$CLUTTER_CONFIG_DEFINES
#define CLUTTER_WINDOWING_X11 \"x11\"
#define CLUTTER_INPUT_X11 \"x11\""])
AS_IF([test "x$SUPPORT_GLX" = "x1"],
      [CLUTTER_CONFIG_DEFINES="$CLUTTER_CONFIG_DEFINES
#define CLUTTER_WINDOWING_GLX \"glx\""])
AS_IF([test "x$SUPPORT_EGL" = "x1"],
      [CLUTTER_CONFIG_DEFINES="$CLUTTER_CONFIG_DEFINES
#define CLUTTER_WINDOWING_EGL \"eglnative\""])
AS_IF([test "x$SUPPORT_EVDEV" = "x1"],
      [CLUTTER_CONFIG_DEFINES="$CLUTTER_CONFIG_DEFINES
#define CLUTTER_INPUT_EVDEV \"evdev\""])

# the 'null' input backend is special
CLUTTER_CONFIG_DEFINES="$CLUTTER_CONFIG_DEFINES
#define CLUTTER_INPUT_NULL \"null\""

AC_SUBST([CLUTTER_CONFIG_DEFINES])

dnl === Clutter substitutions kept for backwards compatibility ================
AC_SUBST([CLUTTER_WINSYS],       [deprecated])
AC_SUBST([CLUTTER_WINSYS_BASE],  [deprecated])
AC_SUBST([CLUTTER_STAGE_TYPE],   [deprecated])
AC_SUBST([CLUTTER_SONAME_INFIX], [deprecated])
AC_SUBST([CLUTTER_FLAVOUR],      [deprecated])
AC_SUBST([CLUTTER_COGL],         [deprecated])
AC_SUBST([COGL_DRIVER],          [deprecated])

dnl === X11 checks, only for X11-based backends ===============================
X11_PC_FILES=""
X11_EXTS=""

AS_IF([test "x$SUPPORT_X11" = "x1"],
      [
        # base X11 includes and libraries
        AC_MSG_CHECKING([for X11])

        # start with pkg-config
        PKG_CHECK_EXISTS([x11], [have_x11=yes], [have_x11=no])
        AS_IF([test "x$have_x11" = "xyes"],
              [
                X11_PC_FILES="x11"

                # we actually need to ask for CFLAGS and LIBS
                X11_CFLAGS=`$PKG_CONFIG --cflags $X11_PC_FILES`
                X11_LIBS=`$PKG_CONFIG --libs $X11_PC_FILES`

                AC_MSG_RESULT([found])
              ],
              [
                # no pkg-config, let's go old school
                AC_PATH_X

                AS_IF([test "x$no_x" = "xyes"],
                      [AC_MSG_ERROR([No X11 Libraries found])],
                      [
                        AS_IF([test "x$x_includes" != "xNONE" && test -n "$x_includes"],
                              [X11_CFLAGS=-I`echo $x_includes | sed -e "s/:/ -I/g"`])

                        AS_IF([test "x$x_libraries" != "xNONE" && test -n "$x_libraries"],
                              [X11_LIBS=-L`echo $x_libraries | sed -e "s/:/ -L/g"`])

                        AC_MSG_RESULT([found])
                      ]
                )
              ]
        )

        # XEXT (required)
        AC_MSG_CHECKING([for XEXT extension])
        PKG_CHECK_EXISTS([xext], [have_xext=yes], [have_xext=no])
        AS_IF([test "x$have_xext" = "xyes"],
              [
                AC_DEFINE(HAVE_XEXT, [1], [Define to 1 if we have the XEXT X extension])

                X11_LIBS="$X11_LIBS -lXext"
                X11_PC_FILES="$X11_PC_FILES xext"
                X11_EXTS="$X11_EXTS xext"

                AC_MSG_RESULT([found])
              ],
              [AC_MSG_ERROR([Not found])]
        )

        # XDAMAGE (required)
        AC_MSG_CHECKING([for XDAMAGE extension])
        PKG_CHECK_EXISTS([xdamage], [have_xdamage=yes], [have_xdamage=no])
        AS_IF([test "x$have_xdamage" = "xyes"],
              [
                AC_DEFINE(HAVE_XDAMAGE, [1], [Define to 1 if we have the XDAMAGE X extension])

                X11_LIBS="$X11_LIBS -lXdamage"
                X11_PC_FILES="$X11_PC_FILES xdamage"
                X11_EXTS="$X11_EXTS xdamage"

                AC_MSG_RESULT([found])
              ],
              [AC_MSG_ERROR([not found])]
        )

        # XCOMPOSITE (optional)
        AC_MSG_CHECKING([for XCOMPOSITE extension >= $XCOMPOSITE_REQ_VERSION])
        PKG_CHECK_EXISTS([xcomposite >= $XCOMPOSITE_REQ_VERSION], [have_xcomposite=yes], [have_xcomposite=no])
        AS_IF([test "x$have_xcomposite" = "xyes"],
              [
                AC_DEFINE(HAVE_XCOMPOSITE, [1], [Define to 1 if we have the XCOMPOSITE X extension])

                X11_LIBS="$X11_LIBS -lXcomposite"
                X11_PC_FILES="$X11_PC_FILES xcomposite >= $XCOMPOSITE_REQ_VERSION"
                X11_EXTS="$X11_EXTS xcomposite"

                AC_MSG_RESULT([found])
              ],
              [AC_MSG_ERROR([not found])]
        )

        # X Generic Extensions (optional)
        clutter_save_CPPFLAGS="$CPPFLAGS"
        CPPFLAGS="$CPPFLAGS $X11_CFLAGS"

        clutter_save_LIBS="$LIBS"
        LIBS="$LIBS $X11_LIBS"

        have_xge=no
        AC_CHECK_FUNC([XGetEventData],
                      [
                        AC_DEFINE([HAVE_XGE], [1], [Define to 1 if X Generic Extensions is available])
                        have_xge=yes
                        X11_EXTS="$X11_EXTS xge"
                      ])

        CPPFLAGS="$clutter_save_CPPFLAGS"
        LIBS="$clutter_save_LIBS"

        # XI (optional)
        AC_ARG_ENABLE([xinput],
                      [AS_HELP_STRING([--enable-xinput], [Use the XI X extension])],
                      [],
                      [enable_xinput=yes])

        AS_IF([test "x$enable_xinput" = "xyes"],
              [
                PKG_CHECK_EXISTS([xi], [have_xinput=yes], [have_xinput=no])
              ],
              [
                have_xinput=no
              ])

        AS_CASE([$have_xinput],

                [yes],
                [
                  AC_CHECK_HEADERS([X11/extensions/XInput2.h],
                                   [
                                     have_xinput2=yes
                                     AC_DEFINE([HAVE_XINPUT_2],
                                               [1],
                                               [Define to 1 if XI2 is available])
                                   ])

                  clutter_save_LIBS="$LIBS"
                  LIBS="$LIBS -lXi"

                  AC_CHECK_FUNC([XIAllowTouchEvents],
                                [
                                  AC_CHECK_MEMBER([XIScrollClassInfo.number],
                                                  [
                                                    have_xinput_2_2=yes
                                                    AC_DEFINE([HAVE_XINPUT_2_2], [1], [Define to 1 if XInput 2.2 is available])
                                                  ],
                                                  [have_xinput_2_2=no],
                                                  [[#include <X11/extensions/XInput2.h>]])])

                  LIBS="$clutter_save_LIBS"

                  X11_LIBS="$X11_LIBS $XINPUT_LIBS"
                  X11_PC_FILES="$X11_PC_FILES xi"

                  AS_IF([test "x$have_xinput_2_2" = "xyes"],
                        [X11_EXTS="$X11_EXTS xi2.2"],
                        [X11_EXTS="$X11_EXTS xi2.0"])
                ],

                [no],
                [have_xinput2=no],

                [*],
                [AC_MSG_ERROR([Invalid argument for --enable-xinput])]
        )

        # XKB
        clutter_save_CPPFLAGS="$CPPFLAGS"
        CPPFLAGS="$CPPFLAGS $X11_CFLAGS"

        clutter_save_LIBS="$LIBS"
        LIBS="$LIBS $X11_LIBS"

        have_xkb=no
        AC_CHECK_FUNC([XkbQueryExtension],
                      [
                        AC_DEFINE(HAVE_XKB, 1, [Define to use XKB extension])
                        have_xkb=yes

                        X11_EXTS="$X11_EXTS xkb"
                      ])

        CPPFLAGS="$clutter_save_CPPFLAGS"
        LIBS="$clutter_save_LIBS"

        BACKEND_PC_FILES="$BACKEND_PC_FILES $X11_PC_FILES"
        FLAVOUR_LIBS="$FLAVOUR_LIBS $X11_LIBS"
        FLAVOUR_CFLAGS="$FLAVOUR_CFLAGS $X11_CFLAGS"
      ]
)

AM_CONDITIONAL([BUILD_XI2], [test "x$have_xinput2" = "xyes"])

X11_EXTS=${X11_EXTS#* }

# strip leading space
BACKEND_PC_FILES=${BACKEND_PC_FILES#* }

# public dependencies, will fill the Requires: field of clutter.pc
CLUTTER_REQUIRES="$CLUTTER_BASE_PC_FILES $BACKEND_PC_FILES"
PKG_CHECK_MODULES(CLUTTER_DEPS, [$CLUTTER_REQUIRES])

# private dependencies, will fill the Requires.private: field of clutter.pc
AS_IF([test "x$CLUTTER_BASE_PC_FILES_PRIVATE" = "x" && test "x$BACKEND_PC_FILES_PRIVATE" = "x"],
      [
        CLUTTER_REQUIRES_PRIVATE=""
        CLUTTER_DEPS_PRIVATE_CFLAGS=""
        CLUTTER_DEPS_PRIVATE_LIBS=""
      ],
      [
        CLUTTER_REQUIRES_PRIVATE="$CLUTTER_BASE_PC_FILES_PRIVATE $BACKEND_PC_FILES_PRIVATE"
        PKG_CHECK_MODULES(CLUTTER_DEPS_PRIVATE, [$CLUTTER_REQUIRES_PRIVATE])
      ])

AC_SUBST(CLUTTER_REQUIRES)
AC_SUBST(CLUTTER_REQUIRES_PRIVATE)

CLUTTER_CFLAGS="$FLAVOUR_CFLAGS $CLUTTER_DEPS_CFLAGS $CLUTTER_DEPS_PRIVATE_CFLAGS $GLIB_CFLAGS"
CLUTTER_LIBS="$FLAVOUR_LIBS $CLUTTER_DEPS_LIBS $CLUTTER_DEPS_PRIVATE_LIBS $GLIB_LIBS"
AC_SUBST(CLUTTER_CFLAGS)
AC_SUBST(CLUTTER_LIBS)

AC_CONFIG_FILES([
Makefile
data/Makefile
doc/Makefile
doc/man/Makefile
src/Makefile
src/libmutter.pc
src/compositor/plugins/Makefile
src/clutter/Makefile
src/clutter/clutter-config.h
src/clutter/clutter-version.h
src/meta/meta-version.h
po/Makefile.in
])

AC_OUTPUT
