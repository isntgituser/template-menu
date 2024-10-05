#pragma once

#include "utils\utils.hpp"
#include "textures\textures.hpp"

extern IMGUI_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND, UINT, WPARAM, LPARAM );

namespace menu::render
{
	/**
	* @brief Configures the ImGui style and loads necessary fonts from web resources.
	*
	* This function sets up the ImGui user interface style, including colors and dimensions.
	* It also loads fonts from specified URLs, including regular and icon fonts, which are added
	* to the ImGui font atlas. If the fonts are not successfully loaded, a debug message is printed.
	*
	* @param device The DirectX device used, mainly for ensuring compatibility or additional configurations.
	*
	* @details The function customizes ImGui's IO settings like disabling the ini and log filenames,
	* adjusts font sizes, manages style colors, and removes window padding and border size.
	* If the required fonts are not loaded properly, a debug message is printed to indicate a failure.
	*
	* @note This function relies on external resources being available at specified URLs,
	* which could lead to issues if the URLs become invalid or the resources are moved.
	*/
	void style( ID3D11Device * );

	/**
	* @brief Renders a full-screen ImGui window with specific style properties.
	*
	* This function sets up and renders a main panel in ImGui, positioned to take up
	* a dynamic size of the screen and with specific window flags that ensure
	* it is interactive in terms of moving and resizing.
	*
	* @param hwnd The handle to the window that the ImGui interface should render over.
	* @param size An ImVec2 structure specifying the dimensions of the window.
	* @param device The DirectX device used for rendering.
	*
	* @return A boolean indicating the state of the window. Returns `true` if the window should remain open.
	*
	* @details The window is rendered as a full-screen overlay with no title bar, movement, resizing, or collapsing capabilities.
	* It uses a specific identifier in ImGui for persistence across frames.
	*
	* @note The function currently does not leverage the `hwnd` and `device` parameters, but they are included for potential future use.
	*/
	bool paint( HWND, ImVec2, ID3D11Device * );
}

namespace menu::window
{
	/// A class responsible for creating and managing a render window with ImGui integration.
	class create_window
	{
	private:
		WNDCLASSEX	wc_				= {}; ///< Handle to the window.
		ImVec2		window_size_	= {}; ///< Current size of the window.
		HWND		hwnd_			= {}; ///< Handle to the window.

		ID3D11Device *device_				= {}; ///< DirectX device.
		ID3D11DeviceContext *device_ctx_	= {}; ///< Device context.
		ID3D11RenderTargetView *render_t_	= {}; ///< Render target view.
		IDXGISwapChain *swap_chain_			= {}; ///< Swap chain for buffer swapping.

	private:
		/**
		* @brief Windows Procedure Callback for handling window messages.
		*
		* @param hwnd Handle to the window.
		* @param msg Message code.
		* @param wparam Additional message information.
		* @param lparam Additional message information.
		*
		* @return Result of message processing. Returns 0 if the message is handled.
		*
		* @details Integrates with ImGui's Win32 message handling. Ignores system command messages
		* related to the ALT key and comments out PostQuitMessage due to known issues.
		*/
		static LRESULT WINAPI wnd_proc( HWND, UINT, WPARAM, LPARAM );

		/**
		* @brief Creates a DirectX device and swap chain.
		*
		* Initializes DirectX 11 resources necessary for rendering, including the device, device context, swap chain,
		* and render target view.
		*
		* @return True if the device and swap chain are successfully created, false otherwise.
		*
		* @details Tries to create the device with hardware acceleration first, falling back to WARP mode
		* if hardware support is unavailable. Also sets up the render target view from the swap chain's back buffer.
		*/
		bool create_device( );

	public:
		/**
		* @brief Constructs a create_window object and initializes a window with DirectX 11 rendering capabilities.
		*
		* @param size A structure of type ImVec2 representing the dimensions of the window to be created.
		*
		* @details This constructor sets up a Windows class and window, enabling DPI awareness and centering the window on the screen.
		* It registers the window class, creates the window with round corners, initializes DirectX 11 device and resources,
		* and finally displays the window. The class uses ImGui for rendering.
		*
		* The window is created as a popup with the dimensions provided in the constructor argument.
		*/
		explicit create_window( const ImVec2 size ) : window_size_( size )
		{
			ImGui_ImplWin32_EnableDpiAwareness( );

			this->wc_ =
			{
				sizeof WNDCLASSEX,
				CS_CLASSDC,
				this->wnd_proc,
				0L,
				0L,
				GetModuleHandle( nullptr ),
				LoadIcon( nullptr, IDI_APPLICATION ),
				LoadCursor( nullptr, IDC_ARROW ),
				nullptr,
				nullptr,
				" ",
				LoadIcon( nullptr, IDI_APPLICATION )
			};

			if ( !RegisterClassEx( &this->wc_ ) )
			{
				throw std::runtime_error( "Failed to register window class." );
			}

			this->hwnd_ = CreateWindow( this->wc_.lpszClassName, this->wc_.lpszMenuName, WS_POPUP,
										GetSystemMetrics( SM_CXSCREEN ) / 2 - static_cast< int >( this->window_size_.x ) / 2,
										GetSystemMetrics( SM_CYSCREEN ) / 2 - static_cast< int >( this->window_size_.y ) / 2,
										this->window_size_.x, this->window_size_.y, nullptr, nullptr, this->wc_.hInstance, nullptr );

			SetWindowRgn( this->hwnd_, CreateRoundRectRgn( 0, 0, static_cast< int >( this->window_size_.x ), 
																 static_cast< int >( this->window_size_.y ), 20, 20 ), true );

			if ( !this->create_device( ) )
			{
				throw std::runtime_error( "Failed to create window." );
			}

			ShowWindow( this->hwnd_, SW_SHOWDEFAULT );
			UpdateWindow( this->hwnd_ );
		}

		/**
		* @brief Destroys the create_window object and cleans up Windows and DirectX resources.
		*
		* @details This destructor handles the cleanup of window and class resources.
		* It ensures that the window is destroyed and the window class is unregistered to prevent resource leaks.
		*/
		~create_window( )
		{
			if ( this->hwnd_ )
			{
				DestroyWindow( this->hwnd_ );
			}

			UnregisterClass( this->wc_.lpszClassName, this->wc_.hInstance );
		}

	public:
		/**
		* @brief Runs the render loop for the ImGui window.
		*
		* Initializes ImGui with DirectX 11 and a Win32 window, applies provided style, and
		* continuously renders frames until a quit message is received.
		*
		* @param style A function to apply custom styling to the DirectX device using ImGui.
		* @param render A function to execute the rendering logic, returning false will terminate the loop.
		*
		* @return Returns false when the application quits.
		*
		* @details Initializes ImGui contexts and ties them to DirectX and Win32 interfaces. The render loop handles
		* message dispatching and frame rendering, using provided lambdas or functions for style and render operations.
		* The loop exits when the render function returns false or if a WM_QUIT message is received.
		*/
		bool run( const std::function<void( ID3D11Device * )> &, const std::function<bool( HWND, const ImVec2, ID3D11Device * )> & ) const;

		/**
		* @brief Releases DirectX resources and cleans up.
		*
		* Ensures that all COM objects are properly released and set to nullptr to prevent memory leaks.
		*/
		void detach( );

	};
}