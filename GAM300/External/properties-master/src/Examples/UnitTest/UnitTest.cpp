#include "../../Properties.h"
#include "../Examples.h"

//--------------------------------------------------------------------------------------------
// Examples
//--------------------------------------------------------------------------------------------
template< typename T_EXAMPLE >
void Tests( void )
{
    printf( "------------------------------------------------------------------------------\n" );
    printf( "Testing: %s\n", property::getTableByType<T_EXAMPLE>().m_pName );

    Test01<T_EXAMPLE>();
    Test02<T_EXAMPLE>();

    printf( "------------------------------------------------------------------------------\n" );
    printf(  "\n\n" );
}

//--------------------------------------------------------------------------------------------
// main program
//--------------------------------------------------------------------------------------------
#if defined(_MSC_VER)
    #include <filesystem>
#endif

int main()
{
    Tests<example4_custom_lists>();
    Tests<example3_custom_lists>();
    Tests<example2_custom_lists>();
    Tests<example1_custom_lists>();
    Tests<example0_custom_lists>();
    Tests<example10>();
    Tests<example9>();
    Tests<example8>();
    Tests<example7>();
    Tests<example6>();
    Tests<example5>();
    Tests<example4>();
    Tests<example3>();
    Tests<example2>();
    Tests<example1>();
    Tests<example0>();

#if defined(__GNUC__) || defined(__GNUG__)
    // let it finish without a pause, this is needed for CI
#else

    // Create the documentation file from the header for easy to parse reasons
    // Documentation.md is therefor a read-only file. 
    try 
    {
        std::filesystem::copy_file( "../../src/Examples/Examples.h", "../../docs/Documentation.md", std::filesystem::copy_options::overwrite_existing ); 
    }
    catch ( std::filesystem::filesystem_error& e ) 
    {
        std::cout << "Could not copy Documentation: " << e.what() << '\n';
    }

    // Pause
    std::cin.get();
#endif

    return 0;
}

