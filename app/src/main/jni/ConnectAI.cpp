#include<jni.h>
#include<stdlib.h>  // get random-number generator
#include<time.h>    // get time to see RNG

// This code adapted from http://cboard.cprogramming.com/game-programming/53886-connect-four-ai-demo.html

//------ insert function prototypes for gameOver and determineComputerMove ------

const char userChar = 'X';  // char to represent user spaces
const char compChar = 'O';  // char to represent computer spaces

// how many move ahead will we look?
int maxDepth = 4;

// constants used in the move evaluation system
const int maxScore = 100;
const int invalidScore = 1000000;


extern "C" {
    jint Java_com_example_david_hw8_MainActivity_determineComputerMove(JNIEnv *env, jobject thiz, jobjectArray curboard);
    jchar Java_com_example_david_hw8_MainActivity_gameOver(JNIEnv *env, jobject thiz, jobjectArray curboard);
}

// Board class
class Board
{
public:
    const char at(const int x, const int y) const;
    void setBoard( JNIEnv *env, jobjectArray javaBoard );
    int getScore() const;
        char checkWin() const;
    void move(const int x, const char player);
        bool canMove(const int x) const;
    void undo(const int x, const char player);

    static const int width=7,           // width of the board
                     height=6,          // height of the board
                     toWin=4;           // # of consecutive spots to win

    Board();

    int moveScore[width];

protected:
    char data[width][height];
    int lastX,lastY;
};

// prototype for function we will need later
int determineMove(Board& board, char player);

// Is the game over
// return 'X' or 'O' for win, or ' ' if nobody has won
jchar Java_com_example_david_hw8_MainActivity_gameOver(
                    JNIEnv *env, jobject thiz, jobjectArray curboard )
{
    Board b;
    // convert the incoming board to the internal Board class
    b.setBoard( env, curboard );

    return b.checkWin();
}

// determine where column the computer should move
// ----- change the name and parameters so this can be called from Java -----
jint Java_com_example_david_hw8_MainActivity_determineComputerMove(JNIEnv *env, jobject thiz, jobjectArray curboard )
{
    Board b;
    // convert the incoming board to the internal Board class
    b.setBoard( env, curboard );

    // initialize the RNG
    srand48( time(0) );

    // come up with a move for the computer
    return determineMove( b, compChar );
}

// set the board from java
void Board::setBoard( JNIEnv *env, jobjectArray javaBoard )
{
	// ----- insert code to copy the board from
	// ----- javaBoard[][] to data[][]

	int i, j;

	for(i = 0; i < width; i++) {
	    jcharArray oneDim = (jcharArray)env->GetObjectArrayElement(javaBoard,i);
	    jchar *element = env->GetCharArrayElements(oneDim,0);
	    for(j=0; j < height; j++) {
	        data[i][j] = element[j];
	    }

	    env->ReleaseCharArrayElements(oneDim, element, 0);
	}
}

// implementation of Board class
const char Board::at(const int x, const int y) const
{
    return data[x][y];
}

// Create a score for this board position
int Board::getScore() const
{
    int score = 0;

    //Pieces in the middle score higher than
    //pieces at the edges

    for (int x=0;x<width;++x)
    {
        int columnscore = (width/2) - x;
        if (columnscore < 0) columnscore = -columnscore;
        columnscore = (width/2) - columnscore;

        //Count the number of pieces in each column
        //and score accordingly
        for (int y=0;y<height;++y)
        {
            int rowscore = (height/2) - y;
            if (rowscore < 0) rowscore = -rowscore;
            rowscore = (height/2) - rowscore;

            if (at(x,y)=='X')
                score += columnscore+rowscore;
            else if (at(x,y) == 'O')
                score -= columnscore+rowscore;
        }
    }

    return score;
}

// did one player or the other win (or a tie)?
char Board::checkWin() const
{
    int col, row, i;

    // look for horizontal win
    for( col=0; col<width-3; col++ )
        for( row=0; row<height; row++ )
            // check for win here
            if ( data[col][row] != ' ' &&
                    data[col][row] == data[col+1][row] &&
                    data[col][row] == data[col+2][row] &&
                    data[col][row] == data[col+3][row] )
                return data[col][row];

    // look for vertical win
    for( col=0; col<width; col++ )
        for( row=0; row<height-3; row++ )
            // check for win here
            if ( data[col][row] != ' ' &&
                    data[col][row] == data[col][row+1] &&
                    data[col][row] == data[col][row+2] &&
                    data[col][row] == data[col][row+3] )
                return data[col][row];

    // look for win diagonal up to the right
    for( col=0; col<width-3; col++ )
        for( row=0; row<height-3; row++ )
            // check for win here
            if ( data[col][row] != ' ' &&
                    data[col][row] == data[col+1][row+1] &&
                    data[col][row] == data[col+2][row+2] &&
                    data[col][row] == data[col+3][row+3] )
                return data[col][row];

    // look for win diagonal up to the left
    for( col=3; col<width; col++ )
        for( row=0; row<height-3; row++ )
            // check for win here
            if ( data[col][row] != ' ' &&
                    data[col][row] == data[col-1][row+1] &&
                    data[col][row] == data[col-2][row+2] &&
                    data[col][row] == data[col-3][row+3] )
                return data[col][row];

    // any empty spots?
    for( col=4; col<width; col++ )
        for( row=0; row<height; row++ )
            // check for win here
            if ( data[col][row] == ' ' )
                // not over
                return ' ';

    // full board with no winner
    return 'T';
}

void Board::move(const int x, const char player)
{
    //Find the first free place and place a
    //brick there
    for (int y=0;y<height;++y)
        if (at(x,y) == ' ')
        {
            data[x][y] = player;
            lastX = x;
            lastY = y;
            return;
        }
}

// can we put a piece in this column?
bool Board::canMove(const int x) const
{
    return data[x][height-1] == ' ';
}

// backing up - undo changes to the board
void Board::undo(const int x, const char player)
{
    //Find the last non-free place and place a
    //brick there
    int y = height-1;
    while (y >= 0 && at(x,y) == ' ') --y;
    if (at(x,y) == player)
        data[x][y] = ' ';
}

// initialize a board to empty
Board::Board()
{
    lastX = lastY = 0;

    for (int x=0;x<width;++x)
    {
        for (int y=0;y<width;++y)
            data[x][y] = ' ';
        moveScore[x] = 0;
    }
}

// do an alpha-beta search for the best computer move
int alphabeta(Board& board, char player, int alpha=-invalidScore, int beta=invalidScore, int depth=0)
{
    //Check if there's a current winner
    char winner = board.checkWin();
    if (winner == 'X')
        return maxScore;
    else if (winner == 'O')
        return -maxScore;

    if (depth >= maxDepth)
    {
        //We cannot recurse more, and we haven't found
        //an end to the game
        //Return the score of the current table
        return board.getScore();
    }

    if (player == 'X')
    {
        //It is player X's turn, he will try to
        //maximize the score

        //Go through all possible moves and get the
        //scores
        int maxScore = -invalidScore;
        for (int move=0;move<Board::width;++move)
            if (board.canMove(move))
            {
                board.move(move,'X');
                int score = alphabeta(board,'O',alpha,beta,depth+1);
                board.undo(move,'X');
                if (score > alpha) alpha = score;
                if (alpha >= beta) return alpha;
            }
        return alpha;
    }

    else if (player == 'O')
    {
        //It is player X's turn, he will try to
        //maximize the score

        //Go through all possible moves and get the
        //scores
        int minScore = invalidScore;
        for (int move=0;move<Board::width;++move)
            if (board.canMove(move))
            {
                board.move(move,'O');
                int score = alphabeta(board,'X',alpha, beta, depth+1);
                board.undo(move,'O');
                if (score < beta) beta = score;
                if (alpha >= beta) return beta;
            }
        return beta;
    }
    else
        //This is just for safety if the function
        //is called with wrong parameters
        return 0;
}

// look at the possible moves
int determineMove(Board& board, char player)
{

    if (player == 'X')
    {
        //It is player X's turn, he will try to
        //maximize the score

        //Go through all possible moves and get the
        //scores
        int maxScore = -invalidScore;
        int maxMove  = 0;
        for (int move=0;move<Board::width;++move)
            if (board.canMove(move))
            {
                board.move(move,'X');
                int score = alphabeta(board,'O');

                board.moveScore[move] = score;

                if (score>=maxScore)
                {
                    maxScore = score;
                    maxMove  = move;
                }
                board.undo(move,'X');
            }
            else
                //Just set the move score to an invalid score
                board.moveScore[move] = invalidScore;

        //Return the move with the highest score
        return maxMove;
    }

    else if (player == 'O')
    {
        //It is player O's turn, he will try to
        //minimize the score

        //Go through all possible moves and get the
        //scores
        int minScore = invalidScore;
        int minMove  = 0;
        for (int move=0;move<Board::width;++move)
        {
            if (board.canMove(move))
            {
                board.move(move,'O');
                int score = alphabeta(board,'X');

                board.moveScore[move] = score;

                if (score<minScore)
                {
                    minScore = score;
                    minMove  = move;
                }
                board.undo(move,'O');
            }
            else
                //Just set the move score to an invalid score
                board.moveScore[move] = invalidScore;
        }
        //Return the move with the least score
        return minMove;
    }
    else
        //This is just for safety if the function
        //is called with wrong parameters
        return 0;
}