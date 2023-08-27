#include "Exceptions.h"
using namespace std;

InvalidStringException::InvalidStringException(string error)
{
	message = error;
}

FileFailureException::FileFailureException(string error)
{
	message = error;
}

NullDataException::NullDataException(string error)
{
	message = error;
}