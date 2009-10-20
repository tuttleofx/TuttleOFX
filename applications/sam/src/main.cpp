/*
 * main.cpp
 *
 *  Created on: 07 oct. 2009
 *      Author: Nicolas Rondaud
 */

#include <tuttle/hostTuttle.hpp>
#include <iostream>

using namespace std;

int main(int argc, char * argv[]) {
	try{
		cout << "Sam command line tool" << endl;

		tuttle::HostTuttle tuttle;

	}
	catch(std::exception & e) {
		cerr << "main: " << e.what() << endl;
		return 1;
	}
	catch(...) {
		cerr << "main: Unknown error." << endl;
		return 1;
	}
	return 0;
}
