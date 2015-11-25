#pragma once

void start();

DWORD WINAPI start_smp(void *param);

void stop();

void exit();

void pause_thread(std::string input);

void resume_thread(std::string input);