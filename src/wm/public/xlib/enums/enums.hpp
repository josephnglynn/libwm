//
// Created by joseph on 27/11/2021.
//

#ifndef FLOW_WM_SRC_WM_PUBLIC_XLIB_EMUMS_ENUMS_HPP
#define FLOW_WM_SRC_WM_PUBLIC_XLIB_EMUMS_ENUMS_HPP

enum
{
	CurResizeTopLeft,
	CurResizeCenterLeft,
	CurResizeBottomLeft,
	CurResizeTopMiddle,
	CurResizeCenterMiddle,
	CurResizeBottomMiddle,
	CurResizeTopRight,
	CurResizeCenterRight,
	CurResizeBottomRight,
	CurNormal,
	CurMove,
	CurLast
};

enum
{
	NetSupported, NetWMName, NetWMState, NetWMCheck,
	NetWMFullscreen, NetActiveWindow, NetWMWindowType,
	NetWMWindowTypeDialog, NetClientList, NetWMTypeDesk, NetWMTypeDock, NetWMStateFs, NetWMStateModel, NetMWMHints, NetWindowOpacity, NetLast
};

enum
{
	WMProtocols, WMDelete, WMState, WMTakeFocus, WMName, WMLast
};

enum
{
	SchemeNorm, SchemeSel
};

enum
{
	ColFg, ColBg, ColBorder, ColButton, ColLast
};

enum
{
	ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
	ClkClientWin, ClkRootWin, ClkLast
};

#endif //FLOW_WM_SRC_WM_PUBLIC_XLIB_EMUMS_ENUMS_HPP
