#ifndef CMSWINDOWSPRIMARYSCREEN_H
#define CMSWINDOWSPRIMARYSCREEN_H

#include "CMSWindowsScreen.h"
#include "IPrimaryScreen.h"
#include "CSynergyHook.h"
#include "MouseTypes.h"
#include "CString.h"

class IScreenReceiver;
class IPrimaryScreenReceiver;

class CMSWindowsPrimaryScreen : public CMSWindowsScreen, public IPrimaryScreen {
public:
	typedef bool (CMSWindowsPrimaryScreen::*HookMethod)(int, WPARAM, LPARAM);

	CMSWindowsPrimaryScreen(IScreenReceiver*, IPrimaryScreenReceiver*);
	virtual ~CMSWindowsPrimaryScreen();

	// IPrimaryScreen overrides
	virtual void		run();
	virtual void		stop();
	virtual void		open();
	virtual void		close();
	virtual void		enter(SInt32 xAbsolute, SInt32 yAbsolute, bool);
	virtual bool		leave();
	virtual void		reconfigure(UInt32 activeSides);
	virtual void		warpCursor(SInt32 xAbsolute, SInt32 yAbsolute);
	virtual void		setClipboard(ClipboardID, const IClipboard*);
	virtual void		grabClipboard(ClipboardID);
	virtual void		getClipboard(ClipboardID, IClipboard*) const;
	virtual KeyModifierMask	getToggleMask() const;
	virtual bool		isLockedToScreen() const;
	
protected:
	// CMSWindowsScreen overrides
	virtual bool		onPreTranslate(const CEvent* event);
	virtual bool		onEvent(CEvent* event);
	virtual CString		getCurrentDesktopName() const;

private:
	SInt32				getJumpZoneSize() const;

	// warp mouse to center of primary display (used when computing
	// motion deltas while mouse is on secondary screen).
	void				warpCursorToCenter();

	void				enterNoWarp();
	bool				showWindow();
	void				hideWindow();

	// check clipboard ownership and, if necessary, tell the receiver
	// of a grab.
	void				checkClipboard();

	// create/destroy window
	// also attach to desktop;  this destroys and recreates the window
	// as necessary.
	void				createWindow();
	void				destroyWindow();

	// start/stop watch for screen saver changes
	void				installScreenSaver();
	void				uninstallScreenSaver();

	// open/close desktop (for windows 95/98/me)
	bool				openDesktop();
	void				closeDesktop();

	// make desk the thread desktop (for windows NT/2000/XP)
	bool				switchDesktop(HDESK desk);

	// discard posted messages
	void				nextMark();

	// key and button queries
	KeyID				mapKey(WPARAM keycode, LPARAM info,
							KeyModifierMask* maskOut);
	ButtonID			mapButton(WPARAM button) const;
	void				updateKeys();
	void				updateKey(UINT vkCode, bool press);

private:
	IScreenReceiver*		m_receiver;
	IPrimaryScreenReceiver*	m_primaryReceiver;

	// true if windows 95/98/me
	bool				m_is95Family;

	// the main loop's thread id
	DWORD				m_threadID;

	// the timer used to check for desktop switching
	UINT				m_timer;

	// the current desk and it's name
	HDESK				m_desk;
	CString				m_deskName;

	// our window (for getting clipboard changes)
	HWND				m_window;

	// m_active is true the hooks are relaying events
	bool				m_active;

	// used to discard queued messages that are no longer needed
	UInt32				m_mark;
	UInt32				m_markReceived;

	// clipboard stuff
	HWND				m_nextClipboardWindow;
	HWND				m_clipboardOwner;

	// map of key state
	BYTE				m_keys[256];

	// last mouse position
	SInt32				m_x, m_y;

	// position of center pixel of screen
	SInt32				m_xCenter, m_yCenter;

	// used to ignore mouse motion
	SInt32				m_mouseMoveIgnore;

	// hook library stuff
	HINSTANCE			m_hookLibrary;
	InitFunc			m_init;
	CleanupFunc			m_cleanup;
	InstallFunc			m_install;
	UninstallFunc		m_uninstall;
	SetSidesFunc		m_setSides;
	SetZoneFunc			m_setZone;
	SetRelayFunc		m_setRelay;
	InstallScreenSaverFunc		m_installScreenSaver;
	UninstallScreenSaverFunc	m_uninstallScreenSaver;

	// stuff for restoring active window
	HWND				m_lastForegroundWindow;
	HWND				m_lastActiveWindow;
	DWORD				m_lastActiveThread;
};

#endif
