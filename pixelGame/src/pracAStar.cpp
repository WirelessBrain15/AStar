#define OLC_PGE_APPLICATION
#include "pixelGame/olcPixelGameEngine.h"

using namespace std;

class pathFinding : public olc::PixelGameEngine
{
public:
	pathFinding()
	{
		sAppName = "A-Star";
	}

private:
    struct sNode
    {
        bool bObstacle;
        bool bVisited;
        int x;
        int y;
        float fLocalGoal;
        float fGlobalGoal;
        vector<sNode*> vecNeighbours;
        sNode* parent;
    };

    sNode* nodes = nullptr;
    int nMapHeight = 16;
    int nMapWidth = 16;

    sNode* nodeStart = nullptr;
    sNode* nodeEnd = nullptr;

protected:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here

        nodes = new sNode[nMapWidth * nMapHeight];
        for (int x = 0; x < nMapWidth; x++)
			for (int y = 0; y < nMapHeight; y++)
			{
				nodes[y * nMapWidth + x].x = x; // ...because we give each node its own coordinates
				nodes[y * nMapWidth + x].y = y;
				nodes[y * nMapWidth + x].bObstacle = false;
				nodes[y * nMapWidth + x].parent = nullptr;
				nodes[y * nMapWidth + x].bVisited = false;
			}

        // connections
        for(int x = 0; x < nMapWidth; x++)
            for(int y = 0; y < nMapHeight; y++)
            {
                if(y > 0)
                    nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[(y - 1) * nMapWidth + x]);
                if(x > 0)
                    nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[y * nMapWidth + x - 1]);
                if(y < nMapHeight - 1)
                    nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 1) * nMapWidth + x]);
                if(x < nMapWidth - 1)
                    nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[y * nMapWidth + x + 1]);

                
                if(y > 0 && x > 0)
                    nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[(y-1)*nMapWidth + x - 1]);
                if(y > 0 && x < nMapWidth - 1)
                    nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[(y-1)*nMapWidth + x + 1]);
                if(y < nMapHeight - 1 && x > 0)
                    nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[(y+1)*nMapWidth + x - 1]);
                if(y < nMapHeight - 1 && x < nMapWidth - 1)
                    nodes[y*nMapWidth + x].vecNeighbours.push_back(&nodes[(y+1)*nMapWidth + x + 1]);
            
            }

        nodeStart = &nodes[(nMapHeight/2) * nMapWidth + 1];
        // nodeStart = &nodes[1];
        // 
        nodeEnd = &nodes[(nMapHeight/2) * nMapWidth + nMapWidth - 2];
        // nodeEnd = &nodes[0];

		return true;
	}

    bool Solve_AStar()
    {
        // reset navigation graph - default all nodes
        for(int x = 0; x < nMapWidth; x++)
            for(int y = 0; y < nMapHeight; y++)
            {
                nodes[y * nMapWidth + x].bVisited = false;
                nodes[y * nMapWidth + x].fGlobalGoal = INFINITY;
                nodes[y * nMapWidth + x].fLocalGoal = INFINITY;
                nodes[y * nMapWidth + x].parent = nullptr;
            }

        auto distance = [](sNode* a, sNode* b)
        {
            return sqrtf(pow((a->x - b->x),2) + pow((a->y - b->y),2));
        };

        auto heuristic = [distance](sNode* a, sNode* b)
        {
            return distance(a, b);
        };

        sNode* nodeCurrent = nodeStart;
        nodeStart->fLocalGoal = 0.0f;
        nodeStart->fGlobalGoal = heuristic(nodeStart, nodeEnd);

        // cout << nodeStart->fLocalGoal << " || " << nodeStart->fGlobalGoal << endl;         
    
        list<sNode*> listNotTestedNodes;
        listNotTestedNodes.push_back(nodeStart);

        while(!listNotTestedNodes.empty() && nodeCurrent != nodeEnd)
        {
            // sort untested nodes by goal
            listNotTestedNodes.sort([](const sNode* lhs, const sNode* rhs){return lhs->fGlobalGoal < rhs->fGlobalGoal;});

            while(!listNotTestedNodes.empty() && listNotTestedNodes.front()->bVisited)
                listNotTestedNodes.pop_front();

            if(listNotTestedNodes.empty())
                break;

            nodeCurrent = listNotTestedNodes.front();
            nodeCurrent->bVisited = true;

            for(auto nodeNeighbour : nodeCurrent->vecNeighbours)
            {
                if(!nodeNeighbour->bVisited && nodeNeighbour->bObstacle == 0)
                    listNotTestedNodes.push_back(nodeNeighbour);

                float fPossibleLowerGoal = nodeCurrent->fLocalGoal + distance(nodeCurrent, nodeNeighbour);

                if(fPossibleLowerGoal < nodeNeighbour->fLocalGoal)
                {
                    nodeNeighbour->parent = nodeCurrent;
                    nodeNeighbour->fLocalGoal = fPossibleLowerGoal;

                    nodeNeighbour->fGlobalGoal = nodeNeighbour->fLocalGoal + heuristic(nodeNeighbour, nodeEnd);
                }
            }
        }
        return true;
    }

	bool OnUserUpdate(float fElapsedTime) override
	{
		// called once per frame
		
        int nNodeBorder = 2;
        int nNodeSize = 9;

        // cursor position
        int nSelectedNodeX;
        int nSelectedNodeY;
        if(GetMouse(0).bReleased)
        {
            nSelectedNodeX = GetMouseX()/9;
            nSelectedNodeY = GetMouseY()/9;
            // cout << nSelectedNodeX << " || " << nSelectedNodeY << endl;
        
            if(nSelectedNodeX >= 0 && nSelectedNodeX < nMapWidth)
                if(nSelectedNodeY >= 0 && nSelectedNodeY < nMapHeight)
                {
                    if(GetKey(olc::Key::SHIFT).bHeld)
                        nodeStart = &nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX];

                    else if(GetKey(olc::Key::CTRL).bHeld)
                        nodeEnd = &nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX];
                    
                    else
                        nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX].bObstacle = !nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX].bObstacle;

                    Solve_AStar();
                }
        
        }

        FillRect(0,0, ScreenWidth(), ScreenHeight(), olc::DARK_BLUE);

        // Draw Connections First - lines from this nodes position to its
		// connected neighbour node positions
		for (int x = 0; x < nMapWidth; x++)
			for (int y = 0; y < nMapHeight; y++)
			{
				for (auto n : nodes[y*nMapWidth + x].vecNeighbours)
				{
					DrawLine(x*nNodeSize + nNodeSize / 2, y*nNodeSize + nNodeSize / 2,
						n->x*nNodeSize + nNodeSize / 2, n->y*nNodeSize + nNodeSize / 2, olc::CYAN);
				}
			}


        // draw nodes
       for (int x = 0; x < nMapWidth; x++)
			for (int y = 0; y < nMapHeight; y++)
			{
	
				FillRect(x*nNodeSize + nNodeBorder, y*nNodeSize + nNodeBorder, 
					7,7, 
					nodes[y * nMapWidth + x].bObstacle ? olc::WHITE : olc::BLUE);

				if (nodes[y * nMapWidth + x].bVisited)
					FillRect(x*nNodeSize + nNodeBorder, y*nNodeSize + nNodeBorder, 7,7, olc::BLUE);

				if(&nodes[y * nMapWidth + x] == nodeStart)
					FillRect(x*nNodeSize + nNodeBorder, y*nNodeSize + nNodeBorder, 7,7, olc::DARK_GREEN);

				if(&nodes[y * nMapWidth + x] == nodeEnd)
					FillRect(x*nNodeSize + nNodeBorder, y*nNodeSize + nNodeBorder, 7,7, olc::RED);						
				
			}

        if(nodeEnd != nullptr)
        {
            sNode* p = nodeEnd;
            while(p->parent != nullptr)
            {
                DrawLine(p->x*nNodeSize + nNodeSize/2, p->y*nNodeSize + nNodeSize/2,
                p->parent->x*nNodeSize + nNodeSize/2, p->parent->y*nNodeSize + nNodeSize/2, olc::YELLOW);

                p = p->parent;
            }
        }


		return true;
	}
};


int main()
{
	pathFinding game;
	if (game.Construct(160, 160, 6, 6))
		game.Start();

	return 0;
}
