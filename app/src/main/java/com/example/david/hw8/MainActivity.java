package com.example.david.hw8;

import android.app.Activity;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;


public class MainActivity extends Activity {
    public static int WIDTH = 7;
    public static int HEIGHT = 6;

    // current board 'X'=user, 'O'=computer, ' '=empty
    char[][] board = new char[WIDTH][HEIGHT];

    // UI items
    ConnectView cv;

    //load cpp file
    static {
        System.loadLibrary("ConnectAI");
    }

    public native int determineComputerMove(char[][] curBoard);
    public native char gameOver(char[][] curBoard);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        cv = (ConnectView) findViewById( R.id.cv_screen );
        cv.setActivity(this);
        this.createBoard();//create board
    }
    public void onSaveInstanceState(Bundle savedInstanceState) {
        super.onSaveInstanceState(savedInstanceState);
        savedInstanceState.putSerializable("board", this.board);
    }

    public void onRestoreInstanceState(Bundle savedInstanceState){
        super.onRestoreInstanceState(savedInstanceState);
        this.board = (char[][])savedInstanceState.getSerializable("board");
    }
    // get one position on the board
    // called by ConnectView
    public char getBoardEntry( int col, int row ) {
        return board[col][row];
    }

    // user is making a move
    // called by ConnectView
    public void userMove( int col ) {
        // --- fill this in ---
        boolean moveMade = false;
        for(int i = 0; i < this.board[col].length; i++) {
            if(this.board[col][i] == ' '){
                this.board[col][i] = 'X';
                moveMade = true;
                break;
            }
        }

        if(!moveMade) {
            Toast.makeText(this.getApplicationContext(), "No positions in this column!", Toast.LENGTH_LONG).show();
                    return;
        }


        //CPU turn
        int computerTurn = determineComputerMove(this.board);

        for(int i = 0; i < this.board[computerTurn].length; i++) {
            if(this.board[computerTurn][i] == ' '){
                this.board[computerTurn][i] = 'O';
                break;
            }
        }

        cv.invalidate();

        char currentGameStatus = this.gameOver(this.board);
        //insert method for gamestatus
        if(currentGameStatus == ' ') {
            return;
        }

        else if (currentGameStatus == 'X'){
            Toast.makeText(this.getApplicationContext(), "User wins", Toast.LENGTH_LONG).show();
        }

        else if (currentGameStatus == 'O'){
            //CPU wins
            Toast.makeText(this.getApplicationContext(), "CPU WIns", Toast.LENGTH_LONG).show();
        }

        else{
            //tie
            Toast.makeText(this.getApplicationContext(), "tie", Toast.LENGTH_LONG).show();
        }

        Toast.makeText(this.getApplicationContext(), "New Game", Toast.LENGTH_LONG).show();

    }
    /*create game board
    initialize the places with spaces
     */
    public void createBoard() {
        for(int i = 0; i < this.board.length; i++) {
            for(int j = 0; j < this.board[i].length; j++) {
                this.board[i][j] = ' ';
            }
        }
    }

    public void newGame(View v) {
        createBoard();
        cv.invalidate();
    }
}
