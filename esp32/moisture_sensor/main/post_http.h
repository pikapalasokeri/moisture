#ifndef POST_HTTP_
#define POST_HTTP_

#include <cstdint>
#include <string>

bool post_http(std::uint32_t const raw_value,
               int const sensor_id,
               std::string const location);

#endif // POST_HTTP_
