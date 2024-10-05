#include "stdafx.hpp"
#include "menu\menu.hpp"

std::unordered_map<std::string, ID3D11ShaderResourceView *> image_cache = {};
std::unordered_map<std::string, ImFont *> font_cache = {};

ID3D11ShaderResourceView *textures::image::get( const std::string &name )
{
    if ( const auto it = image_cache.find( name ); it != image_cache.end( ) )
        return it->second;

    return nullptr;
}

bool textures::image::create_from_web( ID3D11Device *device, const std::string &name, const std::string &url )
{
    const auto data = utils::helper::download( url );
    ID3D11ShaderResourceView *image = nullptr;

    if ( FAILED( D3DX11CreateShaderResourceViewFromMemory( device, data.data( ), data.size( ), nullptr, nullptr, &image, nullptr ) ) )
        return false;
    
    image_cache.emplace( name, image );
    return true;
}

bool textures::image::create_from_png( ID3D11Device *device, const std::string &name, const std::string &path )
{
    ID3D11ShaderResourceView *image = nullptr;

    if ( FAILED( D3DX11CreateShaderResourceViewFromFile( device, path.data( ), nullptr, nullptr,  &image, nullptr ) ) )
        return false;

    image_cache.emplace( name, image );
    return true;
}

ImFont *textures::fonts::get( const std::string &name )
{
    if ( const auto it = font_cache.find( name ); it != font_cache.end( ) )
        return it->second;

    return nullptr;
}

bool textures::fonts::create_from_web( const ImGuiIO &io, const std::string &name, const std::string &url,
                                       const float size, const ImFontConfig *cfg, const ImWchar *glyph_ranges )
{
    const auto data = utils::helper::download( url );

    auto mem_block = std::make_unique<char[ ]>( data.size( ) );
    std::ranges::copy( data, mem_block.get( ) );

    const auto font = io.Fonts->AddFontFromMemoryTTF( mem_block.release( ), static_cast<int>( data.size( ) ), size, cfg, glyph_ranges );

    font_cache.emplace( name, font );
    return true;
}

bool textures::fonts::create_from_ttf( const ImGuiIO &io, const std::string &name, const std::string &path,
                                       const float size, const ImFontConfig *cfg, const ImWchar *glyph_ranges )
{
    const auto font = io.Fonts->AddFontFromFileTTF( path.data( ), size, cfg, glyph_ranges );

    font_cache.emplace( name, font );
    return true;
}