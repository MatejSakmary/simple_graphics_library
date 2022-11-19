template<typename T>
vec<3, T>::vec() : x{T(0.0)}, y{T(0.0)}, z{T(0.0)} {}

template<typename T>
vec<3, T>::vec(const vec<3, T> & other) : x{other.x}, y {other.y}, z{other.z} {}

template<typename T>
vec<3, T>::vec(const T x, const T y, const T z) : x{x}, y{y}, z{z} {}

template<typename T>
auto vec<3, T>::to_string() const -> std::string
{
    std::stringstream s;
    s << "[ " << std::to_string(x) << " " << std::to_string(y) << " " << std::to_string(z) << " ]";
    return s.str();
}

template<typename T>
auto vec<3, T>::get_norm() const -> float
{
    return sqrt(x * x + y * y + z * z);
}

template<typename T>
auto vec<3, T>::normalize() const -> vec<3, T>
{
    auto norm = get_norm();
    return {x / norm, y / norm, z / norm};
}

template<typename T>
vec<3, T> vec<3, T>::operator - (const vec<3, T> & rhs) const
{
    return { x - rhs.x, y - rhs.y, z - rhs.z };
}

template<typename T>
vec<3, T> vec<3, T>::operator + (const vec<3, T> & rhs) const
{
    return { x + rhs.x, y + rhs.y, z + rhs.z};
}

template<typename T>
vec<3, T> vec<3, T>::operator * (const vec<3, T> & rhs) const
{
    return { x * rhs.x, y * rhs.y, z * rhs.z };
}

template<typename T>
vec<3, T> vec<3, T>::operator / (const vec<3, T> & rhs) const
{
    return { x / rhs.x, y / rhs.y, z / rhs.z };
}

template<typename T>
vec<3, T> vec<3, T>::operator * (float scalar) const
{
    return { x * scalar, y * scalar, z * scalar };
}

template<typename T>
vec<3, T> vec<3, T>::operator / (float scalar) const
{
    return { x / scalar, y / scalar, z / scalar };
}

template<typename T>
vec<4, T>::vec() : x{T(0.0)}, y{T(0.0)}, z{T(0.0)}, w{T(0.0)} {}

template<typename T>
vec<4, T>::vec(const vec<4, T> & other) : x{other.x}, y {other.y}, z{other.z}, w{other.w} {}

template<typename T>
vec<4, T>::vec(const T x, const T y, const T z, const T w) : x{x}, y{y}, z{z}, w{w} {}

template<typename T>
auto vec<4, T>::to_string() const -> std::string
{
    std::stringstream s;
    s << "[ " << std::to_string(x) << " " << std::to_string(y) << " " 
      << std::to_string(z) << " " << std::to_string(w) << " ]";
    return s.str();
}

template<typename T>
auto vec<4, T>::get_norm() const -> float
{
    return sqrt(x * x + y * y + z * z + w * w);
}

template<typename T>
auto vec<4, T>::normalize() const -> vec<4, T>
{
    auto norm = get_norm();
    return {x / norm, y / norm, z / norm, w / norm};
}

template<typename T>
vec<4, T> vec<4, T>::operator - (const vec<4, T> & rhs) const
{
    return { x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
}

template<typename T>
vec<4, T> vec<4, T>::operator + (const vec<4, T> & rhs) const
{
    return { x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
}

template<typename T>
vec<4, T> vec<4, T>::operator * (const vec<4, T> & rhs) const
{
    return { x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w };
}

template<typename T>
vec<4, T> vec<4, T>::operator / (const vec<4, T> & rhs) const
{
    return { x / rhs.x, y / rhs.y, z / rhs.z, w * rhs.w };
}

template<typename T>
vec<4, T> vec<4, T>::operator * (float scalar) const
{
    return { x * scalar, y * scalar, z * scalar, w * scalar };
}

template<typename T>
vec<4, T> vec<4, T>::operator / (float scalar) const
{
    return { x / scalar, y / scalar, z / scalar, w * scalar };
}

template<typename T>
auto cross(const vec<3, T> & first, const vec<3, T> & second) -> vec<3, T>
{
    return { first.y * second.z - first.z * second.y,
             first.z * second.x - first.x * second.z,
             first.x * second.y - first.y * second.x };
}

/// @brief reflect *vector* along *normal*
///              normal
///  reflected      x       vector
///                xxx
///       xxxxxx  xxxxx  xxxxxx
///       xxxx     xxx     xxxx
///       x xxx    xxx    xxx x
///       x  xxx   xxx   xxx  x
///           xxx  xxx  xxx
///            xxx xxx xxx
///             xxxxxxxxx
///              xxxxxxx
/// @param vector - outgoing direction from the point of reflection
/// @param normal - outgoing normal from the point of reflection
/// @return reflected vector outgoing from the point of reflection
template<typename T>
auto reflect(const vec<3, T> & vector, const vec<3, T> &normal) -> vec<3, T>
{
    auto normalized_normal = normal.normalize();
    return 2.0f * dot(normalized_normal, vector) * normalized_normal - vector;
}

template<typename T>
auto dot(const vec<3, T> & first, const vec<3, T> & second) -> float
{
    return {first.x * second.x + first.y * second.y + first.z * second.z};
}