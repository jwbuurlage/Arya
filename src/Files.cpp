#include "Files.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include "common/Logger.h"

using std::ifstream;

namespace Arya
{
	template<> FileSystem* Singleton<FileSystem>::singleton = 0;

	FileSystem::FileSystem()
	{
		initApplicationPath();
	}

	FileSystem::~FileSystem()
	{
		unloadAllFiles();
	}

	//getApplicationPath
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>

	void FileSystem::initApplicationPath()
	{
		char ExePath[MAX_PATH] = {0};
		GetModuleFileName(0, ExePath, MAX_PATH);
		string Directory(ExePath);
		applicationPath = Directory.substr(0, Directory.find_last_of('\\') + 1);
		return;
	}
#elif __APPLE__
    #include <mach-o/dyld.h>
	void FileSystem::initApplicationPath()
	{
		char path[1024];
		uint32_t size = sizeof(path);
		if (_NSGetExecutablePath(path, &size) == 0){
			string Directory(path);
			applicationPath = Directory.substr(0, Directory.find_last_of('/') + 1);
		}else
			applicationPath = "./";
		return;
	}
#elif __linux__
	#include <unistd.h>
	void FileSystem::initApplicationPath()
	{
		char path[1024] = {0};
		if( readlink("/proc/self/exe", path, 1024) > 0 ){
			string Directory(path);
			applicationPath = Directory.substr(0, Directory.find_last_of('/') + 1);
		}else
			applicationPath = "./";
		return;
	}
#else
	void FileSystem::initApplicationPath()
	{
		applicationPath = "./";
		return;
	}
#endif

	File* FileSystem::getFile(string filename)
	{
        LOG_INFO("Loading file: " << filename);
		string formattedFilename(filename);
		//Note: only works for ascii strings:
		//Unix filenames are case-sensitive
		//transform(formattedFilename.begin(), formattedFilename.end(), formattedFilename.begin(), tolower);

		//TODO:
		//First convert filename to some default format
		//so that when the same file gets requested but
		//with a different syntax, it will only be loaded once
		//Example: "./textures/tex.tga" should be converted to "textures/tex.tga"

		fileIterator loadedFile = loadedFiles.find(formattedFilename);
		if( loadedFile != loadedFiles.end() ){
			loadedFile->second->refcount++;
			return loadedFile->second;
		}

		string path(applicationPath);
		path.append(formattedFilename);
		ifstream filestream;
		filestream.open( path.c_str(), std::ios::binary );
		if( filestream.is_open() == false ){
            LOG_WARNING("File: " << path << " not found!");
			return 0;
		}

		File* newFile = new File;

		//Get file length
		filestream.seekg(0, std::ios::end);
		newFile->size = (unsigned int)filestream.tellg();
		filestream.seekg(0, std::ios::beg);

		//allocate memory + 1 for terminating zero for text files
		newFile->data = new char[newFile->size+1];
		newFile->data[newFile->size] = 0;

		filestream.read(newFile->data, newFile->size);

		newFile->refcount = 1;

		//Add to loadedFiles
		loadedFiles.insert( _fileValueType(filename, newFile) );
		return newFile;
	}

	void FileSystem::releaseFile(File* file)
	{
		file->refcount--;
		if( file->refcount <= 0 ) unloadFile(file);
	}

	void FileSystem::unloadFile(File* file)
	{
		for( fileIterator fileIter = loadedFiles.begin(); fileIter != loadedFiles.end(); ++fileIter ){
			if( file == fileIter->second ){
				loadedFiles.erase(fileIter);
				if( file->data ) delete[] file->data;
				delete file;
				break;
			}
		}
	}

	void FileSystem::unloadAllFiles()
	{
		for( fileIterator file = loadedFiles.begin(); file != loadedFiles.end(); ++file ){
			if( file->second->data ) delete[] file->second->data;
			delete file->second;
		}
		loadedFiles.clear();
	}

}
