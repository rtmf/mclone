#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "oglconsole.h"
#include "updater.h"
using namespace std;

// Yeah I have really, really not decided anything about these semantics yet,
// plus i'm really high right now, so i'll do it later
#ifdef MINGW_OR_SOMETHING
const char * Updater::shell = "h:/msys/1.0/bin/sh -c ";
const char * Updater::marioRoot = "h:/msys/1.0/home/junk/mario";
#else
const char * Updater::shell = "/bin/sh -c ";
const char * Updater::marioRoot = ".";
#endif

bool Updater::Run(const string& command)
{
    string cmd(shell);
    cmd += "'cd ";
    cmd += marioRoot;
    cmd += "; ";
    cmd += command;
    cmd += "'";
    OGLCONSOLE_Print("!%s\n", cmd.c_str());
    p = popen(cmd.c_str(), "r");
    if (p) return true;
    OGLCONSOLE_Print("The Updater class could not run the following command: \"%s\"", cmd.c_str());
    return false;
}

Updater::Updater()
{
    status = cvs;
    restart = false;
    if (!Run("cvs up")) status = error;
}

void Updater::Work()
{
    char c; int r;

    if (status == done) return;
    if (feof(p))
    {
        pclose(p);
        switch (status)
        {
            case cvs:
                OGLCONSOLE_Print("Finished downloading updates from CVS\n");

                OGLCONSOLE_Print("Backing up current and previous program binaries\n");
                // TODO: non.exe support
                system("cp mario.backup2.exe mario.backup3.exe");
                system("cp mario.backup1.exe mario.backup2.exe");
                system("cp mario.exe mario.backup1.exe");

                OGLCONSOLE_Print("Building newly downloaded updates\n");
                // Run make
                if (!Run("make")) status = error;
                status = build;
                break;

            case build:
                OGLCONSOLE_Print("Finished building updated code\n");
                status = done;

                OGLCONSOLE_Print("Checking to see if the program binary has been updated...");
                if (system("test -nt mario.exe mario.backup1.exe") == 0)
                {
                    OGLCONSOLE_Print("Yes, mario.exe has been updated\n");
                    restart = true;
                }
                else
                {
                    OGLCONSOLE_Print("No, mario.exe has not been updated\n");
                    restart = false;
                }

                break;

            case reporting:
                OGLCONSOLE_Print("Reporting not yet supported\n");
                status = done;

            default:
            case done:
                p = NULL;
                restart = true; // TODO: implement a real test using make or something
                return; // so that we don't read
        }
    }

    // LoL, read one byte at a time
    // TODO: i have the r variable so i can check for values other than just one or non-one, stupid
    r = fread(&c, 1, 1, p);
    if (r == -1)
    {
            OGLCONSOLE_Print("\n\n### Error while fread()ing\n");
            status = error;
    }
    else if (r == 1) OGLCONSOLE_Print("%c", c);
}

Updater::~Updater()
{
	if (p) pclose(p);
}

