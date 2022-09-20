#define OLC_PGE_APPLICATION
#include "pixelGame/olcPixelGameEngine.h"

using namespace std;

class Game : public olc::PixelGameEngine
{
public:
	Game()
	{
		sAppName = "Game";
	}
    struct sNode
    {
        bool bObstacle;
        bool bVisited;
        float fLocalGoal;
        float fGlobalGoal;
        int x;
        int y;
        vector<sNode*> vecNeighbours;
        sNode* parent;
    }
    sNode* nodes = nullptr;
    int nMapWidth = 16;
    int nMapHeight = 16;


public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
        nodes = new sNode[nMapWidth * nMapHeight];
        for(int x = 0; x < nMapWidth; x++)
        {
            for(int y = 0; y < nMapHeight; y++)
            {
                nodes[nMaptWidth * y + x].x = x;
                nodes[nMaptWidth * y + x].y = y;
                nodes[nMaptWidth * y + x].bObstacle = false;
                nodes[nMaptWidth * y + x].bVisited = false;
                nodes[nMaptWidth * y + x].parent = nullptr;
                nodes[nMaptWidth * y + x].fLocalGoal = 0.0f;
                nodes[nMaptWidth * y + x].fGlobalGoal = 0.0f;
            }
        }

        for(int x = 0; x < nMapWidth; x++)
        {
            for(int y = 0; y < nMapHeight; y++)
            {
                if(y > 0)
                    nodes[nMapWidth * y + x].vecNeighbours.push_back(&nodes[nMapWidth * (y - 1) + x]);
                if(x > 0)
                    nodes[nMapWidth * y + x].vecNeighbours.push_back(&nodes[nMapWidth * y + (x - 1)]);
                if(y < nMapHeight - 1)
                    nodes[nMapWidth * y + x].vecNeighbours.push_back(&nodes[nMapWidth * (y + 1) + x]);
                if(x < nMapWidth)
                    nodes[nMapWidth * y + x].vecNeighbours.push_back(&nodes[nMapWidth * y + (x + 1)]);

                if(y > 0 && x > 0)
                    nodes[nMapWidth * y + x].vecNeighbours.push_back(&nodes[nMapWidth * (y - 1) + (x - 1)]);
                if(y > 0 && x < nMapWidth - 1)
                    nodes[nMapWidth * y + x].vecNeighbours.push_back(&nodes[nMapWidth * (y - 1) + (x + 1)]);
                if(y < nMapHeight - 1 && x > 0)
                    nodes[nMapWidth * y + x].vecNeighbours.push_back(&nodes[nMapWidth * (y + 1) + (x - 1)]);
                if(y < nMapHeight && x < nMapWidth - 1)
                    nodes[nMapWidth * y + x].vecNeighbours.push_back(&nodes[nMapWidth * (y + 1) + (x + 1)]);
            }
        }

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// called once per frame
        int blockSize = 7;

        for(int x = 0; x < nMapWidth; x++)
        {
            for(int y = 0; y < nMapHeight; y++)
            {
                FillRect(, )
            }
        }


		return true;
	}
};


int main()
{
	Game game;
	if (game.Construct(160, 160, 6, 6))
		game.Start();

	return 0;
}
