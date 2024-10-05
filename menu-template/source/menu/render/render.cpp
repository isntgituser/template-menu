#include "stdafx.hpp"
#include "menu\menu.hpp"

void menu::render::style( ID3D11Device *device )
{
	auto &io = ImGui::GetIO( );

	io.IniFilename = nullptr;
	io.LogFilename = nullptr;

	constexpr auto base_size = 17.0f;
	constexpr auto icon_size = base_size * 2.0f / 3.0f;

	static constexpr ImWchar icons_ranges[ ] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
	ImFontConfig icons_config;

	icons_config.MergeMode  = true;
	icons_config.PixelSnapH = true;
	icons_config.GlyphMinAdvanceX = icon_size;

	if ( !textures::fonts::create_from_web( io, "open_sans", "http://fonts.gstatic.com/s/opensans/v13/IgZJs4-7SA1XX_edsoXWog.ttf", base_size ) ||
		 !textures::fonts::create_from_web( io, "fa6_solid", "https://raw.githubusercontent.com/FortAwesome/Font-Awesome/6.x/webfonts/fa-solid-900.ttf", icon_size, &icons_config, icons_ranges ) )
	{
		DEBUG_MSG( "one of the fonts were not loaded" );
	}

	auto &style = ImGui::GetStyle( );

	style.Colors[ ImGuiCol_Border ]		= ImColor( 22, 22, 22, 225 );
	style.Colors[ ImGuiCol_WindowBg ]	= ImColor( 13, 13, 13, 225 );

	style.WindowPadding		= { 0, 0 };
	style.WindowBorderSize	= 0.5f;
}

bool menu::render::paint( HWND hwnd, const ImVec2 size, ID3D11Device *device )
{
	static auto flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

	static auto window_state = true, is_dragging = false;
	static POINT offset = { 0, 0 };

	ImGui::SetNextWindowPos( { 0.f, 0.f }, ImGuiCond_Once );
	ImGui::SetNextWindowSize( size, ImGuiCond_Once );

	ImGui::Begin( "###main_panel", &window_state, flags );
	{
		const auto draw = ImGui::GetWindowDrawList( );

		if ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) )
		{
			RECT rect;
			POINT point;

			GetCursorPos( &point );
			GetWindowRect( hwnd, &rect );

			offset.x = point.x - rect.left;
			offset.y = point.y - rect.top;

			is_dragging = ( offset.y >= 0 && static_cast< float >( offset.y ) <= ( ImGui::GetTextLineHeight( ) + ImGui::GetStyle( ).FramePadding.y * 4 ) );
		}

		if ( is_dragging && ImGui::IsMouseDragging( ImGuiMouseButton_Left ) )
		{
			POINT point;
			GetCursorPos( &point );

			const auto x = point.x - offset.x;
			const auto y = point.y - offset.y;

			SetWindowPos( hwnd, nullptr, x, y, static_cast< int >( size.x ), static_cast< int >( size.y ), SWP_NOZORDER );
		}

		draw->AddText( { 15, 15 }, ImGui::GetColorU32( ImGuiCol_Text ), "Desktop Application" );
	}
	ImGui::End( );

	return window_state;
}