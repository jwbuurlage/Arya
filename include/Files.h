//Singleton
//Virtual file system
//Use this for all file access
//All paths are relative to the applications directory
//It will make sure all file data will be followed by at least one 0 in memory so text files are 0-terminated.

//TODO: Functionality to iterate through (virtual) directory tree

#pragma once
#include "common/Singleton.h"
#include <string>
#include <map>

using std::string;
using std::map;

namespace Arya
{
	class File{
	public:
		char* getData(){ return data; }
		unsigned int getSize(){ return size; }
	private:
		char* data;
		unsigned int size;
		int refcount;
		friend class FileSystem;
	};

	class FileSystem : public Singleton<FileSystem>
	{
	public:
		FileSystem();
		~FileSystem();

		//With / appended
		string getApplicationPath(){ return applicationPath; }

		//Will open and load the file if not already opened
		//and close the handle to the file
		//Returns pointer to file in memory or 0 on error
		//Adds a reference to File
		//When the caller is done it should call unloadFile
		File* getFile(string filename);

		//Releases the file. When the reference count is zero
		//the file is removed from memory
		void releaseFile(File* file);

		//Will unload the file from memory
		//!! even if the files still have reference counts
		void unloadFile(File* file);
		void unloadAllFiles();

	private:
		string applicationPath;
		void initApplicationPath();

		//TODO: Have some virtual directory-tree like structure to be able to
		//iterate through all files in a directory.
		map<string,File*> loadedFiles;
		typedef map<string,File*>::iterator fileIterator;
		typedef map<string,File*>::value_type _fileValueType;
	};

}
