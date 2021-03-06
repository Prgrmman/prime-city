#include "gameManager.h"

GameManager::GameManager()
{
    perlinSize = 10;
    png = PerlinNoiseGenerator(10, 10, 1);
    chunkSize = 512;
    renderRadius = 5;
    updateCurrentChunks();
    enemies.push_back(std::make_shared<Enemy>());
}
GameManager::GameManager(int inputChunkSize, int inputRenderRadius, int inputPerlinSize)
{
    chunkSize = inputChunkSize;
    renderRadius = inputRenderRadius;
    perlinSize = inputPerlinSize;
    png = PerlinNoiseGenerator(perlinSize, perlinSize, 1);
    updateCurrentChunks();
}

void GameManager::reactToMouseMovement(double theta)
{
    player.updateAngles(theta);
    player.updateSphericalDirectionBasedOnAngles();
    player.setVelocity(wKey, aKey, sKey, dKey, rKey, cKey);
}
void GameManager::reactToMouseClick()
{
    createMissile();
}


void GameManager::draw() const
{
    for(auto &c : currentChunks)
    {
        c->draw();
    }

    for(auto &e : enemies)
    {
        e->draw();
    }
    // Draw a red square under the player for debug
    /*glDisable(GL_CULL_FACE);
    Vector3 v = player.getLocation();
    glBegin(GL_QUADS);
    glColor4f(1,0,0,1);
    glVertex3f(v.x + 20,3, v.z + 20);
    glVertex3f(v.x - 20,3, v.z + 20);
    glVertex3f(v.x - 20,3, v.z - 20);
    glVertex3f(v.x + 20,3, v.z - 20);

    glEnd();
    glEnable(GL_CULL_FACE);*/
    // Make current chunk red for debug
    /*Point2D p = player.getCurrentChunkCoords();
    glBegin(GL_QUADS);
    glColor4f(1,0,0,1);
    glVertex3f(chunkSize*p.x,1, chunkSize*p.z);
    glVertex3f(chunkSize*p.x,1, chunkSize*p.z + chunkSize);
    glVertex3f(chunkSize*p.x + chunkSize,1, chunkSize*p.z + chunkSize);
    glVertex3f(chunkSize*p.x + chunkSize,1, chunkSize*p.z);

    glEnd();*/
}

void GameManager::tick()
{
    // The player moves
    player.tick();

    // Check for the player hitting a building
    Point2D curPlayerChunk = player.whatChunk();
    std::shared_ptr<Chunk> c = allSeenChunks[pointToInt(curPlayerChunk)];
    player.checkCollisionsAndCorrect(*c, 5);

    // If the player is entering a different chunk
    if(curPlayerChunk != player.getCurrentChunkCoords())
    {
        //std::cout << pointToInt({player.whatChunk().x, player.whatChunk().z}) << std::endl;
        //std::cout << player.getLocation().x << ", " << player.getLocation().z << std::endl;
        updateCurrentChunks();
    }

    // The missiles move
    /*for(std::shared_ptr<Missile> m : missiles)
    {
        m->tick();
    }
    checkMissiles();*/
}

Player GameManager::getPlayer() const
{
    return player;
}
bool GameManager::getWKey()
{
    return wKey;
}
bool GameManager::getAKey()
{
    return aKey;
}
bool GameManager::getSKey()
{
    return sKey;
}
bool GameManager::getDKey()
{
    return dKey;
}
bool GameManager::getRKey()
{
    return rKey;
}
bool GameManager::getCKey()
{
    return cKey;
}

void GameManager::setWKey(bool input)
{
    wKey = input;
    player.setVelocity(wKey, aKey, sKey, dKey, rKey, cKey);
}
void GameManager::setAKey(bool input)
{
    aKey = input;
    player.setVelocity(wKey, aKey, sKey, dKey, rKey, cKey);
}
void GameManager::setSKey(bool input)
{
    sKey = input;
    player.setVelocity(wKey, aKey, sKey, dKey, rKey, cKey);
}
void GameManager::setDKey(bool input)
{
    dKey = input;
    player.setVelocity(wKey, aKey, sKey, dKey, rKey, cKey);
}
void GameManager::setRKey(bool input)
{
    rKey = input;
    player.setVelocity(wKey, aKey, sKey, dKey, rKey, cKey);
}
void GameManager::setCKey(bool input)
{
    cKey = input;
    player.setVelocity(wKey, aKey, sKey, dKey, rKey, cKey);
}

// ============================
//
//       Managing Chunks
//
// ============================
double GameManager::getPerlinValue(Point2D p)
{
    return png.getPerlinNoise()[mod(p.x, perlinSize)][mod(p.z, perlinSize)];
}

void GameManager::updateCurrentChunks()
{
    player.setCurrentChunkCoords(player.whatChunk());

    // Update the list of current chunks
    currentChunks = std::vector<std::shared_ptr<Chunk>>();
    std::vector<Point2D> chunksInRadius = getChunksAroundPointByPoint(player.getCurrentChunkCoords(), renderRadius);
    for(Point2D p : chunksInRadius)
    {
        int index = pointToInt(p);
        if(allSeenChunks.count(index) == 0) // if the chunk has never been seen before
        {
            allSeenChunks[index] = std::make_shared<Chunk>(p, chunkSize, getPerlinValue(p));
        }
        currentChunks.push_back(allSeenChunks[index]);
    }
}
std::shared_ptr<Chunk> GameManager::pointToChunk(Point p)
{
    int chunkIndex = pointToInt({(int)floor(p.x / chunkSize), (int)floor(p.z / chunkSize)});
    if(allSeenChunks.count(chunkIndex) == 0)
    {
        chunkIndex = 0; // If it's out of bounds, just check the 0 chunk for now. can't hurt.
    }
    return allSeenChunks[chunkIndex];
}


// =========================
//
//        Missiles
//
// =========================
void GameManager::createMissile()
{
    Point location = {player.getLocation().x, player.getLocation().y, player.getLocation().z};
    Point velocity = {player.getLookingAt().x - location.x,
                      player.getLookingAt().y - location.y,
                      player.getLookingAt().z - location.z};
    //missiles.push_back(std::make_shared<Missile>(Missile(location, 10, velocity, 5)));
}
void GameManager::checkMissiles()
{
    /*int L = missiles.size();
    int i = 0;
    while(i < L)
    {
        std::shared_ptr<Missile> m = missiles[i];
        Point playerLocation = {player.getLocation().x, player.getLocation().y, player.getLocation().z};

        // If the missile went out of bounds without hitting anything, remove it
        if(m->isOutOfBounds(playerLocation, renderRadius*chunkSize))
        {
            missiles.erase(missiles.begin() + i);
            L -= 1;
            i--;
        }
        // Check if the missile is hitting a building
        else
        {
            std::shared_ptr<Chunk> c = pointToChunk(m->getLocation());
            for(Building &b : c->getBuildings())
            {
                if(b.correctCollision(m->getLocation(), m->getRadius()))
                {
                    missiles.erase(missiles.begin() + i);
                    b.giveSolidsColor();
                    L -= 1;
                    i--;
                    break;
                }
            }
        }
        i++;
    }*/
}




// Camera
Vector3 GameManager::getCameraLocation() const
{
    return player.getLocation();
}
Vector3 GameManager::getCameraLookingAt() const
{
    return player.getLookingAt();
}
Vector3 GameManager::getCameraUp() const
{
    return player.getUp();
}

int mod(int a, int m)
{
    int x = a % m;
    if(x < 0)
    {
        return x + m;
    }
    return x;
}

void GameManager::printPlayerBuildingDebug()
{
    Vector3 v = player.getLocation();
    std::cout << std::endl<< "Player location: " << v.x << "," << v.y << "," << v.z << std::endl;
    Point2D curPlayerChunk = player.whatChunk();
    std::shared_ptr<Chunk> c = allSeenChunks[pointToInt(curPlayerChunk)];
    /*for(Building &b : c->getBuildings())
    {
        for(std::shared_ptr<Solid> s : b.getSolids())
        {
            s->printDebugStats();
        }
        std::cout<<"End of Building" << std::endl;
    }*/
}

void GameManager::drawPlayerDirection(double x, double y) const
{
    int compassRadius = 25;

    // Draw the text
    glColor4f(0.0, 0.0, 0.0, 1.0);
    std::string direction = player.getDirectionString();
    glRasterPos2i(x - (4 * direction.length()), y - 3);
    for (const char &letter : direction)
    {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, letter);
    }

    // Draw the red line
    glColor4f(1.0, 0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    glVertex2f(x, y);
    double theta = player.getXZAngle();
    glVertex2f(x + compassRadius*cos(theta), y - compassRadius*sin(theta));
    glEnd();

    // Draw the circle
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glBegin(GL_TRIANGLE_FAN);
    int smoothness = 24;
    glVertex2f(x, y); // center
    for(int i = 0; i < smoothness + 1; i++)
    {
        glVertex2f(x + compassRadius*cos(i*2*PI/smoothness), y + compassRadius*sin(i*2*PI/smoothness));
    }
    glEnd();
}