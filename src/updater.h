#ifndef AUTOMATIC_UPDATER_HEAEDERA
#define AUTOMATIC_UPDATER_HEAEDERA
#include <string>

class Updater
{
    public:
        Updater();
	~Updater();
        bool Run(const std::string& path);
        void Work();
        static const char * shell;
        static const char * marioRoot;
        FILE *p;
        enum { cvs, build, reporting, done, error } status;
	bool restart; // tells the user if the program can be restarted to experience updates
	bool Done() {return status == done || status == error;}
	bool Restart(){return restart;};
};

#endif

