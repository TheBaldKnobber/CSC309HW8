package com.example.david.hw8;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

public class ConnectView extends View implements View.OnTouchListener {
    Paint backgroundPaint;
    Paint emptyPaint;
    Paint xPaint;
    Paint oPaint;

    // base size of one "box"
    float baseSize;
    // link back to the main activity
    MainActivity mainActivity;

    // rectangles to define the board positions
    RectF[][] boxes = new RectF[MainActivity.WIDTH][MainActivity.HEIGHT];

    public ConnectView(Context context ) {
        super(context);

        // put this in to avoid error when previewing the layout
        if (isInEditMode())
            return;

        init_vars();
    }

    public ConnectView(Context context, AttributeSet attrs) {
        super(context, attrs);

        // put this in to avoid error when previewing the layout
        if (isInEditMode())
            return;

        init_vars();
    }

    public void init_vars() {
        setOnTouchListener(this);

        // create Paints
        backgroundPaint = new Paint();
        backgroundPaint.setStyle(Paint.Style.FILL_AND_STROKE );
        backgroundPaint.setColor(Color.BLUE);

        emptyPaint = new Paint();
        emptyPaint.setStyle(Paint.Style.FILL_AND_STROKE );
        emptyPaint.setColor(Color.WHITE);

        xPaint = new Paint();
        xPaint.setStyle(Paint.Style.FILL_AND_STROKE );
        xPaint.setColor(Color.RED);

        oPaint = new Paint();
        oPaint.setStyle(Paint.Style.FILL_AND_STROKE );
        oPaint.setColor(Color.YELLOW);
    }

    // get main activity
    public void setActivity( MainActivity m ) {
        mainActivity = m;
    }

    // user clicked somewhere
    public boolean onTouch(View v, MotionEvent event) {
        // only respond to finger down
        if ( (event.getAction() & MotionEvent.ACTION_MASK) == MotionEvent.ACTION_DOWN ) {
            float xPos = event.getX();

            // valid touch?
            if ( xPos >= baseSize*0.5 && xPos < baseSize*(MainActivity.WIDTH+0.5)
                    && event.getY() < baseSize*7 ) {
                // yes, is valid
                // where did they touch?
                int col = (int)(xPos/baseSize - 0.5);

                // make the move
                mainActivity.userMove( col );
                return true;
            }
        }
        return false;
    }

    // size has changed, so update boxes
    @Override
    protected void onSizeChanged (int w, int h, int oldw, int oldh) {
        // calculate the base size of one square
        // allowing one extra half-square padding on all sides
        baseSize = (float)Math.min( w/(MainActivity.WIDTH+1), h/(MainActivity.HEIGHT+1) );

        // radius of a circle
        float radius = (float)( baseSize * 0.4 );

        // calculate screen positions anc create boxes
        for( int col=0; col<MainActivity.WIDTH; col++ )
            for( int row=0; row<MainActivity.HEIGHT; row++ ) {
                float baseX = baseSize * (col + 1);
                float baseY = baseSize * (MainActivity.HEIGHT - row);

                boxes[col][row] = new RectF( baseX-radius, baseY-radius,
                        baseX+radius, baseY+radius );
            }
    }

    @Override
    protected void onDraw(Canvas canvas) {
        // draw the background
        canvas.drawRect( 0, 0, baseSize*(MainActivity.WIDTH+1),
                baseSize*(MainActivity.HEIGHT+1) , backgroundPaint );

        // draw dots
        for( int col=0; col<MainActivity.WIDTH; col++ )
            for( int row=0; row<MainActivity.HEIGHT; row++ ) {
                // get the board entry
                char cell = mainActivity.getBoardEntry(col,row);
                // draw this circle
                if ( cell == 'X' )
                    canvas.drawOval( boxes[col][row], xPaint );
                else if ( cell == 'O' )
                    canvas.drawOval( boxes[col][row], oPaint );
                else
                    canvas.drawOval( boxes[col][row], emptyPaint );
            }
    }
}
