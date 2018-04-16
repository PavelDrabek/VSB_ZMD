#include "stdafx.h"

int main( int argc, char * argv[] )
{
	const char * locale = setlocale( LC_ALL, "" );
	std::locale lollocale( locale );
	setlocale( LC_ALL, locale );
	setlocale( LC_NUMERIC, "C" );
	
	MBAR mbar( "..//..//data//test02.png" );
	mbar.start();

	return 0;
}
