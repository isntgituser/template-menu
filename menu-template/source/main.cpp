#include "stdafx.hpp"
#include "menu\menu.hpp"

int main( int argc, char *argv[ ] )
{
	try
	{
		const auto menu = std::make_unique<menu::window::create_window>( ImVec2( 300, 200 ) );

		if ( !menu->run( menu::render::style, menu::render::paint ) )
		{
			menu->detach( );
		}
	}
	catch ( const std::exception &err )
	{
		DEBUG_MSG( err.what( ) );
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}