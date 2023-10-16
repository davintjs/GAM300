#ifndef PROPERTY_CONFIG_H
#define PROPERTY_CONFIG_H

#include <glm/glm.hpp>
#include <imgui.h>

using string_t = std::string;

//--------------------------------------------------------------------------------------------
// Some structure to show that you can add your own atomic structures
//--------------------------------------------------------------------------------------------
//struct oobb
//{
//    float m_Min{}, m_Max{};
//};

class Vector2
{
public:

    float x, y;

    //Constructors
    Vector2() : x(0.f), y(0.f) {}
    Vector2(float x_, float y_) : x(x_), y(y_) {}
    Vector2(float i) : x(i), y(i) {}

    Vector2(Vector2& vec) : x(vec.x), y(vec.y) {}
    Vector2(glm::vec2 vec) : x(vec.x), y(vec.y) {}

    //Operator Overloads
    Vector2& operator=(const Vector2& Vec2)
    {
        x = Vec2.x;
        y = Vec2.y;
        return *this;
    }

    Vector2& operator=(const glm::vec2& glmVec2)
    {
        x = glmVec2.x;
        y = glmVec2.y;
        return *this;
    }

    float& operator[](int id)
    {

        //E_ASSERT((id < 0 || id > 2),  "Vector2 Subscript operator out of range!");

        if (id == 0) return x;
        if (id == 1) return y;
    }

    // Conversion operator from Vector2 to glm::vec2
    operator glm::vec2() const
    {
        return glm::vec2(x, y);
    }

};

class Vector3 {
public:

    float x, y, z;

    //Constructors
    Vector3() : x(0.f), y(0.f), z(0.f) {}
    Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    Vector3(float i) : x(i), y(i), z(i) {}

    Vector3(Vector3& vec) : x(vec.x), y(vec.y), z(vec.z) {}
    Vector3(glm::vec3 vec) : x(vec.x), y(vec.y), z(vec.z) {}

    //Operator Overloads
    Vector3& operator=(const Vector3& Vec3) {
        x = Vec3.x;
        y = Vec3.y;
        z = Vec3.z;
        return *this;
    }

    Vector3& operator=(const glm::vec3& glmVec3) {
        x = glmVec3.x;
        y = glmVec3.y;
        z = glmVec3.z;
        return *this;
    }

    float& operator[](int id) {

        E_ASSERT((id >= 0 && id <= 2),  "Vector3 Subscript operator out of range!");

        if (id == 0) return x;
        if (id == 1) return y;
        if (id == 2) return z;
    }

    bool operator != (const Vector3& other) const {
        return x != other.x || y != other.y || z != other.z;
    }

    // Conversion operator from Vector3 to glm::vec3
    operator glm::vec3() const {
        return glm::vec3(x, y, z);
    }

};



class Vector4 {
public:
    
    float w, x, y, z;

    //Constructors
    Vector4() : x(0.f), y(0.f), z(0.f), w(0.f) {}
    Vector4(float w_, float x_, float y_, float z_) : w(w_), x(x_), y(y_), z(z_) {}
    Vector4(float i) : w(i), x(i), y(i), z(i) {}

    Vector4(Vector4& vec) : w(vec.w), x(vec.x), y(vec.y), z(vec.z) {}
    Vector4(glm::vec4 vec) : w(vec.w), x(vec.x), y(vec.y), z(vec.z) {}

    //Operator Overloads
    Vector4& operator=(const Vector4& Vec4) {
        w = Vec4.w;
        x = Vec4.x;
        y = Vec4.y;
        z = Vec4.z;
        return *this;
    }

    Vector4& operator=(const glm::vec4& glmVec4) {
        w = glmVec4.w;
        x = glmVec4.x;
        y = glmVec4.y;
        z = glmVec4.z;
        return *this;
    }

    Vector4& operator=(const ImVec4& imvec4) {
        w = imvec4.w;
        x = imvec4.x;
        y = imvec4.y;
        z = imvec4.z;
        return *this;
    }

    float& operator[](int id) {

        E_ASSERT((id < 0 || id > 3),  "Vector4 Subscript operator out of range!");

        if (id == 0) return w;
        if (id == 1) return x;
        if (id == 2) return y;
        if (id == 3) return z;
    }

    // Conversion operator from Vector3 to glm::vec3
    operator glm::vec4() const {
        return glm::vec4(w, x, y, z);
    }

};


//--------------------------------------------------------------------------------------------
// User settings
//--------------------------------------------------------------------------------------------
namespace property
{
    //--------------------------------------------------------------------------------------------
    // These section provides a basic frame work to for an editor
    //--------------------------------------------------------------------------------------------
    namespace editor
    {
        // Specifies custom renders for each type
        struct style_info_base
        {
            void* m_pDrawFn{ nullptr };
        };

        // Settings for each type when rendering
        template< typename T >
        struct style_info : style_info_base {};

        // Custom settings for rendering ints
        template<>
        struct style_info<int> : style_info_base
        {
            int                     m_Min;
            int                     m_Max;
            const char*             m_pFormat;
            float                   m_Speed;
        };

        // Custom settings for rendering floats
        template<>
        struct style_info<float> : style_info_base
        {
            float                   m_Min;
            float                   m_Max;
            const char*             m_pFormat;
            float                   m_Speed;
            float                   m_Power;
        };

        template<>
        struct style_info<string_t> : style_info_base
        {
            enum class style
            {
                  NORMAL
                , ENUM 
            };

            const std::pair<const char*, int>*  m_pEnumList;
            std::size_t                         m_EnumCount;
            style                               m_Style;
        };
    }
    //-----------------------------------------------------------------------------------

    template< typename T >
    struct edstyle
    {
        constexpr static editor::style_info<T> Default(void) noexcept;
    };

    //-----------------------------------------------------------------------------------

    template<>
    struct edstyle<int>
    {    
        constexpr static editor::style_info<int> ScrollBar(int Min, int Max, const char* pFormat = "%d")                                                                                             noexcept;
        constexpr static editor::style_info<int> Drag     (float Speed = 1.0f, int Min = std::numeric_limits<int>::lowest(), int Max = std::numeric_limits<int>::max(), const char* pFormat = "%d")  noexcept;
        constexpr static editor::style_info<int> Default  (void) noexcept { return Drag(); }
    };

    //-----------------------------------------------------------------------------------

    template<>
    struct edstyle<float>
    {
        constexpr static editor::style_info<float> ScrollBar(float Min, float Max, const char* pFormat = "%.3f", float Power = 1.0f)                                                                                                 noexcept;
        constexpr static editor::style_info<float> Drag     (float Speed = 1.0f, float Min = std::numeric_limits<float>::lowest(), float Max = std::numeric_limits<float>::max(), const char* pFormat = "%.3f", float Power = 1.0f)  noexcept;
        constexpr static editor::style_info<float> Default  (void) noexcept { return Drag(); }
    };

    //-----------------------------------------------------------------------------------

    template<>
    struct edstyle<string_t>
    {
        template< std::size_t N >
        constexpr static editor::style_info<string_t> Enumeration(const std::array<std::pair<const char*, int>, N>& Array)  noexcept;
        constexpr static editor::style_info<string_t> Button     (void)                                                     noexcept;
        constexpr static editor::style_info<string_t> Default    (void)                                                     noexcept;
    };

    //-----------------------------------------------------------------------------------
    // If we are not including an editor lets give the system some empty functions
    //-----------------------------------------------------------------------------------
#ifndef PROPERTY_EDITOR
    template< typename T >
    constexpr editor::style_info<T>             edstyle<T>::            Default     (void)                                              noexcept { return {}; }
    constexpr editor::style_info<int>           edstyle<int>::          ScrollBar   (int, int, const char*)                             noexcept { return {}; }
    constexpr editor::style_info<int>           edstyle<int>::          Drag        (float, int, int, const char*)                      noexcept { return {}; }
    constexpr editor::style_info<float>         edstyle<float>::        ScrollBar   (float, float, const char*, float)                  noexcept { return {}; }
    constexpr editor::style_info<float>         edstyle<float>::        Drag        (float, float, float, const char*, float)           noexcept { return {}; }
    template< std::size_t N >
    constexpr editor::style_info<string_t>      edstyle<string_t>::     Enumeration (const std::array<std::pair<const char*, int>, N>&) noexcept { return {}; }
    constexpr editor::style_info<string_t>      edstyle<string_t>::     Default     (void)                                              noexcept { return {}; }
    constexpr editor::style_info<string_t>      edstyle<string_t>::     Button      (void)                                              noexcept { return {}; }
#endif

    //--------------------------------------------------------------------------------------------
    // Settings for the property system
    //--------------------------------------------------------------------------------------------
    namespace settings
    {
        //--------------------------------------------------------------------------------------------
        // The properties which the property system is going to know about.
        //--------------------------------------------------------------------------------------------
        using data_variant = std::variant
            <
            int
            , bool
            , float
            , string_t
            , size_t
            , uint32_t
            , Vector3
            , Vector4
        >;
    
        //--------------------------------------------------------------------------------------------
        // Group all the editor::setting_info under a single varient. We must follow data_variant order
        // to stay type safe.
        //--------------------------------------------------------------------------------------------
        namespace editor
        {
            struct empty {};

            // Helper
            namespace details
            {
                template<typename... T>
                std::variant< property::editor::style_info<T> ... , empty> CreateEditorEditStyles( std::variant< T... > );
            }

            // Actual variant with all the different editing styles
            using styles_info_variant = decltype( details::CreateEditorEditStyles( std::declval<data_variant>() ) );
        }

        //--------------------------------------------------------------------------------------------
        // User define data for each property
        //--------------------------------------------------------------------------------------------
        struct user_entry
        {
            const char*                                                 m_pHelp            { nullptr };             // A simple string describing to the editor's user what this property does
            editor::styles_info_variant                                 m_EditStylesInfo   { editor::empty{} };     // If not style is set then the default will be used
            
            constexpr user_entry() = default;

            // Function for user to setup a help string for their properties
            template< typename T = property::setup_entry >                                  // We use a template here to ask the compiler to resolve this function later
            constexpr T Help ( const char* pHelp ) const noexcept                           // Thanks to that we can make this function a constexpr function
            { 
                T r = *static_cast<const T*>(this);                                         // Because we are a constexpr function we can not modify the class directly we must copy it
                r.m_pHelp = pHelp;                                                          // Now we can modify the variable that we care about
                return r;                                                                   // We return our new instance
            }

            // Setting the editor display style (Look in imGuiPropertyExample for mode details)
            template< typename T = property::setup_entry >                                  // We use a template here to ask the compiler to resolve this function later
            constexpr T EDStyle( const editor::styles_info_variant&& Style ) const noexcept // Thanks to that we can make this function a constexpr function
            {
                T r = *static_cast<const T*>(this);
                assert( r.m_FunctionTypeGetSet.index() == Style.index() );                  // Make sure that the property type is the same type
                r.m_EditStylesInfo = std::move(Style);                                      // Call using the constructor to make sure this function can stay constexpr
                return r;
            }
        };
    }
}


#endif // !PROPERTY_CONFIG_H