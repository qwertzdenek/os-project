#include "task_init.h"
#include "sched_calls.h"

int Init::main(int argc, std::string * argv[])
{
	// TODO
	return 0;
}

std::string **Init::parse_command(std::string command)
{
	// TODO: just skeleton
	std::list<const char *> argv;

	const char *start;

	for (auto ch = command.begin(); ch != command.end(); ++ch)
	{
		if (isspace(*ch)) {
			*ch = 0;
			continue;
		}

		start = ch._Ptr;
		while (!isspace(*++ch))
			;
		argv.push_back(start);
	}

	// return array of string pointers
	return NULL;
}
