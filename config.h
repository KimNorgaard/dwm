/* See LICENSE file for copyright and license details. */

#include "X11/XF86keysym.h"

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;    /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "Iosevka:size=12" };
static const char dmenufont[]       = "Iosevka Term:hinting=True:antialias=True:size=10";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char col_fg[] = "#eeeeee";
static const char col_bg[] = "#191919";
static const char col_bg_2[] = "#cccccc";
static const char col_phoenixblue_1[] = "#40BDFF";
static const char col_phoenixblue_2[] = "#5697B8";
static const char *colors[][3]      = {
	/*               fg             bg              border   */
	// [SchemeNorm] = { hn_beige_fg,   hn_orange_bg,   hn_abc },
	// [SchemeSel]  = { black,         hn_orange_bg,   hn_beige_fg  },
	[SchemeNorm] = { col_fg,              col_bg,   col_bg_2 },
	[SchemeSel]  = { col_phoenixblue_2,   col_bg,   col_phoenixblue_1  },
	// [SchemeNorm] = { blue_fg,       blue_bg,   blue_fg2 },
	// [SchemeSel]  = { norm_color,    blue_bg,   blue_fg  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "MPlayer",      NULL,       NULL,             0,            1,           -1 },
	{ "Firefox",      NULL,       NULL,             1,            0,           -1 },
	{ "Alacritty",    NULL,       "work:1:nvim - \" \" 0#,2#,3#",    1<<1,         0,           -1 },
	{ "Alacritty",    NULL,       "scratchpad",     0,            1,           -1 },
	{ "st-256color",  NULL,       "mutt",           1<<1,         0,           -1 },
	{ "st-256color",  NULL,       "scratchpad",     0,            1,           -1 },
	{ "Spotify",      NULL,       NULL,             0,            1,           -1 },
	{ "Rocket.Chat",  NULL,       NULL,             0,            1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.60; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

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
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, NULL };
static const char *termcmd[]  = { "alacritty", NULL };
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "alacritty", "-t", scratchpadname, "-e", "nvim", NULL };
static const char *dpmscmd[] = { "toggle_dpms", NULL };
static const char *keymapcmd[] = { "keymap", "togglenotify", NULL };
static const char *lockcmd[] = { "lockscreen", NULL };
static const char *scrotcmd[] = { "scrot", "-e", "mv $f ~/Downloads/", NULL };
static const char *scrotfocusedcmd[] = { "scrot", "-u", "-e", "mv $f ~/Downloads/", NULL };
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
static const char *clipmenucmd[] = { "clipmenu", "-i", "-m", dmenumon, NULL };

static const Key keys[] = {
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
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_grave,  togglescratch,  {.v = scratchpadcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_u,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_o,      setlayout,      {.v = &layouts[4]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
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
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
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

