#include "stdafx.hpp"
#include "utils\utils.hpp"

std::vector<std::uint8_t> utils::helper::download( const std::string &url )
{
	const auto res = Get( cpr::Url{ url } );

	if ( res.error || res.status_code != 200 )
	{
		throw std::runtime_error( "Failed to download the file." );
	}

	return { res.text.begin( ), res.text.end( ) };
}