#include <windows.h>
#include "board.h"

// The main window class name.
static TCHAR szWindowClass[] = _T("DesktopApp");
int __size = 3;//size of the board (__size x __size boards in a cell)
int moves = 0;
int aiDepth = 7;//depth ai will evaluate for
Board board(__size);

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Ultimate Tic Tac Toe AI");

HINSTANCE hInst;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/*
* Setup for windows desktop application
*/
int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
){
    //Initialize GDIPLUS
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

    WNDCLASSEX wcex;
    //Setup a console to debug outputs to
    AllocConsole();
    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed!"),
            _T("Windows Desktop Guided Tour"),
            NULL);

        return 1;
    }

    // Store instance handle in our global variable
    hInst = hInstance;

    // The parameters to CreateWindowEx explained:
    // WS_EX_OVERLAPPEDWINDOW : An optional extended window style.
    // szWindowClass: the name of the application
    // szTitle: the text that appears in the title bar
    // WS_OVERLAPPEDWINDOW: the type of window to create
    // CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
    // 500, 200: initial size (width, length)
    // NULL: the parent of this window
    // NULL: this application does not have a menu bar
    // hInstance: the first parameter from WinMain
    // NULL: not used in this application
    HWND hWnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        500+15, 500+35,
        NULL,
        NULL,
        hInstance,
        NULL
    );
    SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE)
        & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);

    if (!hWnd)
    {
        MessageBox(NULL,
            _T("Call to CreateWindow failed!"),
            _T("Windows Desktop Guided Tour"),
            NULL);

        return 1;
    }

    // The parameters to ShowWindow explained:
    // hWnd: the value returned from CreateWindow
    // nCmdShow: the fourth parameter from WinMain
    ShowWindow(hWnd,
        nCmdShow);
    UpdateWindow(hWnd);
    // Main message loop:
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    Gdiplus::GdiplusShutdown(gdiplusToken);


    return (int)msg.wParam;
}


/*
* A utility function that prints to the console the current board position
* mainly for debugging
* 
* @param: Board* b: a pointer to the board we want to print
*/
void printBoard(Board* b) {
    for (auto i = 0; i < __size * __size; i++) {
        for (auto y = 0; y < __size * __size; y++) {
            std::cout << (b->smallBoard[i][y] == 1 ? 'x' : (b->smallBoard[i][y] == -1 ? 'o' : '*'));
            if ((y + 1) % __size == 0) { std::cout << "|"; }
        }
        std::cout << std::endl;
        if ((i+1) % __size == 0) { std::cout << "---------------" << std::endl; }
    }
}

/*
    * Function that updates the GUI board for the user
    *
    * @param: HDC hdc : a handle to the device context that we will draw upon
    * @param: Board* b: a pointer to the board we will use for the gui
    */
void drawBoard(HDC hdc, Board* b) {
    Gdiplus::Graphics gf(hdc);
    Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 0));
    b->player_one_turn = !b->player_one_turn;
    int n = b->size;
    int offset = 500 / n;
    pen.SetWidth(5);
    Gdiplus::SolidBrush mySolidBrush(Gdiplus::Color(255, 255, 0, 255));
    //Draw a fill on the region where the valid move may happen, blank if any move
    // besides on finished squares is possible
    gf.FillRectangle(&mySolidBrush, b->validX * offset, b->validY * offset, offset, offset);

    //Draw the BIG Xs and BIG Os on the board
    for (auto y = 0; y < n; y++) {
        for (auto x = 0; x < n; x++) {
            if (b->bigBoard[y][x] == 1) {
                gf.DrawLine(&pen, x * offset, y * offset, (x + 1) * offset, (y + 1) * offset);
                gf.DrawLine(&pen, x * offset, (y + 1) * offset, (x + 1) * offset, y * offset);
            }
            else if (b->bigBoard[y][x] == -1) {
                gf.DrawEllipse(&pen, x * offset, y * offset, offset, offset);
            }
        }
    }

    //Sequester into nths
    for (auto j = 0; j <= n; j++) {
        for (auto i = 0; i <= n; i++) {
            gf.DrawLine(&pen, i * offset, 0, i * offset, j * 500 / n);
            gf.DrawLine(&pen, 0, j * offset, i * 500 / n, j * offset);
        }
    }

    //sequester into n^2ths
    pen.SetWidth(1);
    offset =  500 / (n * n);
    for (auto j = 0; j < (n * n); j++) {
        for (auto i = 0; i < (n * n); i++) {
            if (i % n == 0 || j % n == 0) {
                continue;
            }
            gf.DrawLine(&pen, i * offset, 0, i * offset, j * 500 / n);
            gf.DrawLine(&pen, 0, j * offset, i * 500 / n, j * offset);
        }
    }

    //Go and draw the xs and os on the smallboards
    for (auto y = 0; y < (n * n); y++) {
        for (auto x = 0; x < (n * n); x++) {
            if (b->smallBoard[y][x] == 1) {
                gf.DrawLine(&pen, x * offset, y * offset, (x + 1) * offset, (y + 1) * offset);
                gf.DrawLine(&pen, x * offset, (y + 1) * offset, (x + 1) * offset, y * offset);
            }
            else if (b->smallBoard[y][x] == -1) {
                gf.DrawEllipse(&pen, x * offset, y * offset, offset, offset);
            }
        }
    }
    b->player_one_turn = !b->player_one_turn;
}

/*
* Statically evaluate the position of the board
* Positive indicates an advantage positionwise to X
* Negative indicates an advantage positionwise to O
* 0 indicaates neutral position
* 
* @param: Board* b: a pointer to the board we will evaluate
* @return: the numerical position evaluation of the board
*/
int staticEval(Board* b) {
    //return an evaluation of the board
    if (b->tied) {
        return 0;
    }
    else if (b->wonGame) {
        //see who played last move, they are the winner
        return b->lastPlayer == 'x' ? INT_MAX : -INT_MAX;
    }
    else {
        //evaluate the given board position
        int x2 = 0;
        int o2 = 0;
        int o1 = 0;
        int x1 = 0;

        //sum rows and cols
        int sumR = 0;
        int sumC = 0;
        for (auto x = 0; x < __size * __size; x++) {
            for (auto k = 0; k < __size; k++) {
                sumR += b->rows[x][k] * (b->rowsCounter[x][k] % __size) * (b->rowsCounter[x][k] % __size);//favors 2 in a row, doesn't give points to __size filled no win
                sumC += b->col[x][k] * (b->colCounter[x][k] % __size) * (b->colCounter[x][k] % __size);
            }
        }

        //sum diagonals
        int sumDiag = 0;
        int sumADiag = 0;
        for (auto it : b->diag) {
            sumDiag += it.second * (b->diagCounter[it.first] % __size) * (b->diagCounter[it.first] % __size);
        }
        for (auto it : b->adiag) {
            sumADiag += it.second * (b->adiagCounter[it.first] % __size) * (b->adiagCounter[it.first] % __size);
        }

        //sumBigRows
        int sumBR = 0;
        int sumBC = 0;
        for (auto y = 0; y < __size; y++) {
            for (auto k = 0; k < __size; k++) {
                sumBR += b->bigRow[y] * (b->bigRowCounter[y] % __size) * (b->bigRowCounter[y] % __size);
                sumBC += b->bigCol[y] * (b->bigColCounter[y] % __size) * (b->bigColCounter[y] % __size);
            }
        }

        int sumBD = 0;
        int sumBAD = 0;
        for (auto it : b->bigAdiag) {
            sumBAD += it * (b->bigAdiagCounter % __size) * (b->bigAdiagCounter % __size);
        }
        for (auto it : b->bigDiag) {
            sumBD += it * (b->bigDiagCounter % __size) * (b->bigDiagCounter % __size);
        }


        o1 = min(0, sumR) + min(0, sumC) + min(sumDiag, 0) + min(sumADiag, 0);
        x1 = max(0, sumR) + max(0, sumC) + max(sumDiag, 0) + max(sumADiag, 0);
        o2 = min(0, sumBR) + min(0, sumBC) + min(0, sumBD) + min(0, sumBAD);
        x2 = max(0, sumBR) + max(0, sumBC) + max(0, sumBD) + max(0, sumBAD);
        int eval = (6 * x2 + x1) + (6 * o2 + o1);

        return  eval;

    }
}

/*
* Implementation of the minimax algorithm with alpha beta pruning to sus out the
* most optimal play for the ai player
* 
* @param: Board* position: the board state we are going to minimax
* @param: int depth: the remaining depth we have
* @param: int alpha: the alpha being considered for this board state
* @param: int beta: the beta being considered for this board state
* @param: bool maximizingPlayer: whether the current move is trying to max or min the evaluation
* 
* @return: the max or min (depending on bool) evaluation from all possible moves within depth, 
*           i.e., the estimated best move in the current position
* 
* @NOTE: Eval function sucks so this isn't very good either
*/
int minimax(Board* position, int depth, int alpha, int beta, bool maximizingPlayer) {
    
    if (depth == 0 || position->wonGame || position->tied){
        //printBoard(position);
        int eval = staticEval(position);
        //std::cout << eval << std::endl;;
        return eval;
    }

    if (maximizingPlayer){
        int maxEval = -100000;
        for (auto x = 0; x < (board.size * board.size); x++) {
            for (auto y = 0; y < (board.size * board.size); y++) {
                if (position->validMove(x, y)) {
                    //For a valid move, send the child node which
                    //is the currentBoard + that valid move
                    Board tempBoard(*position);
                    tempBoard.makeTurn(x, y);
                    int eval = minimax(&tempBoard, depth - 1, alpha, beta, false);
                    maxEval = max(maxEval, eval);
                    alpha = max(alpha, eval);
                    if(beta <= alpha) break;
                }
            }
        }
        return maxEval;
    }
    else {
        int minEval = 100000;
        for (auto x = 0; x < (board.size * board.size); x++) {
            for (auto y = 0; y < (board.size * board.size); y++) {
                if (position->validMove(x, y)) {
                    Board tempBoard(*position);
                    tempBoard.makeTurn(x, y);
                    int eval = minimax(&tempBoard, depth - 1, alpha, beta, true);
                    minEval = min(minEval, eval);
                    beta = min(beta, eval);
                    if(beta <= alpha) break;
                }
            }
        }
        return minEval;
    }
    return 0;
}

/*
* Update the user's gui to reflect the current boardstate and highlight the cell that
* the user can play within
* 
* @param: int x: the last played x position
* @param: int y: the last played y position
* @param HWND hWnd: a handle to the window that will be drawn on
*/
void updateGUI(int x, int y, HWND hWnd) {
    int size = board.size;
    int h = board.h;
    int validX = board.validX;
    int validY = board.validY;
    //rectangle that was played in
    RECT r1 = {};
    r1.left = x / size * h / size;
    r1.right = r1.left + (h / size);
    r1.top = y / size * h / size;
    r1.bottom = r1.top + (h / size);
    
    //rectangle next player can play in
    RECT r2 = {};
    r2.left = validX * h / size;
    r2.right = r2.left + (h / size);
    r2.top = validY * h / size;
    r2.bottom = r2.top + (h / size);
    InvalidateRect(hWnd, &r1, TRUE);
    InvalidateRect(hWnd, &r2, TRUE);
    UpdateWindow(hWnd);
}

/*
* Determine if there is a winner on the board, tell the user about it and ask for a rematch
* 
* @param: HWND hWnd: a handle to the window we are playing in
* @param the rest are for calling windproc for reset
*/
void doWinProtocol(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (board.wonGame) {
        //Draw the winner: player_one is x, not_player_one is o
        auto winningMessage = !board.player_one_turn ? L"Congratulations X, you have won!" : L"Congratulations O, you have won!";
        int val = MessageBox(NULL, winningMessage, TEXT("Message"), MB_YESNO);
        switch (val) {
        case IDYES: {
            //User wants to replay
            //reset the board
            Board newBoard(__size);
            board = newBoard;
            //reset the gui
            InvalidateRect(hWnd, NULL, true);
            WndProc(hWnd, WM_PAINT, wParam, lParam);
            break;
        }
        case IDNO:
            //User does not want to replay
            std::exit(0);
            break;
        default:
            break;
        }
    }
    else if (board.tied) {
        auto tiedMsg = L"THE GAME HAS ENDED IN A TIE";
        MessageBox(NULL, tiedMsg, TEXT("Message"), MB_OK);
    }
}

/*
* Have the AI minimax the current board position and find the minimizing move
* 
* @param: int* x: pointer that will be filled by the best available x move (acc to minimax)
* @param: int* y: pointer that will be filled by the best available y move (acc to minimax)
*/
void AITurn(int* bestX, int* bestY) {
    int bestEval = INT_MAX;
    for (auto x = 0; x < (board.size * board.size); x++) {
        for (auto y = 0; y < (board.size * board.size); y++) {
            if (board.validMove(x, y)) {
                Board tmpB = board;
                tmpB.makeTurn(x, y);
                
                int eval = minimax(&tmpB, aiDepth, -INT_MAX, INT_MAX, false);
                std::cout << eval << std::endl;
                if (eval < bestEval) {
                    bestEval = eval;
                    *bestX = x;
                    *bestY = y;
                }
            }
        }
    }
    std::cout << "bestEval i.e. minimizing value is: " << bestEval << std::endl;
}

/*
* Processes messages for the main window.
* 
* WM_PAINT    - Paint the main window
* WM_DESTROY  - post a quit message and return
* WM_LBUTTONDOWN - handle user left clicks
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC hdc;
    switch (message) {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        drawBoard(hdc, &board);
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_LBUTTONDOWN: {
        POINT curPos;
        GetCursorPos(&curPos);
        ScreenToClient(hWnd, &curPos);

        //Find bucket the user clicked in (big square from big board, numbered in
        // x y pair where x is horizontal and y is vertical
        int x = curPos.x / (500 / (board.size * board.size));
        int y = curPos.y / (500 / (board.size * board.size));

        if (board.makeTurn(x, y)) {
            doWinProtocol(hWnd, message, wParam, lParam);
            moves++;
        }
        else {
            break;
        }
        updateGUI(x, y, hWnd);

        AITurn(&x, &y);
        if (board.makeTurn(x, y)) {
            doWinProtocol(hWnd, message, wParam, lParam);
            moves++;
        }
        updateGUI(x, y, hWnd);

        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}
