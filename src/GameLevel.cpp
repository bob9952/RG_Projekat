#pragma once
#include "GameLevel.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <GLFW/glfw3.h>

bool GameLevel::update() {
    this->Cubes.clear();
    float unitSize = 1.0f;

    for(unsigned int i = 0; i < levelSize; i++)
    {
        for(unsigned int j = 0; j < levelSize; j++)
        {
            /*
                9 for borders
                5 for folk
                4 for done boxes
                3 for normal boxes
                2 for empty spots
                1 for normal floor
                0 for free space
            */
            if(cubeData[i][j] == 9) { //zid
                glm::vec3 pos(i * unitSize, j * unitSize, 0.0f);
                glm::vec3 size(unitSize);
                GameObject obj(pos, size, ResourceManager::GetTexture("wall"), glm::vec3(1.0f));
                this->Cubes.push_back(obj);
                pos = glm::vec3(i * unitSize, j * unitSize, -unitSize);
                size = glm::vec3(unitSize);
                obj = GameObject(pos, size, ResourceManager::GetTexture("floor"), glm::vec3(1.0f));
                this->Cubes.push_back(obj);
            }else if(cubeData[i][j] == 5){ //cikica
                this->folkX = i;
                this->folkY = j;
                glm::vec3 pos(i * unitSize, j * unitSize, 0.0f);
                glm::vec3 size(unitSize);
                GameObject obj(pos, size, ResourceManager::GetTexture("folk"), glm::vec3(1.0f));
                this->Cubes.push_back(obj);
                if(goalSquareData[i][j] == 1){
                    pos = glm::vec3(i * unitSize, j * unitSize, -unitSize);
                    size = glm::vec3(unitSize);
                    obj = GameObject(pos, size, ResourceManager::GetTexture("target"), glm::vec3(1.0f));
                    this->Cubes.push_back(obj);
                }else{
                    pos = glm::vec3(i * unitSize, j * unitSize, -unitSize);
                    size = glm::vec3(unitSize);
                    obj = GameObject(pos, size, ResourceManager::GetTexture("floor"), glm::vec3(1.0f));
                    this->Cubes.push_back(obj);
                }
            }else if(cubeData[i][j] == 4){ //done box
                goalSquareData[i][j] = 1;
                glm::vec3 pos(i * unitSize, j * unitSize, 0.0f);
                glm::vec3 size(unitSize);
                GameObject obj(pos, size, ResourceManager::GetTexture("box"), glm::vec3(0.0f, 1.0f, 0.0f));
                this->Cubes.push_back(obj);

                pos = glm::vec3(i * unitSize, j * unitSize, -unitSize);
                size = glm::vec3(unitSize);
                obj = GameObject(pos, size, ResourceManager::GetTexture("target"), glm::vec3(1.0f));
                this->Cubes.push_back(obj);
            }else if(cubeData[i][j] == 3){ //obican box
                glm::vec3 pos(i * unitSize, j * unitSize, 0.0f);
                glm::vec3 size(unitSize);
                GameObject obj(pos, size, ResourceManager::GetTexture("box"), glm::vec3(1.0f));
                this->Cubes.push_back(obj);

                pos = glm::vec3(i * unitSize, j * unitSize, -unitSize);
                size = glm::vec3(unitSize);
                obj = GameObject(pos, size, ResourceManager::GetTexture("floor"), glm::vec3(1.0f));
                this->Cubes.push_back(obj);
            }else if(cubeData[i][j] == 2){ //ciljno polje
                goalSquareData[i][j] = 1;
                glm::vec3 pos(i * unitSize, j * unitSize, -unitSize);
                glm::vec3 size(unitSize);
                GameObject obj(pos, size, ResourceManager::GetTexture("target"), glm::vec3(1.0f, 0.0f, 0.0f));
                this->Cubes.push_back(obj);
            }else if(cubeData[i][j] == 1){ // pod
                glm::vec3 pos(i * unitSize, j * unitSize, -unitSize);
                glm::vec3 size(unitSize);
                GameObject obj(pos, size, ResourceManager::GetTexture("floor"), glm::vec3(1.0f));
                this->Cubes.push_back(obj);
            }
        }
    }
    return this->isCompleted();
}

void GameLevel::Load(const char *file) {
    this->Cubes.clear();

    // load from file
    unsigned int cubeCode;
    GameLevel level;
    std::string line;
    std::ifstream fstream(file);

    if(fstream)
    {
        fstream >> this->levelSize;

        std::getline(fstream, line);
        while(std::getline(fstream, line))
        {
            std::istringstream sstream(line);
            std::vector<unsigned int> row;
            while(sstream >> cubeCode)
                    row.push_back(cubeCode);

            cubeData.push_back(row);
        }

        if(cubeData.size() > 0)
            this->init();
    }else
    {
        //greska
    }
}

void GameLevel::Draw(SpriteRenderer &Renderer) {
    for(GameObject &cube : this->Cubes)
        cube.Draw(Renderer);
}

bool GameLevel::move(unsigned int direction) {
    int new_folk_y, new_folk_x;
    bool move_allowed = true;
    int directionX = 0;
    int directionY = 0;

    switch (direction) {
        case GLFW_KEY_UP:
            directionY = 1;
            break;
        case GLFW_KEY_DOWN:
            directionY = -1;
            break;
        case GLFW_KEY_LEFT:
            directionX = -1;
            break;
        case GLFW_KEY_RIGHT:
            directionX = 1;
            break;
    }

    new_folk_x = this->folkX + directionX;
    new_folk_y = this->folkY + directionY;
    if(cubeData[new_folk_x][new_folk_y] == 9)
        move_allowed = false;
    if(move_allowed && (cubeData[new_folk_x][new_folk_y] == 3 || cubeData[new_folk_x][new_folk_y] == 4))
    {
        int new_box_x = new_folk_x + directionX;
        int new_box_y = new_folk_y + directionY;
        if(cubeData[new_box_x][new_box_y] == 9)
            move_allowed = false;
        if(cubeData[new_box_x][new_box_y] == 3 || cubeData[new_box_x][new_box_y] == 4)
            move_allowed = false;
        if(move_allowed && cubeData[new_box_x][new_box_y]==2){
            cubeData[new_box_x][new_box_y] = 4;
        }
        if(move_allowed && cubeData[new_box_x][new_box_y]==1){
            cubeData[new_box_x][new_box_y] = 3;
        }
    }

    if(move_allowed){
        if(goalSquareData[folkX][folkY]==1) {
            cubeData[folkX][folkY] = 2;
        }else{
            cubeData[folkX][folkY] = 1;
        }
        cubeData[new_folk_x][new_folk_y] = 5;
        folkX = new_folk_x;
        folkY = new_folk_y;
    }
    return move_allowed;
}

bool GameLevel::isCompleted() {
    for(unsigned int i = 0; i < levelSize; i++)
    {
        for(unsigned int j = 0; j < levelSize; j++)
        {
            if(cubeData[i][j] == 2 || goalSquareData[folkX][folkY] == 1){
                return false;
            }
        }
    }

    return true;
}

void GameLevel::init()
{
    float unitSize = 1.0f;
    cubeData.resize(levelSize);
    for(unsigned int i = 0; i < levelSize; i++)
        cubeData[i].resize(levelSize);
    goalSquareData.resize(levelSize);
    for(unsigned int i = 0; i < levelSize; i++)
        goalSquareData[i].resize(levelSize);

    for(unsigned int i = 0; i < levelSize; i++)
    {
        for(unsigned int j = 0; j < levelSize; j++)
        {
            /*
                9 for borders
                5 for folk
                4 for done boxes
                3 for normal boxes
                2 for empty spots
                1 for normal floor
                0 for free space
            */
            if(cubeData[i][j] == 9) { //zid
                glm::vec3 pos(i * unitSize, j * unitSize, 0.0f);
                glm::vec3 size(unitSize);
                GameObject obj(pos, size, ResourceManager::GetTexture("wall"), glm::vec3(1.0f));
                this->Cubes.push_back(obj);
                pos = glm::vec3(i * unitSize, j * unitSize, -unitSize);
                size = glm::vec3(unitSize);
                obj = GameObject(pos, size, ResourceManager::GetTexture("floor"), glm::vec3(1.0f));
                this->Cubes.push_back(obj);
            }else if(cubeData[i][j] == 5){ //cikica
                this->folkX = i;
                this->folkY = j;
                glm::vec3 pos(i * unitSize, j * unitSize, 0.0f);
                glm::vec3 size(unitSize);
                GameObject obj(pos, size, ResourceManager::GetTexture("folk"), glm::vec3(1.0f));
                this->Cubes.push_back(obj);
                if(goalSquareData[i][j] == 1){
                    pos = glm::vec3(i * unitSize, j * unitSize, -unitSize);
                    size = glm::vec3(unitSize);
                    obj = GameObject(pos, size, ResourceManager::GetTexture("target"), glm::vec3(1.0f));
                    this->Cubes.push_back(obj);
                }else{
                    pos = glm::vec3(i * unitSize, j * unitSize, -unitSize);
                    size = glm::vec3(unitSize);
                    obj = GameObject(pos, size, ResourceManager::GetTexture("floor"), glm::vec3(1.0f));
                    this->Cubes.push_back(obj);
                }
            }else if(cubeData[i][j] == 4){ //done box
                glm::vec3 pos(i * unitSize, j * unitSize, 0.0f);
                glm::vec3 size(unitSize);
                GameObject obj(pos, size, ResourceManager::GetTexture("box"), glm::vec3(0.0f, 1.0f, 0.0f));
                this->Cubes.push_back(obj);

                pos = glm::vec3(i * unitSize, j * unitSize, -unitSize);
                size = glm::vec3(unitSize);
                obj = GameObject(pos, size, ResourceManager::GetTexture("target"), glm::vec3(1.0f));
                this->Cubes.push_back(obj);
            }else if(cubeData[i][j] == 3){ //obican box
                glm::vec3 pos(i * unitSize, j * unitSize, 0.0f);
                glm::vec3 size(unitSize);
                GameObject obj(pos, size, ResourceManager::GetTexture("box"), glm::vec3(1.0f));
                this->Cubes.push_back(obj);

                pos = glm::vec3(i * unitSize, j * unitSize, -unitSize);
                size = glm::vec3(unitSize);
                obj = GameObject(pos, size, ResourceManager::GetTexture("floor"), glm::vec3(1.0f));
                this->Cubes.push_back(obj);
            }else if(cubeData[i][j] == 2){ //ciljno polje
                goalSquareData[i][j] = 1;
                glm::vec3 pos(i * unitSize, j * unitSize, -unitSize);
                glm::vec3 size(unitSize);
                GameObject obj(pos, size, ResourceManager::GetTexture("target"), glm::vec3(1.0f, 0.0f, 0.0f));
                this->Cubes.push_back(obj);
            }else if(cubeData[i][j] == 1){ // pod
                glm::vec3 pos(i * unitSize, j * unitSize, -unitSize);
                glm::vec3 size(unitSize);
                GameObject obj(pos, size, ResourceManager::GetTexture("floor"), glm::vec3(1.0f));
                this->Cubes.push_back(obj);
            }
        }
    }
}
