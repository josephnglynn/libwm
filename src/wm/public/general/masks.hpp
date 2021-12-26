//
// Created by joseph on 23/12/2021.
//

#ifndef FLOW_WM_SRC_WM_PUBLIC_GENERAL_MASKS_HPP
#define FLOW_WM_SRC_WM_PUBLIC_GENERAL_MASKS_HPP


#define BUTTON_MASK              (ButtonPressMask|ButtonReleaseMask)
#define MOUSE_MASK               (BUTTON_MASK|PointerMotionMask)
#define FRAME_MASK               (MOUSE_MASK | SubstructureRedirectMask | SubstructureNotifyMask | EnterWindowMask | LeaveWindowMask)
#define CLEAN_MASK(mask)         (mask & ~(keyboard_manager->num_lock_mask|LockMask) & (ShiftMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask))
#define CLIENT_MASK	        (PropertyChangeMask | StructureNotifyMask | FocusChangeMask)
#define CLIENT_NO_PROPAGATE_MASK 	(BUTTON_MASK | ButtonMotionMask)

#endif //FLOW_WM_SRC_WM_PUBLIC_GENERAL_MASKS_HPP
