/*
 * ----------------------------------------------- Memorytracker -----------------------------------------------
 * This one-header file can be used to determine memoryleaks. Instead of overriding the 'new' and 'delete'
 * operator, this tracker implements alternative functions. Usage:
 *
 * 1. Add this headerfile to your project
 *
 * 2. Instead of using 'new', use the MT_NEW() macro. Eg. creating a new 'Test' class:
 *       Test* t1 = MT_NEW (Test (arg1, arg2));
 *    Note, that brackets must be used around the object to be created
 *
 * 3. Instead of using 'delete', use the MT_DELETE() macro, for example:
 *       MT_DELETE (t1);
 *    Note, that brackets must be used around the object to be deleted
 *
 * 4. Uncomment the line #define ENABLED_MEMORY_TRACKER if you want to track memory.
 *    Comment it, if you don't want to track memory (no additional overhead)
 *
 * 5. Run the application and check the file MemoryTracker.txt
 */

#ifndef _MEM_TRACKER_H_
#define _MEM_TRACKER_H_

// Comment next line to enable memory tracking
// Uncomment next line to disable memory tracking
#define ENABLED_MEMORY_TRACKER

#ifdef ENABLED_MEMORY_TRACKER
    #include <iostream>
    #include <fstream>
    #include <map>
    #include <string>

	using namespace std;

	// This struct contains the data involved with memory allocation
	struct MemoryStruct
	{
		string typeName;
		string file;
		int line;
		string functionAlloc;
		size_t allocatedBytes;
    };

    /* This singleton class tracks the memory allocations by storing the relevant data in a map.
	* When the MemoryTracker object object is deleted, it checks which entries in the map are not deleted.
	* Any existing entry in the map represents a memory leak.
	*/
    class MemoryTracker
	{
        public:
            // Constructor
            MemoryTracker(void)
            {
                // Create the output file
                mTrackerFile.open("MemoryTracker.txt");
                mTrackerFile << "Initialize MemoryTracker\n";
            }

            // Destructor
            virtual ~MemoryTracker(void)
            {
                // Validate the map; if there are still entries in the map, there are also memory leaks
                size_t numberOfLeaks = memoryStructMap.size();
                if (numberOfLeaks == 0)
                {
                    mTrackerFile << "No memory leaks\n";
                }
                else
                {
                    numberOfLeaks == 1 ?
                        (mTrackerFile << numberOfLeaks << " memory leak occurence detected\n") :
                        (mTrackerFile << numberOfLeaks << " memory leak occurences detected\n");
                    dumpTrackMemory();
                }

                mTrackerFile << "Shutdown MemoryTracker\n";
                mTrackerFile.close();
            }

            // Get the Singleton
            static MemoryTracker& getInstance()
            {
                static MemoryTracker instance;
                return instance;
            }

            /* Track allocated data.
            * When this function is called, an entry is added to the internal map
            */
            template<typename T>
            T* trackMemory(T* ptr, size_t size, char* file, int line, char* functionAlloc)
            {
                //mTrackerFile << "memory tracked: " << size << "bytes\n";
                MemoryStruct memoryData;
                memoryData.typeName.assign(typeid (T).name());
                memoryData.file.assign(file);
                memoryData.line = line;
                memoryData.functionAlloc.assign(functionAlloc);
                memoryData.allocatedBytes = size;
                memoryStructMap[ptr] = memoryData;
                return ptr;
            }

            /* Untrack allocated data.
            * When this function is called, the corresponding entry is removed from the internal map
            */
            void unTrackMemory(void* ptr)
            {
                if (memoryStructMap.size() == 0)
                    return;

                // Find the pointer in the map and erase the memory data entry
                //mTrackerFile << "untrackMemory\n";
                map<void*, MemoryStruct>::const_iterator it;
                it = memoryStructMap.find(ptr);
                if (it != memoryStructMap.end())
                {
                    //mTrackerFile << "memory untracked\n";
                    memoryStructMap.erase(it);
                }
            }

        protected:
            ofstream mTrackerFile;

            // This map is filled with an entry after each allocation
            map<void*, MemoryStruct> memoryStructMap;

            /* Dump the content of the memory data map
            */
            void dumpTrackMemory(void)
            {
                map<void*, MemoryStruct>::const_iterator it = memoryStructMap.begin();
                map<void*, MemoryStruct>::const_iterator itEnd = memoryStructMap.end();
                MemoryStruct memoryData;
                while (it != itEnd)
                {
                    memoryData = (*it).second;
                    mTrackerFile << "Leak in '"
                        << memoryData.functionAlloc
                        << "' when allocating '"
                        << memoryData.typeName
                        << "' on line "
                        << memoryData.line
                        << " in file "
                        << memoryData.file
                        << "\n";
                    ++it;
                }
            }

        private:
            // Don't Implement private constructor and = operator
            MemoryTracker(MemoryTracker const&);
            void operator=(MemoryTracker const&);
    };

    /** Template function for MT_NEW
	*/
	template<typename T>
    static T* MT_NEW(T* ptr, char* file, int line, char* function)
	{
        MemoryTracker::getInstance().trackMemory(ptr, sizeof(T), file, line, function);
		return ptr;
	}

    /** Template function for MT_DELETE
	*/
	template<typename T>
    static void MT_DELETE(T* ptr)
	{
        MemoryTracker::getInstance().unTrackMemory(ptr);
		free(ptr);
	}
	
    /** Redefine MT_NEW and MT_DELETE to the MT_NEW and MT_DELETE template functions
	*/
    #define MT_NEW(T) MT_NEW(new T, (char*)__FILE__, __LINE__, (char*)__FUNCTION__)
    #define MT_DELETE MT_DELETE

#else

    /** Redefine MT_NEW and MT_DELETE to standard new and delete
	*/
    #define MT_NEW(T) new T
    #define MT_DELETE(T) delete T

#endif

#endif
