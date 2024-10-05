#pragma once

namespace utils::helper
{
	/**
	* @brief Downloads content from a specified URL and returns it as a vector of bytes.
	*
	* @param url The URL from which to download content.
	*
	* @return A `std::vector<std::uint8_t>` containing the raw bytes of the downloaded content.
	*
	* @throws std::runtime_error if the download fails due to a network error or if the response status is not 200.
	*
	* @details The function uses the cpr library to perform an HTTP GET request to the given URL.
	* If the request is successful (i.e., no HTTP error and a status code of 200), it converts the response text to a vector of bytes.
	* Otherwise, it throws an exception indicating a failure in downloading the file.
	*
	* @note Ensure that the cpr library is correctly configured in your build environment.
	*/
	std::vector<std::uint8_t> download( const std::string & );
}
