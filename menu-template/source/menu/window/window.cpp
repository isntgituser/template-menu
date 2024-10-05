#include "stdafx.hpp"
#include "menu\menu.hpp"

bool menu::window::create_window::run( const std::function<void( ID3D11Device * )> &style, const std::function<bool( HWND, ImVec2, ID3D11Device * )> &render ) const
{
	IMGUI_CHECKVERSION( );
	ImGui::CreateContext( );

	ImGui_ImplWin32_Init( this->hwnd_ );
	ImGui_ImplDX11_Init( this->device_, this->device_ctx_ );

	style( this->device_ );

	MSG msg = {};
	std::memset( &msg, 0, sizeof msg );

	while ( msg.message != WM_QUIT )
	{
		if ( PeekMessage( &msg, nullptr, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
			continue;
		}

		ImGui_ImplDX11_NewFrame( );
		ImGui_ImplWin32_NewFrame( );

		ImGui::NewFrame( );
		{
			if ( !render( this->hwnd_, this->window_size_, this->device_ ) )
			{
				msg.message = WM_QUIT;
			}
		}
		ImGui::Render( );

		constexpr float clear_color_with_alpha[ 4 ] = { 0, 0, 0, 0 };
		this->device_ctx_->OMSetRenderTargets( 1, &this->render_t_, nullptr );
		this->device_ctx_->ClearRenderTargetView(  this->render_t_, clear_color_with_alpha );

		ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );

		if ( FAILED( this->swap_chain_->Present( 1, 0 ) ) ) { break; }
	}

	ImGui_ImplDX11_Shutdown( );
	ImGui_ImplWin32_Shutdown( );

	ImGui::DestroyContext( );
	return false;
}

void menu::window::create_window::detach( )
{
	const auto release_and_reset = [ ]( auto *&ptr )
	{
		if ( ptr )
		{
			ptr->Release( );
			ptr = nullptr;
		}
	};

	release_and_reset( this->render_t_ );
	release_and_reset( this->device_   );
	release_and_reset( this->device_ctx_ );
	release_and_reset( this->swap_chain_ );
}

LRESULT WINAPI menu::window::create_window::wnd_proc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
	if ( ImGui_ImplWin32_WndProcHandler( hwnd, msg, wparam, lparam ) )
		return true;

	switch ( msg )
	{
		case WM_SYSCOMMAND:
		{
			if ( ( wparam & 0xFFF0 ) == SC_KEYMENU )
			{
				return 0L;
			}
		}
		break;

		case WM_DESTROY:
			PostQuitMessage( 0 );
			return 0;

		default: ;
	}

	return DefWindowProc( hwnd, msg, wparam, lparam );
}

bool menu::window::create_window::create_device( )
{
	DXGI_SWAP_CHAIN_DESC swap_desc = {};
	std::memset( &swap_desc, 0, sizeof swap_desc );

	swap_desc.BufferCount			= 2;
	swap_desc.BufferDesc.Width		= 0;
	swap_desc.BufferDesc.Height		= 0;
	swap_desc.BufferDesc.Format		= DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_desc.BufferDesc.RefreshRate.Numerator	 = 60;
	swap_desc.BufferDesc.RefreshRate.Denominator = 1;
	swap_desc.Flags			= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swap_desc.BufferUsage	= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_desc.OutputWindow	= this->hwnd_;
	swap_desc.SampleDesc.Count	 = 1;
	swap_desc.SampleDesc.Quality = 0;
	swap_desc.Windowed	 = true;
	swap_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	constexpr std::uint32_t create_device_flags = { 0 };
	constexpr D3D_FEATURE_LEVEL feature_level_array[ 2 ] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

	D3D_FEATURE_LEVEL feature_level = {};

	if ( D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, create_device_flags, feature_level_array, 2, D3D11_SDK_VERSION, &swap_desc, &this->swap_chain_, &this->device_, &feature_level, &this->device_ctx_ ) == DXGI_ERROR_UNSUPPORTED )
	{
		if ( D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_WARP, nullptr, create_device_flags, feature_level_array, 2, D3D11_SDK_VERSION, &swap_desc, &this->swap_chain_, &this->device_, &feature_level, &this->device_ctx_ ) != S_OK )
		{
			return false;
		}
	}

	ID3D11Texture2D *back_buffer = {};

	if ( FAILED( this->swap_chain_->GetBuffer( 0, IID_PPV_ARGS( &back_buffer ) ) ) )
	{
		throw std::runtime_error( "Failed to get buffer from swap chain." );
	}

	if ( FAILED( this->device_->CreateRenderTargetView( back_buffer, nullptr, &this->render_t_ ) ) )
	{
		back_buffer->Release( );
		throw std::runtime_error( "Failed to create render target view." );
	}

	back_buffer->Release( );
	return true;
}