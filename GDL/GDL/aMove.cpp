/**
 *  Game Develop
 *      Player
 *
 *  Par Florian "4ian" Rival
 *
 */
/**
 *
 *  Contient une ou plusieurs actions
 */

#include <vector>
#include <string>
#include "GDL/Object.h"
#include <cmath>
#include "GDL/aMove.h"
#include "GDL/cCollisions.h"
#include "GDL/Event.h"
#include <iostream>
#include <sstream>
#include "GDL/Chercher.h"
#include "GDL/algo.h"
#include "GDL/Force.h"
#include <iostream>
#include "GDL/Access.h"
#include "GDL/RuntimeScene.h"
#include "GDL/ObjectsConcerned.h"
#include "GDL/gpl.h"


////////////////////////////////////////////////////////////
/// Ecarte un objet d'un autre de fa�on absolu
///
/// Type : Rebondir
/// Param�tre 1 : Objet � faire rebondir
/// Param�tre 2 : Objet contre lequel il rebond ( ne bougera pas )
////////////////////////////////////////////////////////////
bool ActEcarter( RuntimeScene * scene, ObjectsConcerned & objectsConcerned, const Instruction & action, const Evaluateur & eval )
{
    ObjList list = objectsConcerned.Pick(action.GetParameter( 0 ).GetPlainString(), action.IsGlobal());
    ObjList list2 = objectsConcerned.Pick(action.GetParameter( 1 ).GetPlainString(), action.IsGlobal());

	ObjList::iterator obj = list.begin();
	ObjList::const_iterator obj_end = list.end();
    for ( ; obj != obj_end; ++obj )
    {
        ObjList::iterator obj2 = list2.begin();
        ObjList::const_iterator obj2_end = list2.end();
        for (; obj2 != obj2_end; ++obj2)
        {
            if ( *obj2 != *obj )
            {
                float Left1 = (*obj)->GetDrawableX();
                float Left2 = (*obj2)->GetDrawableX();
                float Right1 = (*obj)->GetDrawableX() + (*obj)->GetWidth();
                float Right2 = (*obj2)->GetDrawableX() + (*obj2)->GetWidth();
                float Top1 = (*obj)->GetDrawableY();
                float Top2 = (*obj2)->GetDrawableY();
                float Bottom1 = (*obj)->GetDrawableY() + (*obj)->GetHeight();
                float Bottom2 = (*obj2)->GetDrawableY() + (*obj2)->GetHeight();

                if (( Left1 < Left2 ) )
                {
                    (*obj)->SetX( Left2 - (*obj)->GetWidth() );
                }
                else if (( Right1 > Right2 ) )
                {
                    (*obj)->SetX( Right2 );
                }

                if (( Top1 < Top2 ) )
                {
                    (*obj)->SetY( Top2 - (*obj)->GetHeight() );
                }
                else if ( Bottom1 > Bottom2 )
                {
                    (*obj)->SetY( Bottom2 );
                }
            }
        }
    }

    return true;
}

////////////////////////////////////////////////////////////
/// Fait "rebondir" ( Plut�t ecarte ) un objet d'un autre
///
/// Type : Rebondir
/// Param�tre 1 : Objet � faire rebondir
/// Param�tre 2 : Objet contre lequel il rebond ( ne bougera pas )
////////////////////////////////////////////////////////////
bool ActRebondir( RuntimeScene * scene, ObjectsConcerned & objectsConcerned, const Instruction & action, const Evaluateur & eval )
{
    ObjList list = objectsConcerned.Pick(action.GetParameter( 0 ).GetPlainString(), action.IsGlobal());
    ObjList list2 = objectsConcerned.Pick(action.GetParameter( 1 ).GetPlainString(), action.IsGlobal());

	ObjList::iterator obj = list.begin();
	ObjList::const_iterator obj_end = list.end();

    for ( ; obj != obj_end; ++obj )
    {
        ObjList::iterator obj2 = list2.begin();
        ObjList::const_iterator obj2_end = list2.end();
        for ( ; obj2 != obj2_end; ++obj2)
        {
            if ( *obj2 != *obj )
            {
                float Xobj1 = (*obj)->GetDrawableX()+((*obj)->GetCenterX()) ;
                float Yobj1 = (*obj)->GetDrawableY()+((*obj)->GetCenterY()) ;
                float Xobj2 = (*obj2)->GetDrawableX()+((*obj2)->GetCenterX()) ;
                float Yobj2 = (*obj2)->GetDrawableY()+((*obj2)->GetCenterY()) ;

                if ( Xobj1 < Xobj2 )
                {
                    if ( (*obj)->Force5.GetX() == 0 )
                        (*obj)->Force5.SetX( -( (*obj)->TotalForceX() ) - 10 );
                }
                else
                {
                    if ( (*obj)->Force5.GetX() == 0 )
                        (*obj)->Force5.SetX( -( (*obj)->TotalForceX() ) + 10 );
                }

                if ( Yobj1 < Yobj2 )
                {
                    if ( (*obj)->Force5.GetY() == 0 )
                        (*obj)->Force5.SetY( -( (*obj)->TotalForceY() ) - 10 );
                }
                else
                {
                    if ( (*obj)->Force5.GetY() == 0 )
                        (*obj)->Force5.SetY( -( (*obj)->TotalForceY() ) + 10 );
                }
            }
        }
    }

    return true;
}

/**
 * Add a force to an object, with coordinates
 */
bool Object::ActAddForceXY( RuntimeScene * scene, ObjectsConcerned & objectsConcerned, const Instruction & action, const Evaluateur & eval )
{
    Force forceToAdd;
    forceToAdd.SetX( eval.EvalExp( action.GetParameter( 1 ), shared_from_this()  ) );
    forceToAdd.SetY( eval.EvalExp( action.GetParameter( 2 ), shared_from_this()  ) );
    forceToAdd.SetClearing( eval.EvalExp( action.GetParameter( 3 ), shared_from_this()  ) );
    Forces.push_back( forceToAdd );

    return true;
}

/**
 * Add a force to an object, with angle and length
 */
bool Object::ActAddForceAL( RuntimeScene * scene, ObjectsConcerned & objectsConcerned, const Instruction & action, const Evaluateur & eval )
{
    //Ajout de la force
    Force forceToAdd;
    forceToAdd.SetAngle( eval.EvalExp( action.GetParameter( 1 ), shared_from_this() ) );
    forceToAdd.SetLength( eval.EvalExp( action.GetParameter( 2 ), shared_from_this() ) );
    forceToAdd.SetClearing( eval.EvalExp( action.GetParameter( 3 ), shared_from_this() ) );
    Forces.push_back( forceToAdd );

    return true;
}


////////////////////////////////////////////////////////////
/// Ajoute une force � un objet pour qu'il se dirige vers un
/// autre.
///
/// Type : AddForceVers
/// Param�tre 1 : Objet auquelle cr�er la force
/// Param�tre 2 : Objet vers lequel se diriger
/// Param�tre 3 : longueur  de la force
/// Param�tre 4 : Diffusion de la force
////////////////////////////////////////////////////////////
bool ActAddForceVers( RuntimeScene * scene, ObjectsConcerned & objectsConcerned, const Instruction & action, const Evaluateur & eval )
{
    ObjList list = objectsConcerned.Pick(action.GetParameter( 0 ).GetPlainString(), action.IsGlobal());
    ObjList list2 = objectsConcerned.Pick(action.GetParameter( 1 ).GetPlainString(), action.IsGlobal());

    if ( list2.empty() ) return true; //Pas d'objet vers lequel se diriger
    boost::shared_ptr<Object> obj2 = list2[0];

	ObjList::iterator obj = list.begin();
	ObjList::const_iterator obj_end = list.end();
    for ( ; obj != obj_end; ++obj )
    {
        Force forceToAdd;
        forceToAdd.SetLength( eval.EvalExp( action.GetParameter( 2 ), *obj , list2[0] ) );
        forceToAdd.SetClearing( eval.EvalExp( action.GetParameter( 3 ), *obj, list2[0]  ) );
        forceToAdd.SetAngle( atan2(( list2[0]->GetDrawableY() + list2[0]->GetCenterY() ) - ( (*obj)->GetDrawableY() + (*obj)->GetCenterY() ),
                                 ( list2[0]->GetDrawableX() + list2[0]->GetCenterX() ) - ( (*obj)->GetDrawableX() + (*obj)->GetCenterX() ) )
                                 * 180 / 3.14 );

        (*obj)->Forces.push_back( forceToAdd );
    }

    return true;
}

/**
 * Add a force toward a position
 */
bool Object::ActAddForceVersPos( RuntimeScene * scene, ObjectsConcerned & objectsConcerned, const Instruction & action, const Evaluateur & eval )
{
    Force forceToAdd;
    forceToAdd.SetLength( eval.EvalExp( action.GetParameter( 3 ), shared_from_this()) );
    forceToAdd.SetClearing( eval.EvalExp( action.GetParameter( 4 ), shared_from_this() ) );
    forceToAdd.SetAngle( atan2(
                            eval.EvalExp(action.GetParameter( 2 ), shared_from_this() ) - (GetDrawableY()+GetCenterY()),
                            eval.EvalExp(action.GetParameter( 1 ), shared_from_this() ) - (GetDrawableX()+GetCenterX())
                            ) * 180 / 3.14 );

    Forces.push_back( forceToAdd );

    return true;
}

/**
 * Remove all forces of an object
 */
bool Object::ActArreter( RuntimeScene * scene, ObjectsConcerned & objectsConcerned, const Instruction & action, const Evaluateur & eval )
{
    Forces.clear();

    return true;
}

/**
 * Add a force to an object so as it turns around a position
 */
bool Object::ActAddForceTournePos( RuntimeScene * scene, ObjectsConcerned & objectsConcerned, const Instruction & action, const Evaluateur & eval )
{
    //Angle en degr� entre les deux objets
    float angle = atan2(( GetDrawableY() + GetCenterY()) - eval.EvalExp( action.GetParameter( 2 ), shared_from_this() ),
                        ( GetDrawableX() + GetCenterX() ) - eval.EvalExp( action.GetParameter( 1 ), shared_from_this() ))
                        * 180 / 3.14;
    float newangle = angle + eval.EvalExp( action.GetParameter( 3 ), shared_from_this());

    //position actuelle de l'objet 1 par rapport � l'objet centre
    int oldX = ( GetDrawableX() + GetCenterX() ) - eval.EvalExp( action.GetParameter( 1 ), shared_from_this() );
    int oldY = ( GetDrawableY() + GetCenterY() ) - eval.EvalExp( action.GetParameter( 2 ), shared_from_this() );

    //nouvelle position � atteindre
    int newX = cos(newangle/180.f*3.14f) * eval.EvalExp( action.GetParameter( 4 ), shared_from_this());
    int newY = sin(newangle/180.f*3.14f) * eval.EvalExp( action.GetParameter( 4 ), shared_from_this());

    Force forceToAdd;
    forceToAdd.SetX( newX-oldX );
    forceToAdd.SetY( newY-oldY );
    forceToAdd.SetClearing( eval.EvalExp( action.GetParameter( 5 ), shared_from_this()) );

    Forces.push_back( forceToAdd );

    return true;
}

////////////////////////////////////////////////////////////
/// Ajoute une force � un objet pour qu'il tourne autour d'un autre
///
/// Type : AddForceTourne
/// Param�tre 1 : Objet auquelle cr�er la force
/// Param�tre 2 : Objet autour duquel tourner
/// Param�tre 3 : Degr�s par secondes
/// Param�tre 4 : Distance
/// Param�tre 5 : Diffusion de la force
////////////////////////////////////////////////////////////
bool ActAddForceTourne( RuntimeScene * scene, ObjectsConcerned & objectsConcerned, const Instruction & action, const Evaluateur & eval )
{
    ObjList list = objectsConcerned.Pick(action.GetParameter( 0 ).GetPlainString(), action.IsGlobal());
    ObjList list2 = objectsConcerned.Pick(action.GetParameter( 1 ).GetPlainString(), action.IsGlobal());

    if ( list2.empty() ) return true; //Pas d'objet vers lequel se diriger
    boost::shared_ptr<Object> obj2 = list2[0];

	ObjList::iterator obj = list.begin();
	ObjList::const_iterator obj_end = list.end();
    for ( ; obj != obj_end; ++obj )
    {
        //Angle en degr� entre les deux objets
        float angle = atan2(( (*obj)->GetDrawableY() + (*obj)->GetCenterY()) - ( list2[0]->GetDrawableY() + list2[0]->GetCenterY() ),
                            ( (*obj)->GetDrawableX() + (*obj)->GetCenterX() ) - ( list2[0]->GetDrawableX() + list2[0]->GetCenterX() ) )
                             * 180 / 3.14;
        float newangle = angle + eval.EvalExp( action.GetParameter( 2 ), *obj, list2[0]);

        //position actuelle de l'objet 1 par rapport � l'objet centre
        int oldX = ( (*obj)->GetDrawableX() + (*obj)->GetCenterX() ) - ( list2[0]->GetDrawableX() + list2[0]->GetCenterX() );
        int oldY = ( (*obj)->GetDrawableY() + (*obj)->GetCenterY()) - ( list2[0]->GetDrawableY() + list2[0]->GetCenterY());

        //nouvelle position � atteindre
        int newX = cos(newangle/180.f*3.14f) * eval.EvalExp( action.GetParameter( 3 ), *obj, list2[0]  );
        int newY = sin(newangle/180.f*3.14f) * eval.EvalExp( action.GetParameter( 3 ), *obj, list2[0]  );

        Force forceToAdd;
        forceToAdd.SetX( newX-oldX );
        forceToAdd.SetY( newY-oldY );
        forceToAdd.SetClearing( eval.EvalExp( action.GetParameter( 4 ), *obj, list2[0] ) );

        (*obj)->Forces.push_back( forceToAdd );
    }

    return true;
}