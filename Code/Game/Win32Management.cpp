/*
	This file contains all of the function definitions for this example program
*/

// Header Files
//=============

#include "precompiled.h"
#include "Win32Management.h"

// Resource.h contains the #defines for the icon resources
// that the main window will use
#include "Resources\Resource.h"

// Static Data Initialization
//===========================

namespace Win32Management
{
	WindowsManager * WindowsManager::m_Instance = NULL;
	HWND WindowsManager::m_mainWindow = NULL;

	bool WindowsManager::CreateInstance(const HINSTANCE i_thisInstanceOfTheProgram,
										const int i_initialWindowDisplayState,
										const std::string& i_MainWindowClassName,
										const std::string& i_windowCaption,
										const unsigned int i_windowWidth,
										const unsigned int i_windowHeight,
										const bool i_shouldRenderFullScreen)
	{
		if (m_Instance == NULL)
		{
			m_Instance = new WindowsManager(i_thisInstanceOfTheProgram, i_initialWindowDisplayState, i_MainWindowClassName,
											i_windowCaption, i_windowWidth, i_windowHeight, i_shouldRenderFullScreen);
			assert(m_Instance != NULL);
		}

		return m_Instance != NULL;
	}

	WindowsManager* WindowsManager::GetInstance()
	{
		if (m_Instance != NULL)
		{
			return m_Instance;
		}

		assert(false);

		return NULL;
	}

	void WindowsManager::Destroy()
	{
		if (m_Instance != NULL)
		{
			delete m_Instance;
		}
		else
		{
			assert(false);
		}
	}

	WindowsManager::WindowsManager(const HINSTANCE i_thisInstanceOfTheProgram,
		const int i_initialWindowDisplayState,
		const std::string& i_MainWindowClassName,
		const std::string& i_windowCaption,
		const unsigned int i_windowWidth,
		const unsigned int i_windowHeight,
		const bool i_shouldRenderFullScreen) :
		m_thisInstanceOfTheProgram(i_thisInstanceOfTheProgram),
		m_initialWindowDisplayState(i_initialWindowDisplayState),
		m_mainWindowClass_name(i_MainWindowClassName),
		m_windowCaption(i_windowCaption),
		m_windowWidth(i_windowWidth),
		m_windowHeight(i_windowHeight),
		m_shouldRenderFullScreen(i_shouldRenderFullScreen)
	{
		if (false == CreateMainWindow(m_thisInstanceOfTheProgram, m_initialWindowDisplayState))
		{
			assert(false);
		}
	}

	WindowsManager::~WindowsManager()
	{
		ShutdownMainWindow(m_thisInstanceOfTheProgram);
	}

	HWND WindowsManager::GetReferenceToMainWindowHandle(void)
	{
		assert(m_mainWindow != NULL);

		return m_mainWindow;
	}

	bool WindowsManager::CreateMainWindow(const HINSTANCE i_thisInstanceOfTheProgram, const int i_initialWindowDisplayState)
	{
		// Every window that Windows creates must belong to a "class".
		// Note that this is different than a C++ class (but similar in theory):
		// A windows class defines things that will be the same for every window
		// that belongs to the class.
		// (In other words, every window will have exactly one class,
		// but a windows class can have many windows.)
		// To create a new windows class it must be "registered" with Windows.
		ATOM mainWindowClass = RegisterMainWindowClass( i_thisInstanceOfTheProgram );
		if ( mainWindowClass != NULL )
		{
			m_mainWindow = CreateMainWindowHandle( i_thisInstanceOfTheProgram, i_initialWindowDisplayState );
			if (m_mainWindow == NULL)
			{
				goto OnError;
			}

			return true;

	OnError:

			// Unregister the main window class
			{
				UnregisterMainWindowClass( i_thisInstanceOfTheProgram );
				mainWindowClass = NULL;
			}

			return false;
		}
		else
		{
			return false;
		}
	}

	std::string WindowsManager::GetLastWindowsError()
	{
		// Windows stores the error as a code
		const DWORD errorCode = GetLastError();
		// Format the message
		std::string errorMessage;
		{
			const DWORD formattingOptions =
				// Get the error message from Windows
				FORMAT_MESSAGE_FROM_SYSTEM
				// The space for the error message should be allocated by Windows
				| FORMAT_MESSAGE_ALLOCATE_BUFFER
				// Any potentially deferred inserts should be ignored
				// (i.e. the error message should be in its final form)
				| FORMAT_MESSAGE_IGNORE_INSERTS;
			const void* messageIsFromWindows = NULL;
			const DWORD useTheDefaultLanguage = 0;
			char* messageBuffer = NULL;
			const DWORD minimumCharacterCountToAllocate = 1;
			va_list* insertsAreIgnored = NULL;
			const DWORD storedCharacterCount = FormatMessage( formattingOptions, messageIsFromWindows, errorCode,
				useTheDefaultLanguage, reinterpret_cast<LPSTR>( &messageBuffer ), minimumCharacterCountToAllocate, insertsAreIgnored );
			if ( storedCharacterCount != 0 )
			{
				errorMessage = messageBuffer;
			}
			else
			{
				// If there's an error GetLastError() can be called again,
				// but that is too complicated for this program :)
				errorMessage = "Unknown Windows Error";
			}
			// Try to free the memory regardless of whether formatting worked or not,
			// and ignore any error messages
			LocalFree( messageBuffer );
		}
		return errorMessage;
	}

	// CreateMainWindow
	//-----------------

	HWND WindowsManager::CreateMainWindowHandle(const HINSTANCE i_thisInstanceOfTheProgram, const int i_initialWindowDisplayState)
	{
		// Create the main window
		HWND mainWindow;
		{
			// The window's "caption"
			// (The text that is displayed in the title bar)
			const char* windowCaption = m_windowCaption.c_str();
			// The window's style
			const DWORD windowStyle =
				// "Overlapped" is basically the same as "top-level"
				WS_OVERLAPPED
				// The caption is the title bar when in windowed-mode
				| WS_CAPTION
				// The window should never change dimensions, so only a minimize box is allowed
				| WS_MINIMIZEBOX
				// The system menu appears when you right-click the title bar
				| WS_SYSMENU;
			// The window's extended style
			const DWORD windowStyle_extended =
				// The following is a macro to make the extended style the default top-level look
				WS_EX_OVERLAPPEDWINDOW;
			// The width and height of the window.
			// A game cares about the width and height of the actual "client area",
			// which is the part of the window that doesn't include the borders and title bar;
			// this means that if we say that a game runs at a resolution of 800 x 600,
			// the actual window will be slightly bigger than that.
			// Initially, then, the window will be created with default values that Windows chooses
			// and then resized after creation)
			const int width = CW_USEDEFAULT;
			const int height = CW_USEDEFAULT;
			// The initial position of the window
			// (We don't care, and will let Windows decide)
			const int position_x = CW_USEDEFAULT;
			const int position_y = CW_USEDEFAULT;
			// Handle to the parent of this window
			// (Since this is our main window, it can't have a parent)
			const HWND hParent = NULL;
			// Handle to the menu for this window
			// (The main window won't have a menu)
			const HMENU hMenu = NULL;
			// Handle to the instance of the program that this window should be associated with
			const HINSTANCE hProgram = i_thisInstanceOfTheProgram;
			// Any arbitrary pointer can be associated with this window;
			// usually a program's own custom representation of the window will be used, like this:
			// void* userData = someCustomStructOrClass;
			// Since this example program is simple, though, no data will be associated:
			void* userData = NULL;

			// Ask Windows to create the specified window.
			// CreateWindowEx() will return a handle to the window,
			// which is what we'll use to communicate with Windows about this window
			mainWindow = CreateWindowEx( windowStyle_extended, m_mainWindowClass_name.c_str(), windowCaption, windowStyle,
											position_x, position_y, width, height,
											hParent, hMenu, hProgram, userData );
			if ( mainWindow == NULL )
			{
				const char* errorCaption = "No Main Window";
				std::string errorMessage( "Windows failed to create the main window: " );
				errorMessage += GetLastWindowsError();
				MessageBox( NULL, errorMessage.c_str(), errorCaption, MB_OK | MB_ICONERROR );
				return NULL;
			}
		}

		// Change the window's size based on the desired client area resolution
		{
			// In a real game these values would come from an external source
			// rather than be hard-coded
			const int desiredWidth = m_windowWidth;
			const int desiredHeight = m_windowHeight;

			// Calculate how much of the window is coming from the "non-client area"
			// (the borders and title bar)
			RECT windowCoordinates;
			struct
			{
				long width;
				long height;
			} nonClientAreaSize;
			{
				// Get the coordinates of the entire window
				if ( GetWindowRect( mainWindow, &windowCoordinates ) == FALSE )
				{
					std::string errorMessage( "Windows failed to get the coordinates of the main window: " );
					errorMessage += GetLastWindowsError();
					MessageBox( NULL, errorMessage.c_str(), NULL, MB_OK | MB_ICONERROR );
					goto OnError;
				}
				// Get the dimensions of the client area
				RECT clientDimensions;
				if ( GetClientRect( mainWindow, &clientDimensions ) == FALSE )
				{
					std::string errorMessage( "Windows failed to get the dimensions of the main window's client area: " );
					errorMessage += GetLastWindowsError();
					MessageBox( NULL, errorMessage.c_str(), NULL, MB_OK | MB_ICONERROR );
					goto OnError;
				}
				// Get the difference between them
				nonClientAreaSize.width = ( windowCoordinates.right - windowCoordinates.left ) - clientDimensions.right;
				nonClientAreaSize.height = ( windowCoordinates.bottom - windowCoordinates.top ) - clientDimensions.bottom;
			}
			// Resize the window
			{
				BOOL shouldTheResizedWindowBeRedrawn = TRUE;
				if ( MoveWindow( mainWindow, windowCoordinates.left, windowCoordinates.top,
					desiredWidth + nonClientAreaSize.width, desiredHeight + nonClientAreaSize.height,
					shouldTheResizedWindowBeRedrawn ) == FALSE )
				{
					std::string errorMessage( "Windows failed to resize the main window: " );
					errorMessage += GetLastWindowsError();
					MessageBox( NULL, errorMessage.c_str(), NULL, MB_OK | MB_ICONERROR );
					goto OnError;
				}
			}
		}

		// Display the window in the initial state that Windows requested
		ShowWindow( mainWindow, i_initialWindowDisplayState );

		return mainWindow;

	OnError:

		if ( DestroyWindow( mainWindow ) == FALSE )
		{
			std::string errorMessage( "Windows failed to destroy the main window "
				"after an error in creation: " );
			errorMessage += GetLastWindowsError();
			MessageBox( NULL, errorMessage.c_str(), NULL, MB_OK | MB_ICONERROR );
		}
		mainWindow = NULL;

		return NULL;
	}

	ATOM WindowsManager::RegisterMainWindowClass(const HINSTANCE i_thisInstanceOfTheProgram)
	{
		WNDCLASSEX wndClassEx = { 0 };
		wndClassEx.cbSize = sizeof( WNDCLASSEX );
		wndClassEx.hInstance = i_thisInstanceOfTheProgram;

		// The class's style
		// (We don't have to worry about any of these)
		wndClassEx.style = 0;
		// The function that will process all of the messages
		// that Windows will send to windows of this class
		wndClassEx.lpfnWndProc = OnMessageReceived;
		// Extra bytes can be set aside in the class for user data
		wndClassEx.cbClsExtra = 0;
		// Extra bytes can be set aside for each window of this class,
		// but this is usually specified for each window individually
		wndClassEx.cbWndExtra = 0;
		// The large and small icons that windows of this class should use
		// (These can be found in the Resources folder; feel free to change them)
		wndClassEx.hIcon = LoadIcon(i_thisInstanceOfTheProgram, MAKEINTRESOURCE(IDI_GAME));
		wndClassEx.hIconSm = LoadIcon( i_thisInstanceOfTheProgram, MAKEINTRESOURCE( IDI_SMALL ) );
		// The cursor that should display when the mouse pointer is over windows of this class
		wndClassEx.hCursor = LoadCursor( NULL, IDC_ARROW );
		// The "brush" that windows of this class should use as a background
		// (Setting this is a bit confusing but not important,
		// so don't be alarmed if the next line looks scary)
		wndClassEx.hbrBackground = reinterpret_cast<HBRUSH>( IntToPtr( COLOR_BACKGROUND + 1 ) );
		// A menu can be specified that all windows of this class would use by default,
		// but usually this is set for each window individually
		wndClassEx.lpszMenuName = NULL;
		// The class name (see comments where this is initialized)
		wndClassEx.lpszClassName = m_mainWindowClass_name.c_str();

		// Now all of the above information is given to Windows.
		// If all goes well, the class will be successfully registered
		// and it can be specified by name when creating the main window.
		const ATOM mainWindowClass = RegisterClassEx( &wndClassEx );
		if ( mainWindowClass == NULL )
		{
			const char* errorCaption = "No Main Window Class";
			std::string errorMessage( "Windows failed to register the main window's class: " );
			errorMessage += GetLastWindowsError();
			MessageBox( NULL, errorMessage.c_str(), errorCaption, MB_OK | MB_ICONERROR );
		}
		return mainWindowClass;
	}

	bool WindowsManager::CleanupMainWindow()
	{
		if ( m_mainWindow != NULL )
		{
			if (DestroyWindow(m_mainWindow) != FALSE)
			{
				m_mainWindow = NULL;
			}
			else
			{
				const char* errorCaption = "Couldn't Destroy Main Window";
				std::string errorMessage( "Windows failed to destroy the main window: " );
				errorMessage += GetLastWindowsError();
				MessageBox( NULL, errorMessage.c_str(), errorCaption, MB_OK | MB_ICONERROR );
				return false;
			}
		}
		return true;
	}

	bool WindowsManager::OnMainWindowClosed(const HINSTANCE i_thisInstanceOfTheProgram)
	{
		bool wereThereErrors = false;

		if ( !CleanupMainWindow() )
		{
			wereThereErrors = true;
		}

		if ( !UnregisterMainWindowClass( i_thisInstanceOfTheProgram ) )
		{
			wereThereErrors = true;
		}

		return !wereThereErrors;
	}

	LRESULT CALLBACK WindowsManager::OnMessageReceived(HWND i_window, UINT i_message, WPARAM i_wParam, LPARAM i_lParam)
	{
		// DispatchMessage() will send messages to the main window here.
		// There are many messages that get sent to a window,
		// but games can ignore most of them
		// and let Windows handle them in the default way.

		// Process any messages that the game cares about
		// (any messages that are processed here should return a value
		// rather than letting the default processing function try to handle them a second time)
		switch( i_message )
		{
		// A key has been pressed down, and this is the translated character
		case WM_CHAR:
			{
				// This isn't usually the best way to handle keyboard input in a real-time game,
				// but it is a convenient way to handle an exit key
				// (The WPARAM input parameter indicates which key was pressed,
				// but this example program only cares about the escape key)
				if ( i_wParam == VK_ESCAPE )
				{
					// You do _not_ need to do this in your game,
					// but this example program shows you how to display a "message box"
					// that asks the user for confirmation
					int result;
					{
						const char* caption = "Exit Program?";
						const char* message = "Are you sure you want to quit?";
						result = MessageBox(m_mainWindow, message, caption, MB_YESNO | MB_ICONQUESTION);
					}
					if ( result == IDYES )
					{
						// Instruct Windows to send a WM_QUIT message
						{
							// The exit code is ignored,
							// but just as an example of what's possible a different exit code is sent here
							// than when the user closes the program in the usual way
							// (by clicking the X in the upper-right hand corner).
							int exitCode = 1;
							PostQuitMessage( exitCode );
						}

						// For WM_CHAR messages, return 0 to indicate that it was processed
						return 0;
					}
					else
					{
						// If the user doesn't select "Yes" to quit
						// then do nothing to keep running the program
					}
				}

				// If the key press wasn't handled pass it on to Windows to process in the default way
				break;
			}
		// The window's nonclient area is being destroyed
		case WM_NCDESTROY:
			{
				// This is the last message a window will receive
				// (Any child windows have already been destroyed).
				// After this message has been processed the window's handle will be invalid:
				m_mainWindow = NULL;

				// When the main window is destroyed
				// a WM_QUIT message should be sent
				// (if this isn't done the application would continue to run with no window).
				// This is where the exitCode in WaitForShutdown() comes from:
				int exitCode = 0;	// Arbitrary de facto success code
				PostQuitMessage( exitCode );	// This sends a WM_QUIT message

				// For WM_NCDESTROY messages, return 0 to indicate that it was processed
				return 0;
			}
		}

		// Pass any messages that weren't handled on to Windows
		return DefWindowProc(i_window, i_message, i_wParam, i_lParam);
	}

	bool WindowsManager::UnregisterMainWindowClass(const HINSTANCE i_thisInstanceOfTheProgram)
	{
		if ( UnregisterClass( m_mainWindowClass_name.c_str(), i_thisInstanceOfTheProgram ) != FALSE )
		{
			return true;
		}
		else
		{
			const char* errorCaption = "Couldn't Unregister Main Window Class";
			std::string errorMessage( "Windows failed to unregister the main window's class: " );
			errorMessage += GetLastWindowsError();
			MessageBox( NULL, errorMessage.c_str(), errorCaption, MB_OK | MB_ICONERROR );
			return false;
		}
	}

	void WindowsManager::UpdateMainWindow(int& o_exitCode, bool& o_QuitRequested)
	{
		// Enter an infinite loop that will continue until a quit message (WM_QUIT) is received from Windows
		MSG message = { 0 };
		o_QuitRequested = false;
		bool hasWindowsSentAMessage;
		do
		{
			// To send us a message, Windows will add it to a queue.
			// Most Windows applications should wait until a message is received and then react to it.
			// Real-time programs, though, must continually draw new images to the screen as fast as possible
			// and only pause momentarily when there is a Windows message to deal with.

			// This means that the first thing that must be done every iteration of the game loop is to "peek" at the message queue
			// and see if there are any messages from Windows that need to be handled
		
			{
				HWND getMessagesFromAnyWindowBelongingToTheCurrentThread = NULL;
				unsigned int getAllMessageTypes = 0;
				unsigned int ifAMessageExistsRemoveItFromTheQueue = PM_REMOVE;
				hasWindowsSentAMessage = PeekMessage( &message, getMessagesFromAnyWindowBelongingToTheCurrentThread,
					getAllMessageTypes, getAllMessageTypes, ifAMessageExistsRemoveItFromTheQueue ) == TRUE;
			}
			if ( hasWindowsSentAMessage )
			{
				// If Windows _has_ sent a message, this iteration of the loop will handle it

				// First, the message must be "translated"
				// (Key presses are translated into character messages)
				TranslateMessage( &message );

				// Then, the message is sent on to the appropriate processing function.
				// This function is specified in the lpfnWndProc field of the WNDCLASSEX struct
				// used to register a class with Windows.
				// In the case of the main window in this example program
				// it will always be OnMessageReceived()
				DispatchMessage( &message );

				if (message.message == WM_QUIT)
				{
					o_QuitRequested = true;
				}
			}
		} while ( (hasWindowsSentAMessage == false) && (o_QuitRequested != true) );

		// The exit code for the application is stored in the WPARAM of a WM_QUIT message
		if (o_QuitRequested == true)
		{
			o_exitCode = static_cast<int>( message.wParam );
		}
	
		return;
	}

	void WindowsManager::ShutdownMainWindow(const HINSTANCE i_thisInstanceOfTheProgram)
	{
		// Clean up anything that was created/registered/initialized
		OnMainWindowClosed( i_thisInstanceOfTheProgram );
	}

}