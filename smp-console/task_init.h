#pragma once

#include "task.h"

class Init : public Task {
	int main(int argc, std::string *argv[]) override;

	std::string **parse_command(std::string command);
};
