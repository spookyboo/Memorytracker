# C++ Memory Tracker

This one-header file can be used to determine memoryleaks. Instead of overriding the 'new' and 'delete'
operator, this tracker implements alternative functions. Usage:

1. Add this headerfile to your project.

2. Instead of using 'new', use the MT_NEW() macro. Eg. creating a new 'Test' class:  
   __Test* t1 = MT_NEW (Test (arg1, arg2));__  
   Note, that brackets must be used around the object to be created.

3. Instead of using 'delete', use the MT_DELETE() macro, for example:  
   __MT_DELETE (t1);__  
   Note, that brackets must be used around the object to be deleted.

4. Uncomment the line __#define ENABLED_MEMORY_TRACKER__ if you want to track memory.  
   Comment it, if you don't want to track memory (no additional overhead). 

5. Run the application and check the file _MemoryTracker.txt_.