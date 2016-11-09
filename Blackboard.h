#pragma once

#include <typeinfo>
#include <unordered_map>
#include <mutex>
#include <assert.h>
#include <stdexcept>

namespace Utilities {
    //! Forward declare the base type of the data storage object
    namespace Templates { class BaseMap; }

    //! Define alias' for the different types of event callbacks that can be defined
    template<typename T> using EventKeyCallback = void(*)(const std::string&);
    template<typename T> using EventValueCallback = void(*)(const T&);
    template<typename T> using EventKeyValueCallback = void(*)(const std::string&, const T&);

    /*
     *      Name: Blackboard 
     *      Author: Mitchell Croft
     *      Created: 08/11/2016
     *      Modified: 09/11/2016
     *      
     *      Purpose:
     *      Provide a singleton location for a user to store
     *      generic data in a easily accessible location. 
     *      
     *      Callback functionality will be implemented to allow 
     *      listening for when specific keyed data is changed. 
     *      
     *      The create and destroy functions must be called 
     *      prior to use.
     *      
     *      Warning:
     *      Data types stored on the blackboard must have valid
     *      default and copy constructors defined as well as the
     *      assignment operator.
     *      
     *      Only one callback event of each type will be kept for 
     *      each key of every value type. 
    **/
    class Blackboard {
        /*----------Singleton Values----------*/
        static Blackboard* mInstance;
        Blackboard() = default;
        ~Blackboard() = default;

        /*----------Variables----------*/

        //! Store a map of all of the different value types
        std::unordered_map<size_t, Templates::BaseMap*> mDataStorage;

        //! Store a mutex for locking data when in use
        std::recursive_mutex mDataLock;

        //! Convert a template type into a unique ID value
        template<typename T> inline size_t templateToID() const;

        //! Ensure that a ValueMap objects exists for a specific type
        template<typename T> inline size_t supportType();

    public:
        //! Creation/destruction
        /*----------------*/ static bool create();
        /*----------------*/ static void destroy();

        //! Data reading/writing
        template<typename T> static void write(const std::string& pKey, const T& pValue, bool pRaiseCallbacks = true);
        template<typename T> static const T& read(const std::string& pKey);
        template<typename T> static void wipeTypeKey(const std::string& pKey);
        /*----------------*/ static void wipeKey(const std::string& pKey);
        /*----------------*/ static void wipeBoard(bool pWipeCallbacks = false);

        //! Callback functions
        template<typename T> static void subscribe(const std::string& pKey, EventKeyCallback<T> pCb);
        template<typename T> static void subscribe(const std::string& pKey, EventValueCallback<T> pCb);
        template<typename T> static void subscribe(const std::string& pKey, EventKeyValueCallback<T> pCb);
        template<typename T> static void unsubscribe(const std::string& pKey);
        /*----------------*/ static void unsubscribeAll(const std::string& pKey);

        //! Getters
        /*----------------*/ static inline bool isReady() { return (mInstance != nullptr); }
    };

    namespace Templates {
        /*
         *      Name: BaseMap
         *      Author: Mitchell Croft
         *      Created: 08/11/2016
         *      Modified: 08/11/2016
         *      
         *      Purpose:
         *      Provide a base point for the templated ValueMap
         *      objects to inherit from. This allows the 
         *      blackboard to store pointers to the templated 
         *      versions for storing data.
        **/
        class BaseMap { 
        protected:
            //! Set the Value map to be a friend of the blackboard to allow for construction/destruction of the object
            friend class Blackboard;

            //! Privatise the constructor/destructor to prevent external use
            BaseMap() = default; 
            virtual ~BaseMap() = 0; 

            //! Provide virtual methods for wiping keyed information
            inline virtual void wipeKey(const std::string& pKey) = 0;
            inline virtual void wipeAll() = 0;
            inline virtual void unsubscribe(const std::string& pKey) = 0;
            inline virtual void clearAllEvents() = 0;
        };

        //! Define the default destructor for the BaseMap's pure virtual destructor
        inline BaseMap::~BaseMap() {}

        /*
         *      Name: ValueMap (General)
         *      Author: Mitchell Croft
         *      Created: 08/11/2016
         *      Modified: 08/11/2016
         *      
         *      Purpose:
         *      Store templated data type information for recollection
         *      and use within the Blackboard singleton object
        **/
        template<typename T>
        class ValueMap : BaseMap {
        protected:
            //! Set the Value map to be a friend of the blackboard to allow for construction/destruction of the object
            friend class Blackboard;

            /*----------Variables----------*/

            //! Store a map of the values for this type
            std::unordered_map<std::string, T> mValues;

            //! Store maps for the callback events 
            std::unordered_map<std::string, EventKeyCallback<T>> mKeyEvents;
            std::unordered_map<std::string, EventValueCallback<T>> mValueEvents;
            std::unordered_map<std::string, EventKeyValueCallback<T>> mPairEvents;

            /*----------Functions----------*/

            //! Privatise the constructor/destructor to prevent external use
            ValueMap() = default;
            ~ValueMap() override {}

            //! Override the functions used to remove keyed information
            inline void wipeKey(const std::string& pKey) override;
            inline void wipeAll() override;
            inline void unsubscribe(const std::string& pKey) override;
            inline void clearAllEvents() override;
        };
    }

    #pragma region Template Definitions
    #pragma region Blackboard
    /*
        Blackboard : templateToID<T> - Convert the template type T to a unique hash code
        Author: Mitchell Croft
        Created: 08/11/2016
        Modified: 08/11/2016

        template T - A generic, non void type 

        return size_t - Returns the ID as a size_t value
    */
    template<typename T>
    inline size_t Utilities::Blackboard::templateToID() const {
        //Get the type info
        const std::type_info& type = typeid(T);

        //Return the hash code
        return type.hash_code();
    }

    /*
        Blackboard : supportType<T> - Using the type of the template ensure that there is a Value map to support 
                                   holding data of its type
        Author: Mitchell Croft
        Created: 08/11/2016
        Modified: 08/11/2016

        template T - A generic, non void type

        return size_t - Returns the unique hash code for the template type T
    */
    template<typename T>
    inline size_t Utilities::Blackboard::supportType() {
        //Get the hash code for the type
        size_t key = templateToID<T>();

        //If there isn't a entry for the hash code create a new map
        if (mDataStorage.find(key) == mDataStorage.end())
            mDataStorage[key] = new Utilities::Templates::ValueMap<T>();

        //Return the key
        return key;
    }
    
    /*
        Blackboard : write<T> - Write a data value to the Blackboard 
        Author: Mitchell Croft
        Created: 08/11/2016
        Modified: 08/11/2016

        template T - A generic, non void type

        param[in] pKey - The key value to save the data value at
        param[in] pValue - The data value to be saved to the key location
        param[in] pRaiseCallbacks - A flag to indicate if callback events should be raised (Default true)
    */
    template<typename T>
    inline void Utilities::Blackboard::write(const std::string& pKey, const T& pValue, bool pRaiseCallbacks) {
        //Ensure that the singleton has been created
        assert(mInstance);

        //Lock the data
        std::lock_guard<std::recursive_mutex> guard(mInstance->mDataLock);

        //Ensure the key for this type is supported
        size_t key = mInstance->supportType<T>();

        //Cast the Value Map to the type of T
        Utilities::Templates::ValueMap<T>* map = (Utilities::Templates::ValueMap<T>*)(mInstance->mDataStorage[key]);

        //Copy the data value across
        map->mValues[pKey] = pValue;

        //Check event flag
        if (pRaiseCallbacks) {
            //Check for events to raise
            if (map->mKeyEvents.find(pKey) != map->mKeyEvents.end() && map->mKeyEvents[pKey]) map->mKeyEvents[pKey](pKey);
            if (map->mValueEvents.find(pKey) != map->mValueEvents.end() && map->mValueEvents[pKey]) map->mValueEvents[pKey](map->mValues[pKey]);
            if (map->mPairEvents.find(pKey) != map->mPairEvents.end() && map->mPairEvents[pKey]) map->mPairEvents[pKey](pKey, map->mValues[pKey]);
        }
    }

    /*
        Blackboard : read<T> - Read the value of a key value from the Blackboard
        Author: Mitchell Croft
        Created: 08/11/2016
        Modified: 08/11/2016

        template T - A generic, non void type

        param[in] pKey - The key value to read the data value of

        return const T& - Returns a constant reference to the data type of type T
    */
    template<typename T>
    inline const T& Utilities::Blackboard::read(const std::string& pKey) {
        //Ensure that the singleton has been created
        assert(mInstance);

        //Lock the data
        std::lock_guard<std::recursive_mutex> guard(mInstance->mDataLock);

        //Ensure the key for this type is supported
        size_t key = mInstance->supportType<T>();

        //Cast the Value Map to the type of T
        Utilities::Templates::ValueMap<T>* map = (Utilities::Templates::ValueMap<T>*)(mInstance->mDataStorage[key]);

        //Return the value at the key location
        return map->mValues[pKey];
    }

    /*
        Blackboard : wipeTypeKey - Wipe the value stored at a specific key for the specified type
        Author: Mitchell Croft
        Created: 09/11/2016
        Modified: 09/11/2016

        param[in] pKey - A string object containing the key of the value(s) to remove
    */
    template<typename T>
    inline void Utilities::Blackboard::wipeTypeKey(const std::string& pKey) {
        //Ensure that the singleton has been created
        assert(mInstance);

        //Lock the data
        std::lock_guard<std::recursive_mutex> guard(mInstance->mDataLock);

        //Ensure the key for this type is supported
        size_t key = mInstance->supportType<T>();

        //Cast the Value Map to the type of T
        Utilities::Templates::ValueMap<T>* map = (Utilities::Templates::ValueMap<T>*)(mInstance->mDataStorage[key]);

        //Wipe the key from the value map
        map->wipeKey(pKey);
    }

    /*
        Blackboard : subscribe<T> - Set the callback event for a specific key value on a type of data
        Author: Mitchell Croft
        Created: 08/11/2016
        Modified: 08/11/2016

        template T - A generic, non void type

        param[in] pKey - The key to assign the callback event to
        param[in] pCb - A function pointer that takes in a constant string reference as its only parameter
    */
    template<typename T>
    inline void Utilities::Blackboard::subscribe(const std::string& pKey, EventKeyCallback<T> pCb) {
        //Ensure that the singleton has been created
        assert(mInstance);

        //Lock the data
        std::lock_guard<std::recursive_mutex> guard(mInstance->mDataLock);

        //Ensure the key for this type is supported
        size_t key = mInstance->supportType<T>();

        //Cast the Value Map to the type of T
        Utilities::Templates::ValueMap<T>* map = (Utilities::Templates::ValueMap<T>*)(mInstance->mDataStorage[key]);

        //Set the event callback
        map->mKeyEvents[pKey] = pCb;
    }

    /*
        Blackboard : subscribe<T> - Set the callback event for a specific key value on a type of data
        Author: Mitchell Croft
        Created: 08/11/2016
        Modified: 08/11/2016

        template T - A generic, non void type

        param[in] pKey - The key to assign the callback event to
        param[in] pCb - A function pointer that takes in a constant reference to the new value that was assigned
                        as its only parameters
    */
    template<typename T>
    inline void Utilities::Blackboard::subscribe(const std::string& pKey, EventValueCallback<T> pCb) {
        //Ensure that the singleton has been created
        assert(mInstance);

        //Lock the data
        std::lock_guard<std::recursive_mutex> guard(mInstance->mDataLock);

        //Ensure the key for this type is supported
        size_t key = mInstance->supportType<T>();

        //Cast the Value Map to the type of T
        Utilities::Templates::ValueMap<T>* map = (Utilities::Templates::ValueMap<T>*)(mInstance->mDataStorage[key]);

        //Set the event callback
        map->mValueEvents[pKey] = pCb;
    }

    /*
        Blackboard : subscribe<T> - Set the callback event for a specific key value on a type of data
        Author: Mitchell Croft
        Created: 08/11/2016
        Modified: 08/11/2016

        template T - A generic, non void type 

        param[in] pKey - The key to assign the callback event to
        param[in] pCb - A function pointer that takes in a constant string reference and a constant reference to
                        the new value as its only parameters
    */
    template<typename T>
    inline void Utilities::Blackboard::subscribe(const std::string& pKey, EventKeyValueCallback<T> pCb) {
        //Ensure that the singleton has been created
        assert(mInstance);

        //Lock the data
        std::lock_guard<std::recursive_mutex> guard(mInstance->mDataLock);

        //Ensure the key for this type is supported
        size_t key = mInstance->supportType<T>();

        //Cast the Value Map to the type of T
        Utilities::Templates::ValueMap<T>* map = (Utilities::Templates::ValueMap<T>*)(mInstance->mDataStorage[key]);

        //Set the event callback
        map->mPairEvents[pKey] = pCb;
    }

    /*
        Blackboard : unsubscribe - Unsubscribe all events associated with a key value
                                   for a specific type
        Author: Mitchell Croft
        Created: 08/11/2016
        Modified: 08/11/2016

        param[in] pKey - The key to remove the callback events from
    */
    template<typename T>
    inline void Utilities::Blackboard::unsubscribe(const std::string& pKey) {
        //Ensure that the singleton has been created
        assert(mInstance);

        //Lock the data
        std::lock_guard<std::recursive_mutex> guard(mInstance->mDataLock);

        //Ensure the key for this type is supported
        size_t key = mInstance->supportType<T>();

        //Cast the Value Map to the type of T
        Utilities::Templates::ValueMap<T>* map = (Utilities::Templates::ValueMap<T>*)(mInstance->mDataStorage[key]);

        //Pass the unsubscribe key to the Value Map
        map->unsubscribe(pKey);
    }
    #pragma endregion

    #pragma region ValueMap
    /*
        ValueMap<T> : wipeKey - Clear the value associated with a key value
        Author: Mitchell Croft
        Created: 08/11/2016
        Modified: 08/11/2016

        template T - A generic, non void type

        param[in] pKey - The key value to clear the entry of
    */
    template<typename T>
    inline void Utilities::Templates::ValueMap<T>::wipeKey(const std::string& pKey) { mValues.erase(pKey); }

    /*
        ValueMap<T> : wipeAll - Erase all data stored in the map
        Author: Mitchell Croft
        Created: 08/11/2016
        Modified: 08/11/2016

        template T - A generic, non void type
    */
    template<typename T>
    inline void Utilities::Templates::ValueMap<T>::wipeAll() { mValues.clear(); }

    /*
        ValueMap<T> : unsubscribe - Remove all callback events associated with a key value
        Author: Mitchell Croft
        Created: 08/11/2016
        Modified: 08/11/2016

        template T - A generic, non void type

        param[in] pKey - The key to wipe all callback events associated with
    */
    template<typename T>
    inline void Utilities::Templates::ValueMap<T>::unsubscribe(const std::string& pKey) {
        //Find iterators
        auto key = mKeyEvents.find(pKey);
        auto val = mValueEvents.find(pKey);
        auto pair = mPairEvents.find(pKey);

        //Remove the callbacks
        if (key != mKeyEvents.end()) mKeyEvents.erase(key);
        if (val != mValueEvents.end()) mValueEvents.erase(val);
        if (pair != mPairEvents.end()) mPairEvents.erase(pair);
    }

    /*
        ValueMap<T> : clearAllEvents - Clear all event callbacks stored within the Value map
        Author: Mitchell Croft
        Created: 08/11/2016
        Modified: 08/11/2016

        template T - A generic, non void type
    */
    template<typename T>
    inline void Utilities::Templates::ValueMap<T>::clearAllEvents() {
        //Clear all event maps
        mKeyEvents.clear();
        mValueEvents.clear();
        mPairEvents.clear();
    }
    #pragma endregion
    #pragma endregion
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                                                      ////
/////                                                      Object Definition                                               ////
/////                                                                                                                      ////
/////  Include a define for _BLACKBOARD_ in a single .cpp file inside of the project to use the Blackboard functionality   ////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _BLACKBOARD_
//! Define the Blackboards static singleton instance
Utilities::Blackboard* Utilities::Blackboard::mInstance = nullptr;

/*
    Blackboard : create - Initialise the Blackboard singleton for use
    Author: Mitchell Croft
    Created: 08/11/2016
    Modified: 08/11/2016

    return bool - Returns true if the Blackboard was initialised successfully
*/
bool Utilities::Blackboard::create() {
    //Check if the Blackboard has already been set up
    if (isReady()) destroy();

    //Create the instance
    mInstance = new Blackboard();

    //Return success state
    return (mInstance != nullptr);
}

/*
    Blackboard : destroy - Deallocate all data associated with the Blackboard and destroy
                           the singleton instance
    Author: Mitchell Croft
    Created: 08/11/2016
    Modified: 08/11/2016
*/
void Utilities::Blackboard::destroy() {
    //Check that there is an instance to destroy
    if (mInstance) {
        //Lock the data values
        mInstance->mDataLock.lock();

        //Delete all Value Map values
        for (auto pair : mInstance->mDataStorage)
            delete pair.second;

        //Unlock the data
        mInstance->mDataLock.unlock();

        //Delete the singleton 
        delete mInstance;

        //Reset the instance pointer
        mInstance = nullptr;
    }
}

/*
    Blackboard : wipeKey - Clear all data associated with the passed in key value
    Author: Mitchell Croft
    Created: 08/11/2016
    Modified: 08/11/2016

    param[in] pKey - A string object containing the key of the value(s) to remove
*/
void Utilities::Blackboard::wipeKey(const std::string& pKey) {
    //Ensure that the singleton has been created
    assert(mInstance);

    //Lock the data
    std::lock_guard<std::recursive_mutex> guard(mInstance->mDataLock);

    //Loop through the different type collections
    for (auto pair : mInstance->mDataStorage)
        pair.second->wipeKey(pKey);
}

/*
    Blackboard : wipeBoard - Clear all data stored on the Blackboard
    Author: Mitchell Croft
    Created: 08/11/2016
    Modified: 08/11/2016

    param[in] pWipeCallbacks - Flags if all of the set event callbacks should be cleared
                               as well as the values (Default false)
*/
void Utilities::Blackboard::wipeBoard(bool pWipeCallbacks) {
    //Ensure that the singleton has been created
    assert(mInstance);

    //Lock the data
    std::lock_guard<std::recursive_mutex> guard(mInstance->mDataLock);

    //Loop through all stored Value maps
    for (auto pair : mInstance->mDataStorage) {
        //Clear the data values
        pair.second->wipeAll();

        //Clear the callbacks
        if (pWipeCallbacks) pair.second->clearAllEvents();
    }
}

/*
    Blackboard : unsubscribeAll - Remove the associated callback events for a key
                                  from every type map 
    Author: Mitchell Croft
    Created: 08/11/2016
    Modified: 08/11/2016

    param[in] pKey - The key to remove the callback events from
*/
void Utilities::Blackboard::unsubscribeAll(const std::string& pKey) {
    //Ensure that the singleton has been created
    assert(mInstance);

    //Lock the data
    std::lock_guard<std::recursive_mutex> guard(mInstance->mDataLock);

    //Loop through all stored Value maps
    for (auto pair : mInstance->mDataStorage)
        pair.second->unsubscribe(pKey);
}
#endif  //_BLACKBOARD_