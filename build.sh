#!/bin/bash

g++ -o downloader -pthread main.cpp  -L/usr/local/lib/ -lboost_coroutine -lboost_chrono -lboost_thread -lboost_system
