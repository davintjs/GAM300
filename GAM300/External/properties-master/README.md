<img src="https://i.imgur.com/GfJb3sQ.jpg" align="right" width="150px" />


# Properties (Version: 0.4 Alpha)

[      ![pipeline status](https://gitlab.com/LIONant/properties/badges/master/pipeline.svg)](https://gitlab.com/LIONant/properties/commits/master)
[            ![Docs](https://img.shields.io/badge/docs-ready-brightgreen.svg)](https://gitlab.com/LIONant/properties/blob/master/docs/Documentation.md)
<br>
[          ![Clang C++17](https://img.shields.io/badge/clang%20C%2B%2B17-compatible-brightgreen.svg)]()
[            ![GCC C++17](https://img.shields.io/badge/gcc%20C%2B%2B17-compatible-brightgreen.svg)]()
[   ![Visual Studio 2019](https://img.shields.io/badge/Visual%20Studio%202019-compatible-brightgreen.svg)]()
<br>
[            ![Platforms](https://img.shields.io/badge/Platforms-All%20Supported-blue.svg)]()
<br>
[             ![Feedback](https://img.shields.io/badge/feedback-welcome-brightgreen.svg)](https://gitlab.com/LIONant/properties/issues)
[![Contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg)](https://gitlab.com/LIONant/properties)
[              ![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)

Simple yet powerful reflection system / property system. This project is meant for students and professionals alike to have 
a default goto code that solves the very annoying issue of C++ properties. The code should be able to be inserted in game engines 
and applications without a problem. If you wish please read the
[Documentation](https://gitlab.com/LIONant/properties/blob/master/docs/Documentation.md). 

Any feedback is welcome. Please follow us and help support the project.

<img src="https://i.imgur.com/9a5d2ee.png" align="right" width="150px" />

[              ![Twitter](https://img.shields.io/twitter/follow/nickreal03.svg?label=Follow&style=social)](https://twitter.com/nickreal03)
[                 ![chat](https://img.shields.io/discord/552344404258586644.svg?logo=discord)](https://discord.gg/fqaFSRE)

[<img src="https://i.imgur.com/4g2tHbP.png" width="170px" />](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=QPCQL53F8N73J&source=url)

[![Paypal](https://img.shields.io/badge/PayPal-Donate-blue.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=QPCQL53F8N73J)
[        ![SubscriveStar](https://img.shields.io/badge/SubscriveStar-Donate-blue.svg)](https://www.subscribestar.com/LIONant)
[              ![Patreon](https://img.shields.io/badge/Patreon-Donate-blue.svg)](https://www.patreon.com/LIONant)


# Code example:

```cpp
struct test1 : property::base
{
    int             m_Int       { 0 };
    float           m_Float     { 0 };
    bool            m_Bool      { 0 };
    std::string     m_String    {};
    oobb            m_OOBB      { 0 };

    property_vtable()           // Allows the base class to get these properties  
};

property_begin( test1 )
{
      property_var( m_Int     )
    , property_var( m_Float   )
    , property_var( m_String  )
    , property_var( m_Bool    )
    , property_var( m_OOBB    )
} property_vend_h( test1 )
```

# Editor example:
![ImGUI Example](https://i.imgur.com/KEuB81P.png "ImGUI Example" )

# Features
* **MIT license**
* **C++17**
* **Single header**
* **No external dependencies**
* **Super Fast properties with `property::pack`**
* **Super Friendly properties with strings which allows dealing with versions**
* **Minimalistic API**
* **Data and Function base properties**
* **Supports custom lists and understands `std::array` and `std::vector` for lists (more when requested)**
* **Very easy to extend (Easy to add new types, and add extra data per type, or new type of lists)**
* **Supports base class interface for OOP**
* **Handle any kind of inheritance**
* **Op-In properties, ( as requested by epyoncf and SeanMiddleditch from Reddit )**
* **No allocations**
* **No exceptions for maximun speed**
* **Works on all major compilers**
* **Test + Examples for all the features and [Documentation](https://gitlab.com/LIONant/properties/blob/master/docs/Documentation.md)**
* **Editor example using [ImGui](https://github.com/ocornut/imgui) with *undo/redo***
* **Everything is constexpr for all compilers (VS2017 can't [Bug Submitted to Microsoft](https://developercommunity.visualstudio.com/content/problem/463683/constant-expression-fails-while-gcc-and-clang-is-o.html?childToView=469517#comment-469517) )**
* **Unlike other property systems, no extra/external compilation needed.** 

# TODO
* Conversion between `std::vector<property::entry>` to `property::pack`
* Try to reduce the memory that a pack takes
