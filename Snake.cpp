#include <iostream>
#include <vector>
#include <algorithm>
#include <deque>
#include <utility>
#include <algorithm>
#include <ncurses.h>
using namespace std;
using Coordinate = pair<int, int>;




// 칸의 정보를 구별하는 숫자, 맵 클래스에서 사용
enum CellValue {
    EMPTY_SPACE = 0,
    WALL = 1,
    IMMUNE_WALL,
    HEAD = 3,
    TAIL = 4,
    GROWTH_ITEM = 5,
    POISON_ITEM = 6,
    GATE = 7,
    CRUSHED_HEAD = 8
};

// 아이템 클래스
class Item {

private:
    CellValue itemType; // 아이템의 종류
    int lifeTime;       // 아이템의 수명
    Coordinate itemPos; // 아이템의 위치

public:
    Item(CellValue itemType, int lifeTime, Coordinate itemPos) {
        this->itemType = itemType;
        this->lifeTime = lifeTime;
        this->itemPos = itemPos;
    }

    void update() {
        if (lifeTime > 0) {
            lifeTime--;
        }
    }

    int getLifetime() {
        return lifeTime;
    }

    Coordinate getItemPos() {
        return itemPos;
    }

    CellValue getItemType() {
        return itemType;
    }
};
// 맵 클래스
class GameMap
{
private:
    int width;      // 맵의 가로 길이
    int height;     // 맵의 세로 길이
    int** mapArray; // 맵의 데이터를 담을 2차원 배열 
    vector<string> mapPresets; // 맵의 프리셋들을 담을 벡터

    vector<Item*> spawnedItems;  // 생성된 아이템들 주소 벡터 

    int growthItemNum; // 현재 생성된 성장아이템의 수
    int poisonItemNum;  // 현재 생성된 독 아이템의 수

    int maxGrowthItem;  // 최대 성장 아이템의 수
    int maxPoisonItem;  // 최대 독 아이템의 수

    // 미션 변수들
    int currentStage;
    int currentLength;
    int maxLength;
    int growthItemsConsumed;
    int poisonItemsConsumed;
    int gatesUsed;
    bool alreadyUpdated;   // 추가한 변수 - 아이템 소모량 업데이트 여부를 체크하는 변수(이미 업데이트했다면 true)

    // ItemSpawner* _growthItemSpawner; // 성장 아이템 생성기 포인터

    // 맵 배열을 입력한 크기로 할당
    void allocateMapArray(int newWidth, int newHeight) {
        mapArray = new int*[newHeight];
        for (int i = 0; i < newHeight; ++i) {
            mapArray[i] = new int[newWidth];
        }
    }

    // 맵 배열 할당 해제
    void deallocateMapArray() {
        if (height != 0) {

            for (int i = 0; i < height; i++) {
                delete[] mapArray[i];
            }
            delete[] mapArray;
            mapArray = nullptr;
        } 
    }

public:
    // 동적 할당을 포함한 생성자와 소멸자
    GameMap(int presetIndex) : currentStage(1), currentLength(3), maxLength(3),
                               growthItemsConsumed(0), poisonItemsConsumed(0), gatesUsed(0){

        height = 0;
        width = 0;
        alreadyUpdated = false;
        // 프리셋 1
        mapPresets.push_back("211111111111111111112\n"
                             "100000000000000000001\n"
                             "100000000000100000001\n"
                             "100000000000100000001\n"
                             "100000000000100000001\n"
                             "100000000000100000001\n"
                             "100000000000100000001\n"
                             "100011111111100000001\n"
                             "100000000000000000001\n"
                             "100000000000000000001\n"
                             "100010000000034400001\n"
                             "100010000000000000001\n"
                             "100010000000000000001\n"
                             "211121110001111100001\n"
                             "100010000000000100001\n"
                             "100010000000000100001\n"
                             "100010000000000100001\n"
                             "100000000000000100001\n"
                             "100000000000000100001\n"
                             "100000000000000000001\n"
                             "211111111111111111112\n");


        // 프리셋 2
        mapPresets.push_back("211111111121111111112\n"
                             "100000000010000000001\n"
                             "100000000010000000001\n"
                             "100000000010000000001\n"
                             "100000000010000000001\n"
                             "100000000010000000001\n"
                             "100000000010000000001\n"
                             "100000000010000000001\n"
                             "100000000010000000001\n"
                             "100000000000000000001\n"
                             "101111111111111111101\n"
                             "100000000000000000001\n"
                             "100000000000000000001\n"
                             "100030000000000000001\n"
                             "100040000000000000001\n"
                             "100040000000000000001\n"
                             "100000000000000000001\n"
                             "100000000000000000001\n"
                             "100000000000000000001\n"
                             "100000000000000000001\n"
                             "211111111111111111112\n");

        // 프리셋 3
        mapPresets.push_back("211111111111111111112\n"
                             "100000000000000000001\n"
                             "100000000010000000001\n"
                             "100000000010000000001\n"
                             "100001111121111100001\n"
                             "100000000010000000001\n"
                             "100000000010000000001\n"
                             "100000000010000000001\n"
                             "100000000010000000001\n"
                             "100000000010000000001\n"
                             "100000000000000000001\n"
                             "100044300000000000001\n"
                             "100000000000000000001\n"
                             "100000010000010000001\n"
                             "100000010000010000001\n"
                             "100000010000010000001\n"
                             "100000010000010000001\n"
                             "100000010000010000001\n"
                             "100000010000010000001\n"
                             "100000010000010000001\n"
                             "211111121111121111112\n");
                    
        // 프리셋 4
        mapPresets.push_back("211121111111111111112\n"
                             "100010000000000000001\n"
                             "100010000000000000001\n"
                             "100010000000000000001\n"
                             "100011111111111110001\n"
                             "100000000010000010001\n"
                             "100000000010000010001\n"
                             "100000000010000010001\n"
                             "100010000000000000001\n"
                             "100010000000000000001\n"
                             "100010000000000000001\n"
                             "100011111111111111112\n"
                             "100000000000000000001\n"
                             "100000000000000000001\n"
                             "100000000000000000001\n"
                             "211111111000000300001\n"
                             "100000000000000400001\n"
                             "100000000000000400001\n"
                             "100000000000000000001\n"
                             "100000000000000000001\n"
                             "211111111111111111112\n");


        applyPreset(presetIndex);


        // _growthItemSpawner = growthItemSpawner;
        growthItemNum = 0;
        poisonItemNum = 0;
        maxGrowthItem = 3;
        maxPoisonItem = 3;


    }
    ~GameMap() {
        deallocateMapArray();
    }

    int getWidth() {
        return width;
    }

    int getHeight() {
        return height;
    }

    // 프리셋을 선택하여 적용하는 멤버 함수
    void applyPreset(int presetIndex) {
        // 인덱스 예외처리
        if (presetIndex < 0 || presetIndex >= mapPresets.size()) {
            cout << "입력한 인덱스의 프리셋이 존재하지 않습니다." << endl;
            return;
        }



        const string& preset = mapPresets[presetIndex]; // 맵 배열을 구성할 프리셋 스트링 로드

        // 가로 길이와 세로 길이를 계산
        int newWidth = preset.find('\n');  // 첫 번째 개행 문자의 위치를 찾아 가로 길이로 설정
        int newHeight = count(preset.begin(), preset.end(), '\n') + 1;  // 개행 문자의 수 + 1을 세로 길이로 설정


        // 기존 맵 배열을 해제
        deallocateMapArray();

        // 새 크기로 맵 배열을 할당
        width = newWidth;
        height = newHeight;

        allocateMapArray(width, height);

        // 프리셋을 맵 배열에 설정
        int row = 0, col = 0;
        for (char ch : preset) {
            if (ch == '\n') {
                row++;
                col = 0;
            } else {
                mapArray[row][col] = ch - '0'; // 문자열을 정수로 변환
                col++;
            }
        }
    }

    void getCenterPosition(int &startY, int &startX) const {
        int maxHeight, maxWidth;
        getmaxyx(stdscr, maxHeight, maxWidth);
        startY = (maxHeight - height) / 2;
        startX = (maxWidth - width) / 2;
    }

    // 현재 맵 출력
    void print(WINDOW *win) const { // Modified to take WINDOW pointer
        const wchar_t symbols[] = {' ',  '#', 'X', '%', '~', '+', '-', '@', '*'};    // 숫자 대신 사용할 문자
        int startY, startX;
        getCenterPosition(startY, startX); // Calculate center position

        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                mvwaddch(win, startY + i, startX + j, symbols[mapArray[i][j]]); // Use window for printing
            }
        }
        wrefresh(win);        // Refresh the window
    }

    void setAlreadyUpdate(bool state) {
        alreadyUpdated = state;
    }

    // 뱀의 초기 위치를 리턴 (머리부터 이어지는 3개의 순서쌍을 리턴)
    vector<Coordinate> getInitialSnakePosition() const {
        vector<Coordinate> snakePositions;
        int count = 0;
        int curRow, curCol;
        // 우선 머리를 찾음. 
        bool flag = true;
        for (int i = 0; i < height && flag; ++i) {
            for (int j = 0; j < width && flag; ++j) {
                if (mapArray[i][j] == HEAD) {
                    snakePositions.push_back({i, j});       // 머리 위치 목록에 추가
                    flag = false;
                    count = 1;
                    curRow = i;
                    curCol = j;
                }
            }
        }
        // 머리 다음에 이어지는 몸통을 찾아서 목록에 추가.
        while (count < 3) {
            // 아래 확인
            if (curRow + 1 < height && mapArray[curRow + 1][curCol] == TAIL){
                curRow++;
                snakePositions.push_back({curRow, curCol});
            } 
            // 위 확인
            else if (curRow - 1 >= 0 && mapArray[curRow - 1][curCol] == TAIL) {
                curRow--;
                snakePositions.push_back({curRow, curCol});
            }   
            // 왼쪽 확인         
            else if (curCol - 1 >= 0 && mapArray[curRow][curCol - 1] == TAIL) {
                curCol--;
                snakePositions.push_back({curRow, curCol});
            }       
            // 오른쪽 확인     
            else if (curCol + 1 < width && mapArray[curRow][curCol + 1] == TAIL) {
                curCol++;
                snakePositions.push_back({curRow, curCol});
            }
            count++;
        }
        return snakePositions; 
    }

    // 맵 배열을 리턴
    int** getMapArray() const {
        return this->mapArray;
    }

    // 특정 좌표의 값을 변경하는 함수
    void setCellValue(int row, int col, CellValue value) {
        if (row >= 0 && row < height && col >= 0 && col < width) {
            mapArray[row][col] = value;
        }
    }

    // 특정 좌표의 값을 가져오는 함수
    CellValue getCellValue(int row, int col) const {
        if (row >= 0 && row < height && col >= 0 && col < width) {
            return static_cast<CellValue>(mapArray[row][col]);
        }
        return EMPTY_SPACE; // 기본값
    }

    // 아이템을 추가함. 추가에 성공했으면 true반환.
    bool addItem(Item* item) {
        CellValue itemType = item->getItemType();
        if (itemType == GROWTH_ITEM) {
            if (growthItemNum >= maxGrowthItem) {   // 이미 최대치로 생성되어 있으면 패스
                growthItemNum = maxGrowthItem;
                return false;
            }
            else growthItemNum++;
        }
        else {
            if (poisonItemNum >= maxPoisonItem) {
                poisonItemNum = maxPoisonItem;
                return false;
            }
            else poisonItemNum++;
        }
        spawnedItems.push_back(item);
        return true;
    }

    // 아이템들을 관리함(모든 아이템들의 수명을 업데이트하고 수명이 다한 건 삭제함)
    void manageItems() {
        if (spawnedItems.empty()) return;   // 현재 아이템이 없다면 패스
        // 모든 원소들의 수명을 업데이트하고, 수명이 다한 원소는 삭제함.
        for (int i = spawnedItems.size() - 1; i >= 0; --i) {
            Item* itemPointer = spawnedItems[i];
            itemPointer->update();
            
            // 삭제 안되고 있음. 수정 해야함.
            if (itemPointer->getLifetime() == 0) {
                Coordinate itemPos = itemPointer->getItemPos();
                if (getCellValue(itemPos.first, itemPos.second) == itemPointer->getItemType()) 
                    setCellValue(itemPos.first, itemPos.second, EMPTY_SPACE);
                
                (itemPointer->getItemType() == GROWTH_ITEM)? growthItemNum--:poisonItemNum--;
                delete itemPointer;
                spawnedItems.erase(spawnedItems.begin() + i);
            }
        }
    }

    // 벽 위치를 가져오는 함수
    vector<Coordinate> getWallPositions() const {
        vector<Coordinate> wallPositions;
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                if (mapArray[i][j] == WALL) {
                    wallPositions.push_back({i, j});
                }
            }
        }
        return wallPositions;
    }

    //스테이지 미션 초기화
    void resetForNewStage(int stage) {
        currentStage = stage;
        currentLength = 3;
        maxLength = 3;
        growthItemsConsumed = 0;
        poisonItemsConsumed = 0;
        gatesUsed = 0;
        applyPreset(stage - 1);  // 스테이지 번호에 맞는 프리셋 적용
    }

    //스테이지 미션 만족 조건
    bool checkMissionCompleted() {
        return maxLength >= 7 &&
               growthItemsConsumed >= 5 &&
               poisonItemsConsumed >= 3 &&
               gatesUsed >= 3;
    }
    
    //미션 갱신 - 파라미터 타입 enum으로 변경함.
    void updateMissionStatus(CellValue itemType) {
        if (alreadyUpdated) return;  // 이미 업데이트 했다면 패스
        if (itemType == GROWTH_ITEM) {
            growthItemsConsumed++;
            currentLength++;
            if (currentLength > maxLength) {
                maxLength = currentLength;
            }
            alreadyUpdated = true;    
        } else if (itemType == POISON_ITEM) {
            poisonItemsConsumed++;
            currentLength--;
            alreadyUpdated = true;
        } else if (itemType == GATE) {
            gatesUsed++;
            alreadyUpdated = true;
        }
    }

    int getCurrentStage() const {
        return currentStage;
    }

    int getCurrentLength() const {
        return currentLength;
    }

    int getMaxLength() const {
        return maxLength;
    }

    int getGrowthItemsConsumed() const {
        return growthItemsConsumed;
    }

    int getPoisonItemsConsumed() const {
        return poisonItemsConsumed;
    }

    int getGatesUsed() const {
        return gatesUsed;
    }
};


// 게이트 생성기 클래스
class GateSpawner {

private:
    bool active;        // 활성화 여부
    GameMap& _gameMap;   // 게임 맵 참조 변수
    vector<Coordinate> wallPos; // 게이트를 생성할 수 있는 벽의 위치들
    int spawnCycle;         // 소멸부터 생성까지의 주기
    int countCycle;          // 생성 주기를 세는 변수
    int duration;           // 게이트 지속시간
    int countDuration;      // 게이트 지속시간을 세는 변수
    bool isUsing;           // 사용 중일 때를 판별하는 변수
    int timeToUse;          // 사용할 시간(몸의 길이)를 넣을 변수
    bool isOpen;            // 열려 있을 때를 판별하는 변수
    
    Coordinate gatePos_1, gatePos_2;    // 게이트의 위치로 사용할 변수

public:
    GateSpawner(GameMap& gameMap) : _gameMap(gameMap) {
        active = false;
        wallPos = _gameMap.getWallPositions();
        spawnCycle = 10;
        countCycle = spawnCycle;
        duration = 20;
        countDuration = duration;
        timeToUse = 0;
        isOpen = false;
    }

    void resetForNewState() {      // 스테이지 변경 후 초기화
        active = false;
        wallPos = _gameMap.getWallPositions();
        spawnCycle = 10;
        countCycle = spawnCycle;
        duration = 20;
        countDuration = duration;
        timeToUse = 0;
        isOpen = false;
    }

    void update() {

        if (!active) return;    // 비활성화시 혹은 사용중이면 패스

        if (isOpen) {       // 게이트가 열려있을 때
            if (countDuration > 0) { // 게이트 지속중일때
                countDuration--;
                if (timeToUse > 0) timeToUse--;
            }
            else {                   // 게이트 지속시간이 끝났을 때
                if (timeToUse > 0) {
                    timeToUse--;
                    return;    // 사용중이면 소멸 연장
                }
                countDuration = duration;
                removeGate();
                isOpen = false;
            }
        }
        else {              // 게이트가 닫혀있을 때
            if (countCycle > 0) {   // 재생성 대기시간 감소
                countCycle--;
            }
            else {                  // 재생성 대기시간이 끝났을 때
                countCycle = spawnCycle;
                isOpen = true;
                spawnGate();
            }
        }
    }

    void spawnGate() {
        gatePos_1 = getRandomPosition();
        gatePos_2 = getRandomPosition();
        _gameMap.setCellValue(gatePos_1.first, gatePos_1.second, GATE);
        _gameMap.setCellValue(gatePos_2.first, gatePos_2.second, GATE);
        // cout << "gate Spawned" << endl;
    }

    void removeGate() {
        _gameMap.setCellValue(gatePos_1.first, gatePos_1.second, WALL);
        _gameMap.setCellValue(gatePos_2.first, gatePos_2.second, WALL); 
        isOpen = false;
    }

    void setActive(bool state) {
        active = state;
    }

    void setTimeToUse(int time) {
        timeToUse = time;
    }

    int getTimeToUse() {
        return timeToUse;
    }

    Coordinate getGatePos(int gateNum) {
        if (gateNum == 1) return gatePos_1;
        else return gatePos_2;
    }
    
    // wall 중 무작위로 하나의 위치를 반환
    Coordinate getRandomPosition() {
        int maximumLoop = wallPos.size();
        int randIdx; 
        while (maximumLoop--)
        {
            randIdx = rand() % maximumLoop;
            Coordinate pos = wallPos[randIdx];
            if (_gameMap.getCellValue(pos.first, pos.second) == WALL)
                return pos;
        }
        return Coordinate(-1, -1);  // 만약 생성할 수 있는 위치가 없으면 예외 출력.
    }

};


enum Direction{
    UP,
    RIGHT,
    DOWN,
    LEFT
};

// 뱀 클래스
class Snake {

private:
    deque<Coordinate> body;     //뱀의 머리부터 꼬리까지의 좌표가 담긴 디큐
    GameMap& gameMap; // GameMap 객체 참조
    GateSpawner& gateSpawner;   // 게이트 생성기 참조
    Direction curDirection; // 현재 이동방향
    CellValue lastConsumedItem; // 마지막 아이템 - 타입 enum으로 바꿈.

public:
    // 뱀의 초기 위치를 벡터로 받아 설정하는 생성자
    Snake(GameMap& gameMap, GateSpawner& gateSpawner) : gameMap(gameMap), gateSpawner(gateSpawner){
        auto initialPositions = gameMap.getInitialSnakePosition();
        for (const auto& pos : initialPositions) {
            body.push_back(pos);
        }
    }

    void resetForNewStage() {
        body.clear();
        auto initialPositions = gameMap.getInitialSnakePosition();
        for (const auto& pos : initialPositions) {
            body.push_back(pos);
        }
    }


    // 뱀을 이동시키는 함수 (간단히 방향에 따라 한 칸 이동, 게임 오버 여부를 리턴)
    bool moveTest() {
        Coordinate head = body.front(); // 현재 머리 위치
        Coordinate nextHead = head; // 다음에 이동할 머리의 위치로 사용할 변수
        switch (curDirection)
        {
            case UP: nextHead.first -= 1; break;
            case DOWN: nextHead.first += 1; break;
            case RIGHT: nextHead.second += 1; break;
            case LEFT: nextHead.second -= 1; break;
        } // 이동할 위치로 설정.

        // 이동하고자 하는 위치의 정보를 확인함.
        CellValue cellValue = gameMap.getCellValue(nextHead.first, nextHead.second);

        Coordinate lastTail = body.back();
        bool isLastTail = (nextHead.first == lastTail.first && nextHead.second == lastTail.second);
        // 벽이나 마지막 꼬리가 아닌 꼬리로 이동하려 한 경우
        if (cellValue == WALL || cellValue == IMMUNE_WALL || (cellValue == TAIL&&!isLastTail)) {
            gameMap.setCellValue(body.front().first, body.front().second, CRUSHED_HEAD);
            // gameMap.print();
            // cout << "Your head was crushed" << endl;
            return true;    // 게임 오버 여부
        } 
        // 게이트에 닿으면 순간이동
        else if (cellValue == GATE) {
            // 게이트 통과 직후 다시 게이트로 들어갔다면 사망
            if (gateSpawner.getTimeToUse() > 0) {
                gameMap.setCellValue(body.front().first, body.front().second, CRUSHED_HEAD);
                // gameMap.print();
                // cout << "Your head was crushed" << endl;
                return true;    // 게임 오버 여부
            }
            Coordinate gatePos_1 = gateSpawner.getGatePos(1);
            Coordinate gatePos_2 = gateSpawner.getGatePos(2);
            Coordinate destination;

            // 통과중일때 게이트 소멸을 지연시킴.
            gateSpawner.setTimeToUse(body.size());

            // 도착지 설정
            if (nextHead == gatePos_1) destination = gatePos_2;
            else destination = gatePos_1;

            // 게이트 통과 후의 머리 위치 업데이트
            nextHead = getPosAfterGate(destination);
            body.push_front(nextHead);
            decreaseTail();

            
            lastConsumedItem = GATE; // lastConsumedItem 업데이트 - 추가한 부분
            gameMap.setAlreadyUpdate(false);    // 추가한 부분
        }
        // 그 외(전진할 수 있을 때)
        else {

            // 이동할 자리를 머리로 교체
            body.push_front(nextHead);

            // 빈공간이나 마지막꼬리로 이동한 경우
            if (cellValue == EMPTY_SPACE || isLastTail) {
                decreaseTail();
            }
            // 독 아이템 획득시 
            else if (cellValue == POISON_ITEM) {
                decreaseTail();
                decreaseTail();
                gameMap.setAlreadyUpdate(false);    // 추가한 부분
                lastConsumedItem = POISON_ITEM; // lastConsumedItem 업데이트 - 추가한 부분
                

                // 몸 길이가 3보다 작아지면 죽음.
                if (body.size() < 3) {
                    dieAnimation();
                    // gameMap.print();
                    // cout << "You ate poison too much" << endl;
                    return true;
                }
            } 

            // 성장 아이템 획득시엔 꼬리를 삭제하지 않음. 
            else if (cellValue == GROWTH_ITEM) {
                gameMap.setAlreadyUpdate(false);    // 추가한 부분
                lastConsumedItem = GROWTH_ITEM; // lastConsumedItem 업데이트 - 추가한 부분
            }
        }
        return false; // 게임 지속됨.
    }

    // 입력한 방향(wasd)으로 방향 설정. 제대로 입력하면 true
    void setDirection(char input) { 
        switch(input) {
            case 'w': curDirection = UP; break;
            case 's': curDirection = DOWN; break;
            case 'a': curDirection = LEFT; break;
            case 'd': curDirection = RIGHT; break;
        }
    }

    // 게이트 통과 후의 머리가 올 위치를 얻음
    Coordinate getPosAfterGate(Coordinate destinationPos) {
        int row = destinationPos.first;
        int col = destinationPos.second;
        Coordinate afterPos;    // 나와서 이동할 자리 

        // 나오는 게이트가 가장자리에 있는 경우
        if (row == 0) curDirection = DOWN;                          // 위쪽 벽 -> 아래로 출발
        else if (row == gameMap.getHeight()-1) curDirection = UP;   // 아래쪽 벽 -> 위로 출발
        else if (col == 0) curDirection = RIGHT;                    // 왼쪽 벽 -> 오른쪽으로 출발
        else if (col == gameMap.getWidth()-1) curDirection = LEFT;  // 오른쪽 벽 -> 왼쪽으로 출발
        // 나오는 게이트가 가장자리가 아닌 경우
        else {
            curDirection = getNextDirection(curDirection, destinationPos);  // 우선순위를 고려하여 가능한 방향으로 설정
        }

        return getNextPos(curDirection, destinationPos);       // 게이트 통과시 머리가 나올 위치 반환
    }

    // 뱀의 몸을 이루는 좌표들을 리턴
    const deque<Coordinate>& getBody() const {
        return body;
    }

    // 뱀의 꼬리를 한 칸 감소시키는 함수
    void decreaseTail() {
        Coordinate oldTail = body.back();
        body.pop_back();
        gameMap.setCellValue(oldTail.first, oldTail.second, EMPTY_SPACE);
    }

    // 게이트에서 나온 후 이동할 방향을 반환함.(게이트가 가장자리가 아닌 경우)
    Direction getNextDirection(Direction curDir, Coordinate destinationPos) {
        Coordinate nextPos;                 // 다음에 이동할 자리로 쓸 변수
        Direction nextDir = curDir;         // 다음에 이동할 방향으로 쓸 변수
        nextPos = getNextPos(nextDir, destinationPos);  // 다음에 이동할 자리(현재 방향 유지)
        
        if (isThatCellEmpty(nextPos)){      // 들어온 방향으로 이동할 수 있는 경우
            return nextDir;                 // 들어온 방향을 반환
        }
        else {                                          // 현재 이동 방향으로 이동할 수 없는 경우 -> 오른쪽 먼저 고려
            int dir = (nextDir + RIGHT) % 4;                // 오른쪽 90도 회전
            nextDir = (Direction) dir;                      // 이동 방향 오른쪽으로 변경됨.
            nextPos = getNextPos(nextDir, destinationPos);  // 이동할 칸 오른쪽 칸으로 변경됨.
            if (!isThatCellEmpty(nextPos)) {            // 오른쪽으로 이동할 수 없는 경우 -> 왼쪽 고려
                dir = (nextDir + DOWN) % 4;                     // 180도 회전(처음 기준 왼쪽)
                nextDir = (Direction) dir;                      // 이동 방향 왼쪽으로 변경됨.
                nextPos = getNextPos(nextDir, destinationPos);  // 이동할 칸 왼쪽 칸으로 변경됨.
                if (!isThatCellEmpty(nextPos)) {        // 뒤로만 이동할 수 있는 경우
                    dir = (nextDir + LEFT) % 4;                 // 270도 회전(들어온 방향의 반대 방향)
                    nextDir = (Direction) dir;                  // 이동 방향 뒤쪽으로 변경됨.
                    return nextDir;         // 들어온 방향의 반대 방향 반환       
                } else return nextDir;      // 들어온 방향의 왼쪽 방향 반환
            } else return nextDir;          // 들어온 방향의 오른쪽 방향 반환
        }
    }

    // 입력 좌표에서 입력한 방향으로 이동한 곳의 좌표를 반환함.
    Coordinate getNextPos(Direction dir, Coordinate pos) {
        switch (dir)
        {
        case UP: pos.first--; break;
        case DOWN: pos.first++; break;
        case LEFT: pos.second--; break;
        case RIGHT: pos.second++; break;
        }
        return pos;
    }

    // 입력한 위치가 비었는지 알려줌.
    bool isThatCellEmpty(Coordinate pos) {
        return gameMap.getCellValue(pos.first, pos.second) == EMPTY_SPACE;
    }

    // 뱀의 그래픽을 업데이트함. 틱에서 가장 마지막에 호출할 것.
    void update() {
        for (const auto& pos : body) {
            if (pos == body.front()) {
                gameMap.setCellValue(pos.first, pos.second, HEAD);
            }
            else {
                gameMap.setCellValue(pos.first, pos.second, TAIL);
            }
        }
    }

    // 독 먹고 죽을 때 전신이 * 모양이 됨.
    void dieAnimation() {
        for (int i = 0; i <= body.size(); i++) {
            Coordinate part = body.back();
            body.pop_back();
            gameMap.setCellValue(part.first, part.second, CRUSHED_HEAD);
        }
    }
    
    CellValue getLastConsumedItem() const { // 리턴 타입 enum으로 바꿈.
        return lastConsumedItem;
    }

};

// 아이템 생성기 클래스
class ItemSpawner {
private:
    bool active;    // 생성기 활성화 여부
    CellValue itemType;  // 스폰할 아이템의 종류
    int spawnCycle;     // 스폰 주기
    int countCycle;     // 스폰 주기를 측정할 변수
    int itemLife;       // 아이템의 수명
    GameMap& gameMap;   // 게임 맵 참조 변수

public:
    ItemSpawner(CellValue itemType, int spawnCycle, GameMap& gameMap) : gameMap(gameMap) {
        this->itemType = itemType;
        this->spawnCycle = spawnCycle;
        this->countCycle = spawnCycle;
        this->active = false;
        itemLife = 20;
    }

    void resetForNewStage() {
        active = false;
        countCycle = spawnCycle;
    }

    // 활성화 여부 설정
    void setActive(bool state) {
        active = state;
    }

    // 업데이트(틱마다 호출)
    void update() {
        if (!active) return;    // 비활성화시 패스
        countCycle--;
        if (countCycle == 0) {
            spawn();
            countCycle = spawnCycle;
        }
    }

    // 아이템 생성
    void spawn() {
        Coordinate randomPos = this->getRandomPosition();
        if (randomPos.first == -1) return; // 생성할 수 있는 위치가 없다면 패스
        Item* item = new Item(itemType, itemLife, randomPos);
        bool spawnSuccess = gameMap.addItem(item);
        if (spawnSuccess)
            gameMap.setCellValue(randomPos.first, randomPos.second, itemType); // 맵에 아이템 생성
        else
            delete item;
    }



    // EMPTY_SPACE 중 무작위로 하나의 위치를 반환
    Coordinate getRandomPosition() {
        int maximumLoop = gameMap.getWidth() * gameMap.getHeight();
        int row, col;
        while (maximumLoop--)
        {
            // 무작위로 선택한 위치가 EMPTY_SPACE라면 반환
            col = rand() % gameMap.getWidth();
            row = rand() % gameMap.getHeight();
            if (gameMap.getCellValue(row, col) == EMPTY_SPACE){
                return Coordinate(row, col);
            }
        }
        return Coordinate(-1, -1);  // 만약 생성할 수 있는 위치가 없으면 예외 출력.
    }
};

void initializeColors() {
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_BLACK, COLOR_WHITE);
}

void updateScoreBoard(WINDOW *scoreBoardWin, const GameMap &gameMap) {
    werase(scoreBoardWin);

    mvwprintw(scoreBoardWin, 1, 1, "Score Board");
    mvwprintw(scoreBoardWin, 2, 1, "B: %d / %d", gameMap.getCurrentLength(), gameMap.getMaxLength());
    mvwprintw(scoreBoardWin, 3, 1, "+: %d", gameMap.getGrowthItemsConsumed());
    mvwprintw(scoreBoardWin, 4, 1, "-: %d", gameMap.getPoisonItemsConsumed());
    mvwprintw(scoreBoardWin, 5, 1, "G: %d", gameMap.getGatesUsed());

    mvwprintw(scoreBoardWin, 7, 1, "Mission");
    mvwprintw(scoreBoardWin, 8, 1, "B: 7 %s", gameMap.getMaxLength() >= 7 ? "(v)" : "( )");
    mvwprintw(scoreBoardWin, 9, 1, "+: 5 %s", gameMap.getGrowthItemsConsumed() >= 5 ? "(v)" : "( )");
    mvwprintw(scoreBoardWin, 10, 1, "-: 3 %s", gameMap.getPoisonItemsConsumed() >= 3 ? "(v)" : "( )");
    mvwprintw(scoreBoardWin, 11, 1, "G: 3 %s", gameMap.getGatesUsed() >= 3 ? "(v)" : "( )");

    mvwprintw(scoreBoardWin, 12, 1, "Stage: %d", gameMap.getCurrentStage());

    wrefresh(scoreBoardWin);
}


int main() {
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    curs_set(0);

    initializeColors();

    attron(COLOR_PAIR(1));
    border(ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    attroff(COLOR_PAIR(1));
    wbkgd(stdscr, COLOR_PAIR(3));

    int height, width;
    getmaxyx(stdscr, height, width);
    mvprintw(0 ,(width / 2) - 5, " Snake Game ");

    //게임 화면
    WINDOW *gameWin = newwin(height, width - 30, 0, 0); // Main game window
    WINDOW *scoreBoardWin = newwin(height, 30, 0, width - 30); // Score board window
    wbkgd(gameWin, COLOR_PAIR(2));

    GameMap gameMap(0);  // 첫 번째 프리셋을 적용하여 맵 생성
    ItemSpawner growthItemSpawner(GROWTH_ITEM, 1, gameMap);   // 성장 아이템 생성기 생성
    ItemSpawner poisonItemSpawner(POISON_ITEM, 1, gameMap);   // 독 아이템 생성기 생성
    GateSpawner gateSpawner(gameMap);   // 게이트 생성기 생성
    Snake snake(gameMap, gateSpawner); // 뱀 생성

    
    char input;
    bool gameOver = false;
    int spawnerActivePoint = 5;
    while (!gameOver) {
        gameMap.manageItems();
        gameMap.print(gameWin);

        input = getch();
        if(input != ERR)
            snake.setDirection(input);

        gameOver = snake.moveTest(); // wasd로 이동
        if (spawnerActivePoint-- < 0) {
            growthItemSpawner.setActive(true);
            poisonItemSpawner.setActive(true);
            gateSpawner.setActive(true);
        }
        snake.update();
        growthItemSpawner.update();
        poisonItemSpawner.update();
        gateSpawner.update();
        timeout(500);

        gameMap.updateMissionStatus(snake.getLastConsumedItem());

        if (gameMap.checkMissionCompleted()) {
            int nextStage = gameMap.getCurrentStage() + 1;
            gameMap.resetForNewStage(nextStage);
            // 모든 객체들의 멤버 변수들 리셋 추가함.
            snake.resetForNewStage();
            growthItemSpawner.resetForNewStage();
            poisonItemSpawner.resetForNewStage();
        }

        updateScoreBoard(scoreBoardWin, gameMap);
    }   

    delwin(gameWin);
    delwin(scoreBoardWin);
    clear();
    mvprintw(height / 2,(width / 2) - 5, " Game Over ");
    
    timeout(2000);
    getch();
    endwin();
    return 0;
}
