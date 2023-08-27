#ifndef _Exceptions_
#define _Exceptions_
#include <string>

struct InvalidStringException
{
	std::string message;

	InvalidStringException(std::string error);
};

struct FileFailureException
{
	std::string message;

	FileFailureException(std::string error);
};

struct NullDataException
{
	std::string message;

	NullDataException(std::string error);
};
#endif