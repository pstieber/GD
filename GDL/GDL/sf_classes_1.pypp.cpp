// This file has been generated by Py++.
#ifdef PYSUPPORT
#include "boost/python.hpp"
#include "c:/libs/sfml/include/sfml/system.hpp"
#include "c:/libs/sfml/include/sfml/window.hpp"
#include "c:/libs/sfml/include/sfml/window/contextsettings.hpp"
#include "c:/libs/sfml/include/sfml/graphics.hpp"
#include "c:/libs/sfml/include/sfml/audio.hpp"
#include "sf_classes_1.pypp.hpp"

namespace bp = boost::python;

void register_classes_1(){

    bp::class_< sf::Clock >( "Clock", bp::init< >() )
        .def(
            "GetElapsedTime"
            , (float ( ::sf::Clock::* )(  ) const)( &::sf::Clock::GetElapsedTime ) )
        .def(
            "Reset"
            , (void ( ::sf::Clock::* )(  ) )( &::sf::Clock::Reset ) );

    bp::class_< sf::Color >( "Color", bp::init< >() )
        .def( bp::init< sf::Uint8, sf::Uint8, sf::Uint8, bp::optional< sf::Uint8 > >(( bp::arg("red"), bp::arg("green"), bp::arg("blue"), bp::arg("alpha")=(unsigned char)(255) )) )
        .def_readonly( "Black", sf::Color::Black )
        .def_readonly( "Blue", sf::Color::Blue )
        .def_readonly( "Cyan", sf::Color::Cyan )
        .def_readonly( "Green", sf::Color::Green )
        .def_readonly( "Magenta", sf::Color::Magenta )
        .def_readonly( "Red", sf::Color::Red )
        .def_readonly( "White", sf::Color::White )
        .def_readonly( "Yellow", sf::Color::Yellow )
        .def_readwrite( "a", &sf::Color::a )
        .def_readwrite( "b", &sf::Color::b )
        .def_readwrite( "g", &sf::Color::g )
        .def_readwrite( "r", &sf::Color::r )
        .def( bp::self != bp::self )
        .def( bp::self * bp::self )
        .def( bp::self *= bp::self )
        .def( bp::self + bp::self )
        .def( bp::self += bp::self )
        .def( bp::self == bp::self );

    bp::class_< sf::NonCopyable, boost::noncopyable >( "NonCopyable", bp::no_init );

    bp::class_< sf::Context, boost::noncopyable >( "Context", bp::init< >() )
        .def(
            "SetActive"
            , (void ( ::sf::Context::* )( bool ) )( &::sf::Context::SetActive )
            , ( bp::arg("active") ) );

    bp::class_< sf::ContextSettings >( "ContextSettings", bp::init< bp::optional< unsigned int, unsigned int, unsigned int > >(( bp::arg("depth")=(unsigned int)(24), bp::arg("stencil")=(unsigned int)(8), bp::arg("antialiasing")=(unsigned int)(0) )) )
        .def_readwrite( "AntialiasingLevel", &sf::ContextSettings::AntialiasingLevel )
        .def_readwrite( "DepthBits", &sf::ContextSettings::DepthBits )
        .def_readwrite( "StencilBits", &sf::ContextSettings::StencilBits );

    bp::class_< sf::Vector2< float > >( "Vector2f", bp::init< >() )
        .def( bp::init< float, float >(( bp::arg("X"), bp::arg("Y") )) )
        .def_readwrite( "x", &sf::Vector2< float >::x )
        .def_readwrite( "y", &sf::Vector2< float >::y );

}
#endif