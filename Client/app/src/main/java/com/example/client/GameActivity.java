package com.example.client;

import androidx.appcompat.app.AppCompatActivity;

import android.app.Activity;
import android.graphics.Point;
import android.os.Bundle;
import android.view.Display;

public class GameActivity extends Activity {

    // Declare an instance of GameEngine
    GameEngine gameEngine;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Get the pixel dimensions of the screen
        Display display = getWindowManager().getDefaultDisplay();

        // Initialize the result into a Point object
        Point size = new Point();
        display.getSize(size);

        // Create a new instance of the SnakeEngine class
        gameEngine = new GameEngine(this, size);

        setContentView(gameEngine);

    }

    @Override
    protected void onResume(){
        super.onResume();
        gameEngine.resume();
    }

    @Override
    protected void onPause(){
        super.onPause();
        gameEngine.pause();
    }
}
