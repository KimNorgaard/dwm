/* See LICENSE file for copyright and license details. */
#include "X11/XF86keysym.h"

#include "push.c"

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
// static const char *fonts[]          = { "Iosevka Term:hinting=True:antialias=True:size=10" };
// static const char dmenufont[]       = "Iosevka Term:hinting=True:antialias=True:size=10";
static const char *fonts[]          = { "monospace:size=12" };
static const char dmenufont[]       = "monospace:size=12";
static const char black[]           = "#000000";
static const char red[]             = "#cc241d";
static const char green[]           = "#98971a";
static const char yellow[]          = "#d79921";
static const char blue[]            = "#458588";
static const char magenta[]         = "#b16286";
static const char cyan[]            = "#689d6a";
static const char white[]           = "#a89984";
static const char bright_black[]    = "#928374";
static const char bright_red[]      = "#fb4934";
static const char bright_green[]    = "#b8bb26";
static const char bright_yellow[]   = "#fabd2f";
static const char bright_blue[]     = "#83a598";
static const char bright_magenta[]  = "#d3869b";
static const char bright_cyan[]     = "#8ec07c";
static const char bright_white[]    = "#ebdbb2";
static const char norm_color[] = "#eeeeee";
static const char blue_bg[] = "#114488";
static const char blue_fg[] = "#82a9dc";
static const char blue_fg2[] = "#09346f";
static const char *colors[][3]      = {
	/*               fg             bg         border   */
	[SchemeNorm] = { blue_fg,       blue_bg,   blue_fg2 },
	[SchemeSel]  = { norm_color,    blue_bg,   blue_fg  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class          instance    title             tags mask     isfloating   monitor */
	{ "MPlayer",      NULL,       NULL,             0,            1,           -1 },
	{ "Firefox",      NULL,       NULL,             1,            0,           -1 },
	{ "HipChat",      NULL,       NULL,             1<<3,         0,           -1 },
	{ "Termite",      NULL,       "mutt",           1<<1,         0,           -1 },
	{ "st-256color",  NULL,       "mutt",           1<<1,         0,           -1 },
	{ "st-256color",  NULL,       "scratchpad",     0,            1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.60; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", black, "-nf", bright_black, "-sb", black, "-sf", bright_yellow, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "st", "-t", scratchpadname, "-g", "120x34", "-e", "nvim", NULL };
static const char *dpmscmd[] = { "toggle_dpms", NULL };
static const char *keymapcmd[] = { "keymap", "togglenotify", NULL };
static const char *lockcmd[] = { "lockscreen", NULL };
static const char *scrotcmd[] = { "scrot", NULL };
static const char *scrotfocusedcmd[] = { "scrot", "-u", NULL };
static const char *scrotselectcmd[] = { "scrot_select", NULL };
static const char *scrotclipboardcmd[] = { "scrot2clipboard", NULL };
static const char *voldowncmd[] = { "volume", "down", NULL };
static const char *volupcmd[] = { "volume", "up", NULL };
static const char *micmutecmd[] = { "pactl", "set-source-mute", "alsa_input.pci-0000_00_1f.3.analog-stereo", "toggle", NULL };
static const char *volmutecmd[] = { "volume", "mute", NULL };
static const char *volshowcmd[] = { "volume", NULL };
static const char *brightupcmd[] = { "sudo", "/etc/acpi/backlight.sh", "+", NULL };
static const char *brightdowncmd[] = { "sudo", "/etc/acpi/backlight.sh", "-", NULL };
static const char *dmenucalccmd[] = { "=", "--dmenu=dmenu", NULL };
static const char *clipmenucmd[] = { "clipmenu", "-i", "-m", dmenumon, "-fn", dmenufont, "-nb", black, "-nf", bright_black, "-sb", black, "-sf", bright_yellow, NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ 0,                            XF86XK_Launch2,            spawn,          {.v = dpmscmd } },
	{ 0,                            XF86XK_Launch3,            spawn,          {.v = keymapcmd } },
	{ 0,                            XF86XK_AudioLowerVolume,   spawn,          {.v = voldowncmd } },
	{ 0,                            XF86XK_AudioRaiseVolume,   spawn,          {.v = volupcmd } },
	{ 0,                            XF86XK_AudioMute,          spawn,          {.v = volmutecmd } },
	{ 0,                            XF86XK_AudioMicMute,       spawn,          {.v = micmutecmd } },
	{ ShiftMask,                    XF86XK_AudioMute,          spawn,          {.v = volshowcmd } },
	{ 0,                            XF86XK_MonBrightnessUp,    spawn,          {.v = brightupcmd } },
	{ 0,                            XF86XK_MonBrightnessDown,  spawn,          {.v = brightdowncmd } },
	{ MODKEY,                       XK_Print,                  spawn,          {.v = keymapcmd } },
	{ 0,                            XK_Print,                  spawn,          {.v = scrotcmd } },
	{ ControlMask,                  XK_Print,                  spawn,          {.v = scrotselectcmd } },
	{ ShiftMask,                    XK_Print,                  spawn,          {.v = scrotfocusedcmd } },
	{ MODKEY|ShiftMask,             XK_Print,                  spawn,          {.v = scrotclipboardcmd } },
	{ MODKEY,                       XK_Escape,                 spawn,          {.v = lockcmd } },
	{ MODKEY,                       XK_x,                      spawn,          {.v = dmenucalccmd } },
	{ MODKEY,                       XK_c,                      spawn,          {.v = clipmenucmd } },
	{ MODKEY,                       XK_p,                      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return,                 spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_grave,                  togglescratch,  {.v = scratchpadcmd } },
	{ MODKEY,                       XK_b,                      togglebar,      {0} },
	{ MODKEY|ControlMask,           XK_j,                      pushdown,       {0} },
	{ MODKEY|ControlMask,           XK_k,                      pushup,         {0} },
	{ MODKEY,                       XK_j,                      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,                      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,                      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,                      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,                      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,                      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return,                 zoom,           {0} },
	{ MODKEY,                       XK_Tab,                    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,                      killclient,     {0} },
	{ MODKEY,                       XK_t,                      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,                      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,                      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_u,                      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_o,                      setlayout,      {.v = &layouts[4]} },
	{ MODKEY,                       XK_space,                  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,                  togglefloating, {0} },
	{ MODKEY,                       XK_0,                      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,                      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,                  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period,                 focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,                  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period,                 tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

