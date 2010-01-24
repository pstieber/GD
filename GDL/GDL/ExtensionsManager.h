#ifndef EXTENSIONSMANAGER_H
#define EXTENSIONSMANAGER_H
#include <string>
#include <iostream>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/bimap/bimap.hpp>

#include "GDL/ExtensionBase.h"
class Object;

#ifdef GDE
class Game;
class MainEditorCommand;
#endif

using namespace std;

namespace gdp
{

/**
 * ExtensionsManager manages extension, and provide useful things like :
 *
 * -Functions for creating an object ( from another or from a type ).
 * -Functions for getting pointers to actions/conditions functions.
 * -Convert typeId to type name and vice-versa.
 */
class GD_API ExtensionsManager
{
    public:

        /**
         * Add an extension to the manager
         */
        bool AddExtension(boost::shared_ptr<ExtensionBase> extension);

        /**
         * Return true if an extension with the sam name is loaded
         */
        bool IsExtensionLoaded(string name) const;

        /**
         * Get an extension
         * @return Shared pointer to the extension
         */
        boost::shared_ptr<ExtensionBase> GetExtension(string name) const;

        /**
         * Get all extensions
         * @return Vector of Shared pointer containing all extensions
         */
        inline const vector < boost::shared_ptr<ExtensionBase> > & GetExtensions() const { return extensionsLoaded; };

        /**
         * Verifying if a ( static ) condition exists
         * @return true if the ( static ) condition exists
         */
        bool HasCondition(string name) const;

        /**
         * Get a pointer to a ( static ) condition
         * @return Pointer to the condition, NULL if this latter does not exist.
         */
        InstructionFunPtr GetConditionFunctionPtr(string name) const;

        /**
         * Verifying if a ( object ) action exists
         * @return true if the ( object ) action exists
         */
        bool HasObjectCondition(unsigned int objectTypeId, string name) const;

        /**
         * Get a pointer to a ( object ) action
         * @return Pointer to the action, NULL if this latter does not exist.
         */
        InstructionObjectFunPtr GetObjectConditionFunctionPtr(unsigned int objectTypeId, string name) const;

        /**
         * Verifying if a ( static ) action exists
         * @return true if the ( static ) action exists
         */
        bool HasAction(string name) const;

        /**
         * Get information about an action from its type
         * Works for object and static actions.
         */
        const InstructionInfos & GetActionInfos(string actionType) const;

        /**
         * Get information about a condition from its type
         * Works for object and static conditions.
         */
        const InstructionInfos & GetConditionInfos(string conditionType) const;

        /**
         * Get information about an expression from its type
         * Works for object and static expressions.
         */
        const ExpressionInfos & GetExpressionInfos(string exprType, bool onlyStaticExpressions = false) const;

        /**
         * Get a pointer to a ( static ) action
         * @return Pointer to the action, NULL if this latter does not exist.
         */
        InstructionFunPtr GetActionFunctionPtr(string name) const;

        /**
         * Verifying if a ( object ) action exists
         * @return true if the ( object ) action exists
         */
        bool HasObjectAction(unsigned int objectTypeId, string name) const;

        /**
         * Get a pointer to a ( object ) action
         * @return Pointer to the action, NULL if this latter does not exist.
         */
        InstructionObjectFunPtr GetObjectActionFunctionPtr(unsigned int objectTypeId, string name) const;

        /**
         * Verifying if a ( static ) expression exists
         * @return true if the ( static ) expression exists
         */
        bool HasExpression(string name) const;

        /**
         * Get a pointer to a ( static ) expression
         * @return Pointer to the expression, NULL if this latter does not exist.
         */
        ExpressionFunPtr GetExpressionFunctionPtr(string name) const;

        /**
         * Verifying if a ( object ) expression exists
         * @return true if the ( object ) expression exists
         */
        bool HasObjectExpression(unsigned int objectTypeId, string name) const;

        /**
         * Get a pointer to a ( object ) expression
         * @return Pointer to the expression, NULL if this latter does not exist.
         */
        ExpressionObjectFunPtr GetObjectExpressionFunctionPtr(unsigned int objectTypeId, string name) const;

        /**
         * Return a shared_ptr to a new object.
         */
        virtual boost::shared_ptr<Object> CreateObject(unsigned int typeId, std::string name);

        /**
         * Return a shared_ptr to a new object, created by copying the source.
         */
        virtual boost::shared_ptr<Object> CreateObject(boost::shared_ptr<Object> src);

        /**
         * Get the typeId associated with a name
         * @return typeId ( 0 if not found )
         */
        inline unsigned int GetTypeIdFromString(std::string name)
        {
            if ( extObjectNameToTypeId.left.find(name) != extObjectNameToTypeId.left.end())
                return extObjectNameToTypeId.left.at(name);

            return 0;
        }

        /**
         * Get the name associated with a typeId
         * @return name ( "" if not found )
         */
        inline std::string GetStringFromTypeId(unsigned int typeId)
        {
            if ( extObjectNameToTypeId.right.find(typeId) != extObjectNameToTypeId.right.end())
                return extObjectNameToTypeId.right.at(typeId);

            return "";
        }

        static ExtensionsManager *getInstance()
        {
            if ( NULL == _singleton )
            {
                _singleton = new ExtensionsManager;
            }

            return ( static_cast<ExtensionsManager*>( _singleton ) );
        }

        static void kill()
        {
            if ( NULL != _singleton )
            {
                delete _singleton;
                _singleton = NULL;
            }
        }

    private:
        ExtensionsManager();
        virtual ~ExtensionsManager() {};

        vector < boost::shared_ptr<ExtensionBase> > extensionsLoaded;
        vector < CreateFunPtr >             creationFunctionTable;
        vector < CreateByCopyFunPtr >       creationByCopyFunctionTable;
        vector < DestroyFunPtr >            destroyFunctionTable;

        typedef boost::bimaps::bimap < string, unsigned int > StringToTypeIdBiMap;
        StringToTypeIdBiMap                 extObjectNameToTypeId;

        static InstructionInfos badInstructionInfos;
        static ExpressionInfos badExpressionInfos;

        static ExtensionsManager *_singleton;
};



}
#endif // EXTENSIONSMANAGER_H