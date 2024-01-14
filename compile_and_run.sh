#!/bin/bash

clang++ -o try main.cpp menu.cpp multiplayer.cpp leaderboard.cpp connectfour.cpp singleplayer.cpp -lGL -lGLU -lglut -lsqlite3
./connect4
