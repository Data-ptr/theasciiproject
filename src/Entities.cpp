/*
 * =====================================================================================
 *
 *       Filename:  Entities.cpp
 *
 *    Description:  This object defines the behavior of our "Entities", ie
 *                  The necessary functions for an entity to exist on the "Map"
 *
 *        Version:  1.0
 *        Created:  07/03/2012 03:41:29 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yamamushi (Jon Rumion)
 *   Organization:  The ASCII Project
 *
 *	  License:  GPLv3
 *
 *	  Copyright 2012 Jonathan Rumion
 *
 *   This file is part of The ASCII Project.
 *
 *   The ASCII Project is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   The ASCII Project is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with The ASCII Project.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * =====================================================================================
 */

#include "Entities.h"
#include "TileMap.h"
#include "EntityMap.h"
#include "WorldMap.h"
#include "ClientMap.h"
#include "RenderMap.h"
#include "FovLib.h"

// Temporary location for these variables, they will be moved elsewhere
// When DB library code is integrated (soon-ish).

unsigned int UIDList = 0;

void Entity::init_entity(wchar_t *p)
{
    
    
    symbol = p;
    X = 0;
    Y = 0;
    
    H = 0;
    S = 0;
    V = 0;
    
    R = 0;
    G = 0;
    B = 0;
    
    TypeID = 0;
    SubTypeID = 0;
    UID = UIDList + 1;
    UIDList++;
    
    
    initialized = false;
    clientActive = false;
    
    int x, y;
    for (x = 0; x < MAP_WIDTH; x++) {
        for (y = 0; y < MAP_HEIGHT; y++) {
            fov[x][y] = false;
            sharedFov[x][y] = false;
        }
    }
    
}


void Entity::setEntName(std::string entName_)
{
    
    entName = entName_;
    
}


std::string Entity::getEntName()
{
    
    return entName;
    
}

void Entity::setPos(int x, int y)
{
    X = x;
    Y = y;
    
}


bool Entity::move(int dx, int dy)
{
    
    if(dx+X > 0 && dx+X < MAP_WIDTH && dy+Y > 0 && dy+Y < MAP_HEIGHT)
    {
        if (initialized) {
            if ((world->virtMap[(dx + X)][(dy + Y)]->blocked) || entMap->checkOccupied(dx+X, dy+Y) || (world->virtMap[(dx+ X)][(dy + Y)]->getTypeID() == 3 && wMap->getEntityZ(entMap, -1)->contextMap->virtMap[(dx+X)][(dy+Y)]->getTypeID() != 1 && wMap->getEntityZ(entMap, -1)->contextMap->virtMap[(dx+X)][(dy+Y)]->getTypeID() != 4 )){
                entMap->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                return false;
            }
            else {
                X += dx;
                Y += dy;
                
                entMap->refreshLightMap();
                entMap->refreshEntityMap();
                
                
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                
                return true;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    
}


bool Entity::removeWall(int dx, int dy)
{
    
    if(dx+X > 0 && dx+X < MAP_WIDTH && dy+Y > 0 && dy+Y < MAP_HEIGHT)
    {
        if (initialized) {
            if ((world->virtMap[(dx + X)][(dy + Y)]->blocked)){
                
                
                if(wMap->getEntityZ(entMap, 1) != nullptr && wMap->getEntityZ(entMap, 1)->contextMap->virtMap[dx+X][dy+Y]->getTypeID() != 3)
                {
                    entMap->contextMap->floorTile(dx+X, dy+Y);
                }
                
                else
                {
                    entMap->contextMap->slopeTile(dx + X, dy + Y);
                }
                //entMap->refreshTileMap();
                entMap->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                return true;
            }
            else {
                
                entMap->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                
                return false;
            }
        }
        else
        {
            return false;
        }
        
        
    }
    else if(dx+X > 0 && dx+X >= MAP_WIDTH && dy+Y >= 0 && dy+Y < MAP_HEIGHT)
    {
        if (initialized) {
            if ((wMap->getNextEntMap(this, 6)->contextMap->virtMap[1][(dy + Y)]->blocked)){
                
                if(wMap->getEntityZ(wMap->getNextEntMap(this, 6), 1) != nullptr && wMap->getEntityZ(wMap->getNextEntMap(this, 6), 1)->contextMap->virtMap[1][dy+Y]->getTypeID() != 3)
                {
                    wMap->getNextEntMap(this, 6)->contextMap->floorTile(1, dy+Y);
                }
                
                
                else
                {
                    wMap->getNextEntMap(this, 6)->contextMap->slopeTile(1, dy+Y);
                }
                
                
                wMap->getNextEntMap(this, 6)->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                return true;
            }
            else {
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                
                return false;
            }
        }
        else
        {
            return false;
        }
        
        
    }
    else if(dx+X <= 0 && dx+X < MAP_WIDTH && dy+Y >= 0 && dy+Y < MAP_HEIGHT)
    {
        if (initialized) {
            if ((wMap->getNextEntMap(this, 4)->contextMap->virtMap[MAP_WIDTH-1][(dy + Y)]->blocked)){
                
                
                
                if(wMap->getEntityZ(wMap->getNextEntMap(this, 4), 1) != nullptr && wMap->getEntityZ(wMap->getNextEntMap(this, 4), 1)->contextMap->virtMap[MAP_WIDTH-1][dy+Y]->getTypeID() != 3)
                {
                    wMap->getNextEntMap(this, 4)->contextMap->floorTile(MAP_WIDTH-1, dy+Y);
                }
                
                else
                {
                    wMap->getNextEntMap(this, 4)->contextMap->slopeTile(MAP_WIDTH-1, dy+Y);
                }
                
                
                wMap->getNextEntMap(this, 4)->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                return true;
            }
            else {
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                
                return false;
            }
        }
        else
        {
            return false;
        }
        
        
    }
    else if(dx+X >= 0 && dx+X < MAP_WIDTH && dy+Y > 0 && dy+Y >= MAP_HEIGHT)
    {
        
        if (initialized) {
            if ((wMap->getNextEntMap(this, 2)->contextMap->virtMap[dx+X][(1)]->blocked)){
                
                
                
                if(wMap->getEntityZ(wMap->getNextEntMap(this, 2), 1) != nullptr && wMap->getEntityZ(wMap->getNextEntMap(this, 2), 1)->contextMap->virtMap[dx+X][1]->getTypeID() != 3)
                {
                    wMap->getNextEntMap(this, 2)->contextMap->floorTile(dx+X, 1);
                }
                
                
                else
                {
                    wMap->getNextEntMap(this, 2)->contextMap->slopeTile(dx+X, 1);
                }
                
                
                wMap->getNextEntMap(this, 2)->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                return true;
            }
            else {
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                
                return false;
            }
        }
        else
        {
            return false;
        }
        
    }
    else if(dx+X >= 0 && dx+X < MAP_WIDTH && dy+Y <= 0 && dy+Y < MAP_HEIGHT)
    {
        
        if (initialized) {
            if ((wMap->getNextEntMap(this, 8)->contextMap->virtMap[dx+X][(MAP_HEIGHT-1)]->blocked)){
                
                
                
                if(wMap->getEntityZ(wMap->getNextEntMap(this, 8), 1) != nullptr && wMap->getEntityZ(wMap->getNextEntMap(this, 8), 1)->contextMap->virtMap[dx+X][MAP_HEIGHT-1]->getTypeID() != 3)
                {
                    wMap->getNextEntMap(this, 8)->contextMap->floorTile(dx+X, MAP_HEIGHT-1);
                }
                
                
                else
                {
                    wMap->getNextEntMap(this, 8)->contextMap->slopeTile(dx+X, MAP_HEIGHT-1);
                }
                
                
                wMap->getNextEntMap(this, 8)->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                return true;
            }
            else {
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                
                return false;
            }
        }
        else
        {
            return false;
        }
        
        
    }
    
    else
    {
        return false;
    }
}



bool Entity::placeWall(int dx, int dy)
{
    
    if(dx+X > 0 && dx+X < MAP_WIDTH && dy+Y > 0 && dy+Y < MAP_HEIGHT)
    {
        if (initialized) {
            if ((world->virtMap[(dx + X)][(dy + Y)]->blocked) || entMap->checkOccupied(dx+X, dy+Y)){
                entMap->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                return false;
            }
            else {
                
                entMap->contextMap->wallTile(dx + X, dy + Y);
                //entMap->refreshTileMap();
                entMap->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                
                return true;
            }
        }
        else
        {
            return false;
        }
        
        
    }
    else if(dx+X > 0 && dx+X >= MAP_WIDTH && dy+Y >= 0 && dy+Y < MAP_HEIGHT)
    {
        if (initialized) {
            if ((wMap->getNextEntMap(this, 6)->contextMap->virtMap[1][(dy + Y)]->blocked) || wMap->getNextEntMap(this, 6)->checkOccupied(1, dy+Y)){
                wMap->getNextEntMap(this, 6)->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                return false;
            }
            else {
                
                wMap->getNextEntMap(this, 6)->contextMap->wallTile(1, dy + Y);
                //entMap->refreshTileMap();
                wMap->getNextEntMap(this, 6)->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                
                return true;
            }
        }
        else
        {
            return false;
        }
        
        
    }
    else if(dx+X <= 0 && dx+X < MAP_WIDTH && dy+Y >= 0 && dy+Y < MAP_HEIGHT)
    {
        if (initialized) {
            if ((wMap->getNextEntMap(this, 4)->contextMap->virtMap[MAP_WIDTH-1][(dy + Y)]->blocked) || wMap->getNextEntMap(this, 4)->checkOccupied(MAP_WIDTH-1, dy+Y)){
                wMap->getNextEntMap(this, 4)->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                return false;
            }
            else {
                
                wMap->getNextEntMap(this, 4)->contextMap->wallTile(MAP_WIDTH-1, dy + Y);
                //entMap->refreshTileMap();
                wMap->getNextEntMap(this, 4)->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                
                return true;
            }
        }
        else
        {
            return false;
        }
        
        
    }
    else if(dx+X >= 0 && dx+X < MAP_WIDTH && dy+Y > 0 && dy+Y >= MAP_HEIGHT)
    {
        if (initialized) {
            if ((wMap->getNextEntMap(this, 2)->contextMap->virtMap[dx+X][(1)]->blocked) || wMap->getNextEntMap(this, 2)->checkOccupied(dx+X, 1)){
                wMap->getNextEntMap(this, 2)->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                return false;
            }
            else {
                
                wMap->getNextEntMap(this, 2)->contextMap->wallTile(dx+X, 1);
                //entMap->refreshTileMap();
                wMap->getNextEntMap(this, 2)->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                
                return true;
            }
        }
        else
        {
            return false;
        }
        
        
    }
    else if(dx+X >= 0 && dx+X < MAP_WIDTH && dy+Y <= 0 && dy+Y < MAP_HEIGHT)
    {
        cout << "checking north" << endl;
        if (initialized) {
            if ((wMap->getNextEntMap(this, 8)->contextMap->virtMap[X][(MAP_HEIGHT-1)]->blocked) || wMap->getNextEntMap(this, 8)->checkOccupied(dx+X, MAP_HEIGHT-1)){
                wMap->getNextEntMap(this, 8)->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                return false;
            }
            else {
                
                wMap->getNextEntMap(this, 8)->contextMap->wallTile(dx+X, MAP_HEIGHT-1);
                //entMap->refreshTileMap();
                wMap->getNextEntMap(this, 8)->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                
                return true;
            }
        }
        else
        {
            return false;
        }
        
        
    }
    else
    {
        return false;
    }
}





bool Entity::placeFloor(int dx, int dy)
{
    
    
    if(dx+X > 0 && dx+X < MAP_WIDTH && dy+Y > 0 && dy+Y < MAP_HEIGHT)
    {
        if (initialized) {
            if ((world->virtMap[(dx + X)][(dy + Y)]->blocked) || entMap->checkOccupied(dx+X, dy+Y)){
                entMap->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                return false;
            }
            else {
                
                entMap->contextMap->floorTile(dx + X, dy + Y);
                //entMap->refreshTileMap();
                entMap->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                
                return true;
            }
        }
        else
        {
            return false;
        }
        
        
    }
    else if(dx+X > 0 && dx+X >= MAP_WIDTH && dy+Y >= 0 && dy+Y < MAP_HEIGHT)
    {
        if (initialized) {
            if ((wMap->getNextEntMap(this, 6)->contextMap->virtMap[1][(dy + Y)]->blocked) || wMap->getNextEntMap(this, 6)->checkOccupied(1, dy+Y)){
                wMap->getNextEntMap(this, 6)->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                return false;
            }
            else {
                
                wMap->getNextEntMap(this, 6)->contextMap->floorTile(1, dy + Y);
                //entMap->refreshTileMap();
                wMap->getNextEntMap(this, 6)->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                
                return true;
            }
        }
        else
        {
            return false;
        }
        
        
    }
    else if(dx+X <= 0 && dx+X < MAP_WIDTH && dy+Y >= 0 && dy+Y < MAP_HEIGHT)
    {
        if (initialized) {
            if ((wMap->getNextEntMap(this, 4)->contextMap->virtMap[MAP_WIDTH-1][(dy + Y)]->blocked) || wMap->getNextEntMap(this, 4)->checkOccupied(MAP_WIDTH-1, dy+Y)){
                wMap->getNextEntMap(this, 4)->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                return false;
            }
            else {
                
                wMap->getNextEntMap(this, 4)->contextMap->floorTile(MAP_WIDTH-1, dy + Y);
                //entMap->refreshTileMap();
                wMap->getNextEntMap(this, 4)->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                
                return true;
            }
        }
        else
        {
            return false;
        }
        
        
    }
    else if(dx+X >= 0 && dx+X < MAP_WIDTH && dy+Y > 0 && dy+Y >= MAP_HEIGHT)
    {
        if (initialized) {
            if ((wMap->getNextEntMap(this, 2)->contextMap->virtMap[dx+X][(1)]->blocked) || wMap->getNextEntMap(this, 2)->checkOccupied(dx+X, 1)){
                wMap->getNextEntMap(this, 2)->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                return false;
            }
            else {
                
                wMap->getNextEntMap(this, 2)->contextMap->floorTile(dx+X, 1);
                //entMap->refreshTileMap();
                wMap->getNextEntMap(this, 2)->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                
                return true;
            }
        }
        else
        {
            return false;
        }
        
        
    }
    else if(dx+X >= 0 && dx+X < MAP_WIDTH && dy+Y <= 0 && dy+Y < MAP_HEIGHT)
    {
        
        if (initialized) {
            if ((wMap->getNextEntMap(this, 8)->contextMap->virtMap[X][(MAP_HEIGHT-1)]->blocked) || wMap->getNextEntMap(this, 8)->checkOccupied(dx+X, MAP_HEIGHT-1)){
                wMap->getNextEntMap(this, 8)->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                return false;
            }
            else {
                
                wMap->getNextEntMap(this, 8)->contextMap->floorTile(dx+X, MAP_HEIGHT-1);
                //entMap->refreshTileMap();
                wMap->getNextEntMap(this, 8)->refreshEntityMap();
                
                if (clientActive) {
                    
                    clientFovSync();
                }
                
                
                return true;
            }
        }
        else
        {
            return false;
        }
        
        
    }
    else
    {
        return false;
    }
    
}


bool Entity::digUp()
{
    
    if(initialized)
    {
        if(wMap->getEntityZ(entMap, 1) != nullptr && wMap->getEntityZ(entMap, 1)->contextMap->virtMap[X][Y]->getTypeID() != 3)
        {
            
            
            
            wMap->getEntityZ(entMap, 1)->contextMap->airTile(X, Y);
            wMap->getEntityZ(entMap, 1)->refreshEntityMap();
            entMap->contextMap->slopeTile(X, Y);
            entMap->refreshEntityMap();
            entMap->refreshLightMap();
            
            if (clientActive) {
                
                clientFovSync();
            }
            
            return true;
            
        }
        else
        {
            return false;
            
        }
        
    }
    else
    {
        return false;
    }
}


bool Entity::digDown()
{
    
    if(initialized)
    {
        if(wMap->getEntityZ(entMap, -1) != nullptr && wMap->getEntityZ(entMap, -1)->contextMap->virtMap[X][Y]->getTypeID() == 1)
        {
            wMap->getEntityZ(entMap, -1)->contextMap->slopeTile(X, Y);
            wMap->getEntityZ(entMap, -1)->refreshEntityMap();
            
            entMap->contextMap->airTile(X, Y);
            
            entMap->refreshEntityMap();
            wMap->getEntityZ(entMap, -1)->refreshLightMap();
            if (clientActive) {
                
                clientFovSync();
            }
            
            return true;
            
        }
        else
        {
            return false;
            
        }
        
    }
    else
    {
        return false;
    }
    
    
}



bool Entity::digHole(int dx, int dy)
{
    
    if(wZ != 0)
    {
        if(dx+X > 0 && dx+X < MAP_WIDTH && dy+Y > 0 && dy+Y < MAP_HEIGHT)
        {
            if (initialized) {
                if (!(world->virtMap[(dx + X)][(dy + Y)]->blocked)){
                    
                    
                    if(wMap->getEntityZ(entMap, -1) == nullptr)
                    {
                        entMap->contextMap->floorTile(dx+X, dy+Y);
                        
                    }
                    
                    else if(wMap->getEntityZ(entMap, -1) != nullptr && wMap->getEntityZ(entMap, -1)->contextMap->virtMap[dx+X][dy+Y]->getTypeID() == 1)
                    {
                        entMap->contextMap->airTile(dx+X, dy+Y);
                        wMap->getEntityZ(entMap, -1)->contextMap->slopeTile(dx+X, dy+Y);
                        wMap->getEntityZ(entMap, -1)->refreshLightMap();
                    }
                    
                    else
                    {
                        entMap->contextMap->airTile(dx + X, dy + Y);
                    }
                    //entMap->refreshTileMap();
                    entMap->refreshEntityMap();
                    
                    
                    if (clientActive) {
                        
                        clientFovSync();
                    }
                    
                    return true;
                }
                else {
                    
                    entMap->refreshEntityMap();
                    
                    if (clientActive) {
                        
                        clientFovSync();
                    }
                    
                    
                    return false;
                }
            }
            else
            {
                return false;
            }
            
            
        }
        else if(dx+X > 0 && dx+X >= MAP_WIDTH && dy+Y >= 0 && dy+Y < MAP_HEIGHT)
        {
            if (initialized) {
                if (!(wMap->getNextEntMap(this, 6)->contextMap->virtMap[1][(dy + Y)]->blocked)){
                    
                    if(wMap->getEntityZ(wMap->getNextEntMap(this, 6), -1) == nullptr)
                    {
                        wMap->getNextEntMap(this, 6)->contextMap->floorTile(1, dy+Y);
                    }
                    
                    else if(wMap->getEntityZ(wMap->getNextEntMap(this, 6), -1) != nullptr && wMap->getEntityZ(wMap->getNextEntMap(this, 6), -1)->contextMap->virtMap[1][dy+Y]->getTypeID() == 1)
                    {
                        wMap->getNextEntMap(this, 6)->contextMap->airTile(1, dy+Y);
                        wMap->getEntityZ(wMap->getNextEntMap(this, 6), -1)->contextMap->slopeTile(1, dy + Y);
                        wMap->getEntityZ(wMap->getNextEntMap(this, 6), -1)->refreshLightMap();
                    }
                    
                    else
                    {
                        wMap->getNextEntMap(this, 6)->contextMap->airTile(1, dy+Y);
                    }
                    
                    
                    wMap->getNextEntMap(this, 6)->refreshEntityMap();
                    
                    
                    if (clientActive) {
                        
                        clientFovSync();
                    }
                    
                    return true;
                }
                else {
                    
                    if (clientActive) {
                        
                        clientFovSync();
                    }
                    
                    
                    return false;
                }
            }
            else
            {
                return false;
            }
            
            
        }
        else if(dx+X <= 0 && dx+X < MAP_WIDTH && dy+Y >= 0 && dy+Y < MAP_HEIGHT)
        {
            if (initialized) {
                if (!(wMap->getNextEntMap(this, 4)->contextMap->virtMap[MAP_WIDTH-1][(dy + Y)]->blocked)){
                    
                    
                    
                    if(wMap->getEntityZ(wMap->getNextEntMap(this, 4), -1) == nullptr)
                    {
                        wMap->getNextEntMap(this, 4)->contextMap->floorTile(MAP_WIDTH-1, dy+Y);
                    }
                    
                    else if(wMap->getEntityZ(wMap->getNextEntMap(this, 4), -1) != nullptr && wMap->getEntityZ(wMap->getNextEntMap(this, 4), -1)->contextMap->virtMap[MAP_WIDTH-1][dy+Y]->getTypeID() == 1)
                    {
                        wMap->getNextEntMap(this, 4)->contextMap->airTile(MAP_WIDTH-1, dy+Y);
                        wMap->getEntityZ(wMap->getNextEntMap(this, 4), -1)->contextMap->slopeTile(MAP_WIDTH-1, dy + Y);
                        wMap->getEntityZ(wMap->getNextEntMap(this, 4), -1)->refreshLightMap();
                    }
                    
                    else
                    {
                        wMap->getNextEntMap(this, 4)->contextMap->airTile(MAP_WIDTH-1, dy+Y);
                    }
                    
                    
                    wMap->getNextEntMap(this, 4)->refreshEntityMap();
                    
                    
                    if (clientActive) {
                        
                        clientFovSync();
                    }
                    
                    return true;
                }
                else {
                    
                    if (clientActive) {
                        
                        clientFovSync();
                    }
                    
                    
                    return false;
                }
            }
            else
            {
                return false;
            }
            
            
        }
        else if(dx+X >= 0 && dx+X < MAP_WIDTH && dy+Y > 0 && dy+Y >= MAP_HEIGHT)
        {
            
            if (initialized) {
                if (!(wMap->getNextEntMap(this, 2)->contextMap->virtMap[dx+X][(1)]->blocked)){
                    
                    
                    
                    if(wMap->getEntityZ(wMap->getNextEntMap(this, 2), -1) != nullptr && wMap->getEntityZ(wMap->getNextEntMap(this, 2), -1)->contextMap->virtMap[dx+X][1]->getTypeID() != 1)
                    {
                        wMap->getNextEntMap(this, 2)->contextMap->floorTile(dx+X, 1);
                    }
                    
                    else if(wMap->getEntityZ(wMap->getNextEntMap(this, 2), -1) != nullptr && wMap->getEntityZ(wMap->getNextEntMap(this, 2), -1)->contextMap->virtMap[dx+X][1]->getTypeID() == 1)
                    {
                        wMap->getNextEntMap(this, 2)->contextMap->airTile(dx+X, 1);
                        wMap->getEntityZ(wMap->getNextEntMap(this, 2), -1)->contextMap->slopeTile(dx+X, 1);
                        wMap->getEntityZ(wMap->getNextEntMap(this, 2), -1)->refreshLightMap();
                    }
                    
                    else
                    {
                        wMap->getNextEntMap(this, 2)->contextMap->airTile(dx+X, 1);
                    }
                    
                    
                    wMap->getNextEntMap(this, 2)->refreshEntityMap();
                    
                    
                    if (clientActive) {
                        
                        clientFovSync();
                    }
                    
                    return true;
                }
                else {
                    
                    if (clientActive) {
                        
                        clientFovSync();
                    }
                    
                    
                    return false;
                }
            }
            else
            {
                return false;
            }
            
        }
        else if(dx+X >= 0 && dx+X < MAP_WIDTH && dy+Y <= 0 && dy+Y < MAP_HEIGHT)
        {
            
            if (initialized) {
                if (!(wMap->getNextEntMap(this, 8)->contextMap->virtMap[dx+X][(MAP_HEIGHT-1)]->blocked)){
                    
                    
                    
                    if(wMap->getEntityZ(wMap->getNextEntMap(this, 8), -1) != nullptr && wMap->getEntityZ(wMap->getNextEntMap(this, 8), -1)->contextMap->virtMap[dx+X][MAP_HEIGHT-1]->getTypeID() != 1)
                    {
                        wMap->getNextEntMap(this, 8)->contextMap->floorTile(dx+X, MAP_HEIGHT-1);
                    }
                    
                    else if(wMap->getEntityZ(wMap->getNextEntMap(this, 8), -1) != nullptr && wMap->getEntityZ(wMap->getNextEntMap(this, 8), -1)->contextMap->virtMap[dx+X][MAP_HEIGHT-1]->getTypeID() == 1)
                    {
                        
                        wMap->getNextEntMap(this, 8)->contextMap->airTile(dx+X, MAP_HEIGHT-1);
                        wMap->getEntityZ(wMap->getNextEntMap(this, 8), -1)->contextMap->slopeTile(dx+X, MAP_HEIGHT-1);
                        wMap->getEntityZ(wMap->getNextEntMap(this, 8), -1)->refreshLightMap();
                    }
                    
                    
                    else
                    {
                        wMap->getNextEntMap(this, 8)->contextMap->airTile(dx+X, MAP_HEIGHT-1);
                    }
                    
                    
                    wMap->getNextEntMap(this, 8)->refreshEntityMap();
                    
                    
                    if (clientActive) {
                        
                        clientFovSync();
                    }
                    
                    return true;
                }
                else {
                    
                    if (clientActive) {
                        
                        clientFovSync();
                    }
                    
                    
                    return false;
                }
            }
            else
            {
                return false;
            }
            
            
        }
        
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    
}






void Entity::setGlobal(WorldMap *WMap)
{
    
    if(WMap != nullptr)
        wMap = WMap;
    
    
}



void Entity::setWorldPosition(int x, int y, int z)
{
    
    wX = x;
    wY = y;
    wZ = z;
    
    
}

void Entity::clientFovSync(bool forceReset){
    
    
    //refreshFov(10);
    
    int pX, pY;
    pX = X;
    pY = Y;
    
    int cX, cY;
    cX = MAP_WIDTH/2;
    cY = MAP_HEIGHT/2;
    
    
    int offsetX = cX-25;//cY-4;
    int offsetY = cY-4;
    
    

    
    //cMap->clientRefresh();
    
    FOV->refreshFov(this, cX, cY, 7);
    
    int x = pX-offsetX;
    for(int iX = cX-offsetX; iX < cX+offsetX; iX++ )
    {
        int y = pY-offsetY;
        for(int iY = cY-offsetY; iY < cY+offsetY; iY++ )
        {
            cMap->cMap[iX][iY]->explored = true;
            //cMap->cMap[iX][iY]->visible = false;
            
            if(forceReset)
            {
                cMap->cMap[iX][iY]->sendMe = true;
            }
            
                
            if(y > 0 && y < MAP_HEIGHT && x > 0 && x < MAP_WIDTH)
                
            {
                
                
                if(cMap->cMap[iX][iY]->symbol != rMap->getSymbol(x, y))
                {
                    cMap->cMap[iX][iY]->sendMe = true;
                    cMap->cMap[iX][iY]->symbol = rMap->getSymbol(x, y);
                }
                if(cMap->cMap[iX][iY]->H != rMap->returnH(x, y))
                {
                    cMap->cMap[iX][iY]->sendMe = true;
                    cMap->cMap[iX][iY]->H = rMap->returnH(x, y);
                }
                if(cMap->cMap[iX][iY]->S != rMap->returnS(x, y))
                {
                    cMap->cMap[iX][iY]->sendMe = true;
                    cMap->cMap[iX][iY]->S = rMap->returnS(x, y);
                }
                if(cMap->cMap[iX][iY]->V != rMap->returnV(x, y))
                {
                    cMap->cMap[iX][iY]->sendMe = true;
                    cMap->cMap[iX][iY]->V = rMap->returnV(x, y);
                }
                
                if(cMap->cMap[iX][iY]->HD != rMap->returnHD(x, y))
                {
                    cMap->cMap[iX][iY]->sendMe = true;
                    cMap->cMap[iX][iY]->HD = rMap->returnHD(x, y);
                }
                if(cMap->cMap[iX][iY]->SD != rMap->returnSD(x, y))
                {
                    cMap->cMap[iX][iY]->sendMe = true;
                    cMap->cMap[iX][iY]->SD = rMap->returnSD(x, y);
                }
                if(cMap->cMap[iX][iY]->VD != rMap->returnVD(x, y))
                {
                    cMap->cMap[iX][iY]->sendMe = true;
                    cMap->cMap[iX][iY]->VD = rMap->returnVD(x, y);
                }
                
                
                if(cMap->cMap[iX][iY]->blocked != rMap->returnBlocked(x, y))
                {
                    cMap->cMap[iX][iY]->sendMe = true;
                    cMap->cMap[iX][iY]->blocked = rMap->returnBlocked(x, y);
                }
                if(cMap->cMap[iX][iY]->blockSight != rMap->returnBlockSight(x, y))
                {
                    cMap->cMap[iX][iY]->sendMe = true;
                    cMap->cMap[iX][iY]->blockSight = rMap->returnBlockSight(x, y);
                }
                
                if(cMap->cMap[iX][iY]->isLit != rMap->returnLit(x, y))
                {
                    cMap->cMap[iX][iY]->sendMe = true;
                    cMap->cMap[iX][iY]->isLit = rMap->returnLit(x, y);
                }
                                          
                
            }
            
            else if(y > 0 && y > MAP_HEIGHT && x > 0 && x < MAP_WIDTH)
            {
                
                RenderMap *trMap = wMap->getNextEntMap(this, 2)->rMap;
                int iy;
                iy = y-MAP_HEIGHT;
                
                if(cMap->cMap[iX][iY-1]->symbol != trMap->getSymbol(x, iy))
                {
                    cMap->cMap[iX][iY-1]->symbol = trMap->getSymbol(x, iy);
                    cMap->cMap[iX][iY-1]->sendMe = true;
                    
                }
                if(cMap->cMap[iX][iY-1]->H != trMap->returnH(x, iy))
                {
                    cMap->cMap[iX][iY-1]->H = trMap->returnH(x, iy);
                    cMap->cMap[iX][iY-1]->sendMe = true;
                    
                }
                if(cMap->cMap[iX][iY-1]->HD != trMap->returnHD(x, iy))
                {
                    cMap->cMap[iX][iY-1]->HD = trMap->returnHD(x, iy);
                    cMap->cMap[iX][iY-1]->sendMe = true;
                    
                }
                if(cMap->cMap[iX][iY-1]->S != trMap->returnS(x, iy))
                {
                    cMap->cMap[iX][iY-1]->S = trMap->returnS(x, iy);
                    cMap->cMap[iX][iY-1]->sendMe = true;
                    
                }
                if(cMap->cMap[iX][iY-1]->SD != trMap->returnSD(x, iy))
                {
                    cMap->cMap[iX][iY-1]->SD = trMap->returnSD(x, iy);
                    cMap->cMap[iX][iY-1]->sendMe = true;
                    
                }
                if(cMap->cMap[iX][iY-1]->V != trMap->returnV(x, iy))
                {
                    cMap->cMap[iX][iY-1]->V = trMap->returnV(x, iy);
                    cMap->cMap[iX][iY-1]->sendMe = true;
                    
                }
                if(cMap->cMap[iX][iY-1]->VD != trMap->returnVD(x, iy))
                {
                    cMap->cMap[iX][iY-1]->VD = trMap->returnVD(x, iy);
                    cMap->cMap[iX][iY-1]->sendMe = true;
                    
                }
                
                if(cMap->cMap[iX][iY-1]->blocked != trMap->returnBlocked(x, iy))
                {
                    cMap->cMap[iX][iY-1]->blocked = trMap->returnBlocked(x, iy);
                    cMap->cMap[iX][iY-1]->sendMe = true;
                    
                }
                if(cMap->cMap[iX][iY-1]->blockSight != trMap->returnBlockSight(x, iy))
                {
                    cMap->cMap[iX][iY-1]->blockSight = trMap->returnBlockSight(x, iy);
                    cMap->cMap[iX][iY-1]->sendMe = true;
                    
                }
                
                if(cMap->cMap[iX][iY-1]->isLit != trMap->returnLit(x, iy))
                {
                    cMap->cMap[iX][iY-1]->isLit = trMap->returnLit(x, iy);
                    cMap->cMap[iX][iY-1]->sendMe = true;
                    
                }
                                                
            }
            
            else if(y < 0 && y < MAP_HEIGHT && x > 0 && x < MAP_WIDTH)
            {
                
                RenderMap *trMap = wMap->getNextEntMap(this, 8)->rMap;
                int iy;
                iy = MAP_HEIGHT+y;
                
                if(cMap->cMap[iX][iY+1]->symbol != trMap->getSymbol(x, iy))
                {
                    cMap->cMap[iX][iY+1]->symbol = trMap->getSymbol(x, iy);
                    cMap->cMap[iX][iY+1]->sendMe = true;
                }
                
                if(cMap->cMap[iX][iY+1]->H != trMap->returnH(x, iy))
                {
                    cMap->cMap[iX][iY+1]->H = trMap->returnH(x, iy);
                    cMap->cMap[iX][iY+1]->sendMe = true;
                }
                if(cMap->cMap[iX][iY+1]->HD!= trMap->returnHD(x, iy))
                {
                    cMap->cMap[iX][iY+1]->HD = trMap->returnHD(x, iy);
                    cMap->cMap[iX][iY+1]->sendMe = true;
                }
                if(cMap->cMap[iX][iY+1]->S != trMap->returnS(x, iy))
                {
                    cMap->cMap[iX][iY+1]->S = trMap->returnS(x, iy);
                    cMap->cMap[iX][iY+1]->sendMe = true;
                }
                if(cMap->cMap[iX][iY+1]->SD != trMap->returnSD(x, iy))
                {
                    cMap->cMap[iX][iY+1]->SD = trMap->returnSD(x, iy);
                    cMap->cMap[iX][iY+1]->sendMe = true;
                }
                if(cMap->cMap[iX][iY+1]->V != trMap->returnV(x, iy))
                {
                    cMap->cMap[iX][iY+1]->V = trMap->returnV(x, iy);
                    cMap->cMap[iX][iY+1]->sendMe = true;
                }
                if(cMap->cMap[iX][iY+1]->VD != trMap->returnVD(x, iy))
                {
                    cMap->cMap[iX][iY+1]->VD = trMap->returnVD(x, iy);
                    cMap->cMap[iX][iY+1]->sendMe = true;
                }
                
                
                if(cMap->cMap[iX][iY+1]->blocked != trMap->returnBlocked(x, iy))
                {
                    cMap->cMap[iX][iY+1]->blocked = trMap->returnBlocked(x, iy);
                    cMap->cMap[iX][iY+1]->sendMe = true;
                }
                if(cMap->cMap[iX][iY+1]->blockSight != trMap->returnBlockSight(x, iy))
                {
                    cMap->cMap[iX][iY+1]->blockSight = trMap->returnBlockSight(x, iy);
                    cMap->cMap[iX][iY+1]->sendMe = true;
                }
                                
                if(cMap->cMap[iX][iY+1]->isLit != trMap->returnLit(x, iy))
                {
                    cMap->cMap[iX][iY+1]->isLit = trMap->returnLit(x, iy);
                    cMap->cMap[iX][iY+1]->sendMe = true;
                }
                                
            }
            
            else if(y > 0 && y < MAP_HEIGHT && x < 0 && x < MAP_WIDTH)
            {
                
                RenderMap *trMap = wMap->getNextEntMap(this, 4)->rMap;
                int ix;
                ix = MAP_WIDTH+x;
                
                if(cMap->cMap[iX+1][iY]->symbol != trMap->getSymbol(ix, y))
                {
                    
                    cMap->cMap[iX+1][iY]->symbol = trMap->getSymbol(ix, y);
                    cMap->cMap[iX+1][iY]->sendMe = true;
                    
                }
                
                if(cMap->cMap[iX+1][iY]->H != trMap->returnH(ix, y))
                {
                    
                    cMap->cMap[iX+1][iY]->H = trMap->returnH(ix, y);
                    cMap->cMap[iX+1][iY]->sendMe = true;
                    
                }
                if(cMap->cMap[iX+1][iY]->HD != trMap->returnHD(ix, y))
                {
                    
                    cMap->cMap[iX+1][iY]->HD = trMap->returnHD(ix, y);
                    cMap->cMap[iX+1][iY]->sendMe = true;
                    
                }
                if(cMap->cMap[iX+1][iY]->S != trMap->returnS(ix, y))
                {
                    
                    cMap->cMap[iX+1][iY]->S = trMap->returnS(ix, y);
                    cMap->cMap[iX+1][iY]->sendMe = true;
                    
                }
                if(cMap->cMap[iX+1][iY]->SD != trMap->returnSD(ix, y))
                {
                    
                    cMap->cMap[iX+1][iY]->SD = trMap->returnSD(ix, y);
                    cMap->cMap[iX+1][iY]->sendMe = true;
                    
                }
                if(cMap->cMap[iX+1][iY]->V != trMap->returnV(ix, y))
                {
                    
                    cMap->cMap[iX+1][iY]->V = trMap->returnV(ix, y);
                    cMap->cMap[iX+1][iY]->sendMe = true;
                    
                }
                if(cMap->cMap[iX+1][iY]->VD != trMap->returnVD(ix, y))
                {
                    
                    cMap->cMap[iX+1][iY]->VD = trMap->returnVD(ix, y);
                    cMap->cMap[iX+1][iY]->sendMe = true;
                    
                }
                
                if(cMap->cMap[iX+1][iY]->blocked != trMap->returnBlocked(ix, y))
                {
                    
                    cMap->cMap[iX+1][iY]->blocked = trMap->returnBlocked(ix, y);
                    cMap->cMap[iX+1][iY]->sendMe = true;
                    
                }
                if(cMap->cMap[iX+1][iY]->blockSight != trMap->returnBlockSight(ix, y))
                {
                    
                    cMap->cMap[iX+1][iY]->blockSight = trMap->returnBlockSight(ix, y);
                    cMap->cMap[iX+1][iY]->sendMe = true;
                    
                }
                if(cMap->cMap[iX+1][iY]->isLit != trMap->returnLit(ix, y))
                {
                    
                    cMap->cMap[iX+1][iY]->isLit = trMap->returnLit(ix, y);
                    cMap->cMap[iX+1][iY]->sendMe = true;
                    
                }

               
            }
            
            
            else if(y > 0 && y < MAP_HEIGHT && x > 0 && x > MAP_WIDTH)
            {
                
                RenderMap *trMap = wMap->getNextEntMap(this, 6)->rMap;
                int ix;
                ix = x-MAP_WIDTH;
                
                
                if(cMap->cMap[iX-1][iY]->symbol != trMap->getSymbol(ix, y))
                {
                    cMap->cMap[iX-1][iY]->symbol = trMap->getSymbol(ix, y);
                    cMap->cMap[iX-1][iY]->sendMe = true;
                }
                
                
                if(cMap->cMap[iX-1][iY]->H != trMap->returnH(ix, y))
                {
                    cMap->cMap[iX-1][iY]->H = trMap->returnH(ix, y);
                    cMap->cMap[iX-1][iY]->sendMe = true;
                }
                if(cMap->cMap[iX-1][iY]->HD != trMap->returnHD(ix, y))
                {
                    cMap->cMap[iX-1][iY]->HD = trMap->returnHD(ix, y);
                    cMap->cMap[iX-1][iY]->sendMe = true;
                }
                if(cMap->cMap[iX-1][iY]->S != trMap->returnS(ix, y))
                {
                    cMap->cMap[iX-1][iY]->S = trMap->returnS(ix, y);
                    cMap->cMap[iX-1][iY]->sendMe = true;
                }
                if(cMap->cMap[iX-1][iY]->SD != trMap->returnSD(ix, y))
                {
                    cMap->cMap[iX-1][iY]->SD = trMap->returnSD(ix, y);
                    cMap->cMap[iX-1][iY]->sendMe = true;
                }
                if(cMap->cMap[iX-1][iY]->V != trMap->returnV(ix, y))
                {
                    cMap->cMap[iX-1][iY]->V = trMap->returnV(ix, y);
                    cMap->cMap[iX-1][iY]->sendMe = true;
                }
                if(cMap->cMap[iX-1][iY]->VD != trMap->returnVD(ix, y))
                {
                    cMap->cMap[iX-1][iY]->VD = trMap->returnVD(ix, y);
                    cMap->cMap[iX-1][iY]->sendMe = true;
                }
                    
                
                if(cMap->cMap[iX-1][iY]->blocked != trMap->returnBlocked(ix, y))
                {
                    cMap->cMap[iX-1][iY]->blocked = trMap->returnBlocked(ix, y);
                    cMap->cMap[iX-1][iY]->sendMe = true;
                }
                if(cMap->cMap[iX-1][iY]->blockSight != trMap->returnBlockSight(ix, y))
                {
                    cMap->cMap[iX-1][iY]->blockSight = trMap->returnBlockSight(ix, y);
                    cMap->cMap[iX-1][iY]->sendMe = true;
                }
                if(cMap->cMap[iX-1][iY]->isLit != trMap->returnLit(ix, y))
                {
                    cMap->cMap[iX-1][iY]->isLit = trMap->returnLit(ix, y);
                    cMap->cMap[iX-1][iY]->sendMe = true;
                }

                
            }
            else if(y < 0 && y < MAP_HEIGHT && x > 0 && x > MAP_WIDTH)
            {
                
                RenderMap *trMap = wMap->getNextEntMap(this, 9)->rMap;
                int ix, iy;
                ix = x-MAP_WIDTH;
                iy = MAP_HEIGHT+y;
                
                if(cMap->cMap[iX-1][iY+1]->symbol != trMap->getSymbol(ix, iy))
                {
                    cMap->cMap[iX-1][iY+1]->symbol = trMap->getSymbol(ix, iy);
                    cMap->cMap[iX-1][iY+1]->sendMe = true;
                }
                
                
                if(cMap->cMap[iX-1][iY+1]->H != trMap->returnH(ix, iy))
                {
                    cMap->cMap[iX-1][iY+1]->H = trMap->returnH(ix, iy);
                    cMap->cMap[iX-1][iY+1]->sendMe = true;
                }
                if(cMap->cMap[iX-1][iY+1]->HD != trMap->returnHD(ix, iy))
                {
                    cMap->cMap[iX-1][iY+1]->HD = trMap->returnHD(ix, iy);
                    cMap->cMap[iX-1][iY+1]->sendMe = true;
                }
                if(cMap->cMap[iX-1][iY+1]->S != trMap->returnS(ix, iy))
                {
                    cMap->cMap[iX-1][iY+1]->S = trMap->returnS(ix, iy);
                    cMap->cMap[iX-1][iY+1]->sendMe = true;
                }
                if(cMap->cMap[iX-1][iY+1]->SD != trMap->returnSD(ix, iy))
                {
                    cMap->cMap[iX-1][iY+1]->SD = trMap->returnSD(ix, iy);
                    cMap->cMap[iX-1][iY+1]->sendMe = true;
                }
                if(cMap->cMap[iX-1][iY+1]->V != trMap->returnV(ix, iy))
                {
                    cMap->cMap[iX-1][iY+1]->V = trMap->returnV(ix, iy);
                    cMap->cMap[iX-1][iY+1]->sendMe = true;
                }
                if(cMap->cMap[iX-1][iY+1]->VD != trMap->returnVD(ix, iy))
                {
                    cMap->cMap[iX-1][iY+1]->VD = trMap->returnVD(ix, iy);
                    cMap->cMap[iX-1][iY+1]->sendMe = true;
                }

                
                if(cMap->cMap[iX-1][iY+1]->blocked != trMap->returnBlocked(ix, iy))
                {
                    cMap->cMap[iX-1][iY+1]->blocked = trMap->returnBlocked(ix, iy);
                    cMap->cMap[iX-1][iY+1]->sendMe = true;
                }
                if(cMap->cMap[iX-1][iY+1]->blockSight != trMap->returnBlockSight(ix, iy))
                {
                    cMap->cMap[iX-1][iY+1]->blockSight = trMap->returnBlockSight(ix, iy);
                    cMap->cMap[iX-1][iY+1]->sendMe = true;
                }
                if(cMap->cMap[iX-1][iY+1]->isLit != trMap->returnLit(ix, iy))
                {
                    cMap->cMap[iX-1][iY+1]->isLit = trMap->returnLit(ix, iy);
                    cMap->cMap[iX-1][iY+1]->sendMe = true;
                }

               
            }
            else if(y > 0 && y > MAP_HEIGHT && x > 0 && x > MAP_WIDTH)
            {
                
                RenderMap *trMap = wMap->getNextEntMap(this, 3)->rMap;
                int ix, iy;
                ix = x-MAP_WIDTH;
                iy = y-MAP_HEIGHT;
                
                if(cMap->cMap[iX-1][iY-1]->symbol != trMap->getSymbol(ix, iy))
                {
                    cMap->cMap[iX-1][iY-1]->symbol = trMap->getSymbol(ix, iy);
                    cMap->cMap[iX-1][iY-1]->sendMe = true;
                }
                
                
                if(cMap->cMap[iX-1][iY-1]->H != trMap->returnH(ix, iy))
                {
                    cMap->cMap[iX-1][iY-1]->H = trMap->returnH(ix, iy);
                    cMap->cMap[iX-1][iY-1]->sendMe = true;
                }
                if(cMap->cMap[iX-1][iY-1]->HD != trMap->returnHD(ix, iy))
                {
                    cMap->cMap[iX-1][iY-1]->HD = trMap->returnHD(ix, iy);
                    cMap->cMap[iX-1][iY-1]->sendMe = true;
                }
                if(cMap->cMap[iX-1][iY-1]->S != trMap->returnS(ix, iy))
                {
                    cMap->cMap[iX-1][iY-1]->S = trMap->returnS(ix, iy);
                    cMap->cMap[iX-1][iY-1]->sendMe = true;
                }
                if(cMap->cMap[iX-1][iY-1]->SD != trMap->returnSD(ix, iy))
                {
                    cMap->cMap[iX-1][iY-1]->SD = trMap->returnSD(ix, iy);
                    cMap->cMap[iX-1][iY-1]->sendMe = true;
                }
                if(cMap->cMap[iX-1][iY-1]->V != trMap->returnV(ix, iy))
                {
                    cMap->cMap[iX-1][iY-1]->V = trMap->returnV(ix, iy);
                    cMap->cMap[iX-1][iY-1]->sendMe = true;
                }
                if(cMap->cMap[iX-1][iY-1]->VD != trMap->returnVD(ix, iy))
                {
                    cMap->cMap[iX-1][iY-1]->VD = trMap->returnVD(ix, iy);
                    cMap->cMap[iX-1][iY-1]->sendMe = true;
                }

                if(cMap->cMap[iX-1][iY-1]->blocked != trMap->returnBlocked(ix, iy))
                {
                    cMap->cMap[iX-1][iY-1]->blocked = trMap->returnBlocked(ix, iy);
                    cMap->cMap[iX-1][iY-1]->sendMe = true;
                }
                if(cMap->cMap[iX-1][iY-1]->blockSight != trMap->returnBlockSight(ix, iy))
                {
                    cMap->cMap[iX-1][iY-1]->blockSight = trMap->returnBlockSight(ix, iy);
                    cMap->cMap[iX-1][iY-1]->sendMe = true;
                }
                if(cMap->cMap[iX-1][iY-1]->isLit != trMap->returnLit(ix, iy))
                {
                    cMap->cMap[iX-1][iY-1]->isLit = trMap->returnLit(ix, iy);
                    cMap->cMap[iX-1][iY-1]->sendMe = true;
                }
          
                
            }
            else if(y < 0 && y < MAP_HEIGHT && x < 0 && x < MAP_WIDTH)
            {
                
                RenderMap *trMap = wMap->getNextEntMap(this, 7)->rMap;
                int ix, iy;
                ix = MAP_WIDTH+x;
                iy = MAP_HEIGHT+y;
                
                if(cMap->cMap[iX+1][iY+1]->symbol != trMap->getSymbol(ix, iy))
                {
                    cMap->cMap[iX+1][iY+1]->symbol = trMap->getSymbol(ix, iy);
                    cMap->cMap[iX+1][iY+1]->sendMe = true;
                }
                
                if(cMap->cMap[iX+1][iY+1]->H != trMap->returnH(ix, iy))
                {
                    cMap->cMap[iX+1][iY+1]->H = trMap->returnH(ix, iy);
                    cMap->cMap[iX+1][iY+1]->sendMe = true;
                }
                if(cMap->cMap[iX+1][iY+1]->HD != trMap->returnHD(ix, iy))
                {
                    cMap->cMap[iX+1][iY+1]->HD = trMap->returnHD(ix, iy);
                    cMap->cMap[iX+1][iY+1]->sendMe = true;
                }
                if(cMap->cMap[iX+1][iY+1]->S != trMap->returnS(ix, iy))
                {
                    cMap->cMap[iX+1][iY+1]->S = trMap->returnS(ix, iy);
                    cMap->cMap[iX+1][iY+1]->sendMe = true;
                }
                if(cMap->cMap[iX+1][iY+1]->SD != trMap->returnSD(ix, iy))
                {
                    cMap->cMap[iX+1][iY+1]->SD = trMap->returnSD(ix, iy);
                    cMap->cMap[iX+1][iY+1]->sendMe = true;
                }
                if(cMap->cMap[iX+1][iY+1]->V != trMap->returnV(ix, iy))
                {
                    cMap->cMap[iX+1][iY+1]->V = trMap->returnV(ix, iy);
                    cMap->cMap[iX+1][iY+1]->sendMe = true;
                }
                if(cMap->cMap[iX+1][iY+1]->VD != trMap->returnVD(ix, iy))
                {
                    cMap->cMap[iX+1][iY+1]->VD = trMap->returnVD(ix, iy);
                    cMap->cMap[iX+1][iY+1]->sendMe = true;
                }
                
                
                if(cMap->cMap[iX+1][iY+1]->blocked != trMap->returnBlocked(ix, iy))
                {
                    cMap->cMap[iX+1][iY+1]->blocked = trMap->returnBlocked(ix, iy);
                    cMap->cMap[iX+1][iY+1]->sendMe = true;
                }
                if(cMap->cMap[iX+1][iY+1]->blockSight != trMap->returnBlockSight(ix, iy))
                {
                    cMap->cMap[iX+1][iY+1]->blockSight = trMap->returnBlockSight(ix, iy);
                    cMap->cMap[iX+1][iY+1]->sendMe = true;
                }
                if(cMap->cMap[iX+1][iY+1]->isLit != trMap->returnLit(ix, iy))
                {
                    cMap->cMap[iX+1][iY+1]->isLit = trMap->returnLit(ix, iy);
                    cMap->cMap[iX+1][iY+1]->sendMe = true;
                }

               
                
            }
            else if(y > 0 && y > MAP_HEIGHT && x < 0 && x < MAP_WIDTH)
            {
                
                RenderMap *trMap = wMap->getNextEntMap(this, 1)->rMap;
                int ix, iy;
                ix = MAP_WIDTH+x;
                iy = y-MAP_HEIGHT;
                
                if(cMap->cMap[iX+1][iY-1]->symbol != trMap->getSymbol(ix, iy))
                {
                    cMap->cMap[iX+1][iY-1]->symbol = trMap->getSymbol(ix, iy);
                    cMap->cMap[iX+1][iY-1]->sendMe = true;
                }
                
                
                if(cMap->cMap[iX+1][iY-1]->H != trMap->returnH(ix, iy))
                {
                    cMap->cMap[iX+1][iY-1]->H = trMap->returnH(ix, iy);
                    cMap->cMap[iX+1][iY-1]->sendMe = true;
                }
                if(cMap->cMap[iX+1][iY-1]->HD != trMap->returnHD(ix, iy))
                {
                    cMap->cMap[iX+1][iY-1]->HD = trMap->returnHD(ix, iy);
                    cMap->cMap[iX+1][iY-1]->sendMe = true;
                }
                if(cMap->cMap[iX+1][iY-1]->S != trMap->returnS(ix, iy))
                {
                    cMap->cMap[iX+1][iY-1]->S = trMap->returnS(ix, iy);
                    cMap->cMap[iX+1][iY-1]->sendMe = true;
                }
                if(cMap->cMap[iX+1][iY-1]->SD != trMap->returnSD(ix, iy))
                {
                    cMap->cMap[iX+1][iY-1]->SD = trMap->returnSD(ix, iy);
                    cMap->cMap[iX+1][iY-1]->sendMe = true;
                }
                if(cMap->cMap[iX+1][iY-1]->V != trMap->returnV(ix, iy))
                {
                    cMap->cMap[iX+1][iY-1]->V = trMap->returnV(ix, iy);
                    cMap->cMap[iX+1][iY-1]->sendMe = true;
                }
                if(cMap->cMap[iX+1][iY-1]->VD != trMap->returnVD(ix, iy))
                {
                    cMap->cMap[iX+1][iY-1]->VD = trMap->returnVD(ix, iy);
                    cMap->cMap[iX+1][iY-1]->sendMe = true;
                }

                
                if(cMap->cMap[iX+1][iY-1]->blocked != trMap->returnBlocked(ix, iy))
                {
                    cMap->cMap[iX+1][iY-1]->blocked = trMap->returnBlocked(ix, iy);
                    cMap->cMap[iX+1][iY-1]->sendMe = true;
                }
                if(cMap->cMap[iX+1][iY-1]->blockSight != trMap->returnBlockSight(ix, iy))
                {
                    cMap->cMap[iX+1][iY-1]->blockSight = trMap->returnBlockSight(ix, iy);
                    cMap->cMap[iX+1][iY-1]->sendMe = true;
                }
                if(cMap->cMap[iX+1][iY-1]->isLit != trMap->returnLit(ix, iy))
                {
                    cMap->cMap[iX+1][iY-1]->isLit = trMap->returnLit(ix, iy);
                    cMap->cMap[iX+1][iY-1]->sendMe = true;
                }
            
            }
            
            y++;
        }
      
        x++;
    }
    
   // FOV->refreshFov(this, cX, cY, 7);

    
}

void Entity::refreshFov(int radius)
{
    
    FOV->refreshFov(this, radius);
    
}

int Entity::posX()
{
    return X;
}

int Entity::posY()
{
    return Y;
}

void Entity::init_in_world(FovLib *fovLib)
{
    
    FOV = fovLib;
    initialized = true;
    world = fovLib->getTileMap();
    
    
    
    
}

wchar_t *Entity::getSymbol()
{
    
    return symbol;
    
}

void Entity::setSymbol(wchar_t *ch)
{
    symbol = ch;
}

void Entity::setTypeID(int i)
{
    TypeID = i;
}

void Entity::setSubTypeID(int i)
{
    SubTypeID = i;
}

bool Entity::isInitialized()
{
    
    return initialized;
    
}

void Entity::associateClient(RenderMap *RMap)
{
    if(cMap != nullptr)
    {
        delete cMap;
    }
    cMap = new ClientMap();
    rMap = RMap;
    clientActive = true;
    
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            //fov[x][y] = false;
            cMap->cMap[x][y]->explored = false;
        }
    }
    
    entMap->refreshEntityMap();
    
    clientFovSync();
    //   refreshFov();
}

bool Entity::getAssociated()
{
    
    return clientActive;
    
}

ClientMap *Entity::returnCMap()
{
    
    return cMap;
    
}

void Entity::move_self(int dx, int dy)
{
    
    move(dx, dy);
    
    
};

void Entity::setEntityMap(EntityMap *map)
{
    
    entMap = map;
    entMap->refreshEntityMap();
    
};

/*
 *
 *
 *		Entities
 *
 *	Now we start listing some entity types
 *
 *	Though how this works will likely change in the
 *	future.
 *
 *
 *
 */



Monster::Monster() : Entity((wchar_t *)L"\uFFF7")
{
    
    //Entity((wchar_t *)L"\u263A");
    
}

Player::Player() : Entity((wchar_t *)L"\u263A")
{
    
    // Entity((wchar_t *)L"\u263A");
    H = 0.0;
    S = 0.0;
    V = 1.0;
    
}

Goblin::Goblin() : Entity((wchar_t *)L"\uFFF7")
{
    
    //  Entity((wchar_t *)L"\uFFF7");
    H = 30.0;
    S = 0.58;
    V = 0.40;
    
}

