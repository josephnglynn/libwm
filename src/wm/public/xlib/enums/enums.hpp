//
// Created by joseph on 27/11/2021.
//

#ifndef FLOW_WM_SRC_WM_PUBLIC_XLIB_EMUMS_ENUMS_HPP
#define FLOW_WM_SRC_WM_PUBLIC_XLIB_EMUMS_ENUMS_HPP

enum
{
	CurResizeBottomRight, CurResizeBottomLeft, CurResizeTopRight, CurResizeTopLeft, CurNormal, CurMove, CurLast
};

enum
{
	NetSupported, NetWMName, NetWMState, NetWMCheck,
	NetWMFullscreen, NetActiveWindow, NetWMWindowType,
	NetWMWindowTypeDialog, NetClientList, NetLast
};

enum
{
	WMProtocols, WMDelete, WMState, WMTakeFocus, WMLast
};

enum
{
	SchemeNorm, SchemeSel
};

enum
{
	ColFg, ColBg, ColBorder
};

enum
{
	ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
	ClkClientWin, ClkRootWin, ClkLast
};

#endif //FLOW_WM_SRC_WM_PUBLIC_XLIB_EMUMS_ENUMS_HPP
